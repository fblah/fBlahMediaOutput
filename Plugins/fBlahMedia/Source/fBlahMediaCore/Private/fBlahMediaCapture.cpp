// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

#include "fBlahMediaCapture.h"

#include "fBlahMediaOutput.h"
#include "ImageWriteQueue.h"
#include "ImageWriteTask.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"

#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Common/TcpSocketBuilder.h"
#include "Async/Async.h"
#include "Engine/NetDriver.h"
#include <algorithm>

DEFINE_LOG_CATEGORY(LogfBlahMediaCapture);

class FTCPWrapperUtility
{
public:
	static TFuture<void> RunLambdaOnBackGroundThread(TFunction< void()> InFunction)
	{
		return Async(EAsyncExecution::Thread, InFunction);
	}
};

void UfBlahMediaCapture::OnFrameCaptured_RenderingThread(const FCaptureBaseData& InBaseData, TSharedPtr<FMediaCaptureUserData, ESPMode::ThreadSafe> InUserData, void* InBuffer, int32 Width, int32 Height)
{
	FScopeLock ScopeLock(&RenderThreadCriticalSection);
	//UE_LOG(LogfBlahMediaCapture, Error, TEXT("Render"));
	int32 BytesSent = 0;
	TArray<FTCPClient> AllClients;
	bool Success = true;
	bool SentOnce = false;
	int dataLength = 8 + (4 * Width * Height);
	char* header = "fblahad";
	uint8* result = new uint8[dataLength];
	uint8* data = reinterpret_cast<uint8*>(InBuffer);
	std::copy(header, header + 8, result);
	std::copy(data, data + (4 * Width * Height), result + 8);
	Clients.GenerateValueArray(AllClients);
	//for (FTCPClient Client : AllClients)
	for (auto& pair : Clients)
	{
		FTCPClient *Client = &pair.Value;
		if (Client->Connected)
		{
			ESocketType SocketType = Client->Socket->GetSocketType();
			if (!(SocketType == SOCKTYPE_Datagram || SocketType == SOCKTYPE_Streaming))
			{
				continue;
			}
			
			bool Sent = Client->Socket->Send(result, dataLength, BytesSent);
			
			if (!Sent)
			{
				UE_LOG(LogfBlahMediaCapture, Warning, TEXT("No send"));	
				Client->Fails++;
				if (Client->Fails > 600) 
				{
					Client->Connected = false;
				}				
			}
			else
			{
				Client->Fails = 0;
				SentOnce = true;
			}

			Success = Sent && Success;
		}
	}
	if (!SentOnce) 
	{
		//SetState(EMediaCaptureState::Preparing);
	}
	else
	{
		SetState(EMediaCaptureState::Capturing);
	}
	delete[] result;
}


bool UfBlahMediaCapture::CaptureSceneViewportImpl(TSharedPtr<FSceneViewport>& InSceneViewport)
{

	UE_LOG(LogfBlahMediaCapture, Log, TEXT("SV fBlah Media"));
	
	CacheMediaOutputValues();

	SetState(EMediaCaptureState::Capturing);
	return true;
}


bool UfBlahMediaCapture::CaptureRenderTargetImpl(UTextureRenderTarget2D* InRenderTarget)
{
	UE_LOG(LogfBlahMediaCapture, Log, TEXT("RT fBlah Media"));

	CacheMediaOutputValues();

	SetState(EMediaCaptureState::Capturing);
	return true;
}


void UfBlahMediaCapture::CacheMediaOutputValues()
{
	UfBlahMediaOutput* fBlahMediaOutput = CastChecked<UfBlahMediaOutput>(MediaOutput);
	
	if (fBlahMediaOutput) 
	{
		//FString address, port;

		if (ListenSocket)
		{
			ListenSocket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
			ListenSocket = nullptr;
			for (auto ClientPair : Clients)
			{
				FTCPClient Client = ClientPair.Value;
				Client.Socket->Close();
				Clients.Remove(Client.Address);
				UE_LOG(LogfBlahMediaCapture, Log, TEXT("Client disconnected"));
			}
		}
		

		//fBlahMediaOutput->IPString.Split(":", &address, &port);
		FIPv4Address Addr;
		FIPv4Address::Parse(TEXT("0.0.0.0"), Addr);
		//FIPv4Endpoint Endpoint(Addr, FCString::Atoi(*port));
		FIPv4Endpoint Endpoint(Addr, fBlahMediaOutput->TCPPort);
		FString SocketName = "fBlah Media Socket";
		int32 BufferMaxSize = 10 * 1024 * 1024;
		ListenSocket = FTcpSocketBuilder(*SocketName)
			.AsNonBlocking()
			.AsReusable()
			.BoundToEndpoint(Endpoint)
			.WithReceiveBufferSize(BufferMaxSize)
			.WithSendBufferSize(BufferMaxSize);
		ListenSocket->SetReceiveBufferSize(BufferMaxSize, BufferMaxSize);
		ListenSocket->SetSendBufferSize(BufferMaxSize, BufferMaxSize);
		ListenSocket->Listen(8);
		//m_TCP_Listener = new FTcpListener(*Socket);
		//m_TCP_Listener->OnConnectionAccepted().BindRaw(this, &UfBlahMediaCapture::HandleListenerConnectionAccepted);

		bShouldListen = true;

		//Start a lambda thread to handle data
		ServerFinishedFuture = FTCPWrapperUtility::RunLambdaOnBackGroundThread([&]()
		{
			uint32 BufferSize = 0;
			TArray<uint8> ReceiveBuffer;
			TArray<FTCPClient> ClientsDisconnected;

			FDateTime LastPing = FDateTime::Now();

			while (bShouldListen)
			{
				//Do we have clients trying to connect? connect them
				bool bHasPendingConnection;
				ListenSocket->HasPendingConnection(bHasPendingConnection);
				if (bHasPendingConnection)
				{
					TSharedPtr<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
					FSocket* Client = ListenSocket->Accept(*Addr, TEXT("tcp-client"));

					const FString AddressString = Addr->ToString(true);

					FTCPClient ClientItem = FTCPClient();
					ClientItem.Address = AddressString;
					ClientItem.Socket = Client;
					ClientItem.Connected = true;
					ClientItem.Fails = 0;

					Clients.Add(AddressString, ClientItem);	//todo: balance this with remove when clients disconnect
					UE_LOG(LogfBlahMediaCapture, Log, TEXT("Client connected"));
					//AsyncTask(ENamedThreads::GameThread, [&, AddressString]()
					//{
						//OnClientConnected.Broadcast(AddressString);
					//});
				}

				//Check each endpoint for data
				for (auto ClientPair : Clients)
				{
					FTCPClient Client = ClientPair.Value;
					
					if (!Client.Connected)
					{
						ClientsDisconnected.Add(Client);
						continue;
					}

					ESocketType SocketType = Client.Socket->GetSocketType();
					if (!(SocketType == SOCKTYPE_Datagram || SocketType == SOCKTYPE_Streaming))
					{
						Client.Connected = false;
						continue;
					}
					//Did we disconnect? Note that this almost never changed from connected due to engine bug, instead it will be caught when trying to send data
					ESocketConnectionState ConnectionState = Client.Socket->GetConnectionState();
					if (ConnectionState != ESocketConnectionState::SCS_Connected)
					{
						Client.Connected = false;
						//ClientsDisconnected.Add(Client);
						continue;
					}
					

					if (Client.Socket->HasPendingData(BufferSize))
					{
						ReceiveBuffer.SetNumUninitialized(BufferSize);
						int32 Read = 0;

						Client.Socket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read);
						TArray<uint8> ReceiveBufferGT;
						ReceiveBufferGT.Append(ReceiveBuffer);
					
					}
				}

				//Handle disconnections
				if (ClientsDisconnected.Num() > 0)
				{
					for (FTCPClient& ClientToRemove : ClientsDisconnected)
					{
						const FString Address = ClientToRemove.Address;
						Clients.Remove(Address);
						UE_LOG(LogfBlahMediaCapture, Log, TEXT("Client disconnected"));
						/*AsyncTask(ENamedThreads::GameThread, [this, Address]()
						{
							
						});*/
					}
					ClientsDisconnected.Empty();
				}

				//sleep for 100microns
				FPlatformProcess::Sleep(0.0001);
			}//end while

			for (auto ClientPair : Clients)
			{
				ClientPair.Value.Socket->Close();
			}
			Clients.Empty();

			//Server ended
			AsyncTask(ENamedThreads::GameThread, [&]()
			{
				Clients.Empty();
				//OnListenEnd.Broadcast();
			});
		});
	}
}

void UfBlahMediaCapture::StopCaptureImpl(bool bAllowPendingFrameToBeProcess)
{
	if (!bAllowPendingFrameToBeProcess)
	{
		{
			// Prevent the rendering thread from copying while we are stopping the capture.
			FScopeLock ScopeLock(&RenderThreadCriticalSection);
			/*if (m_TCP_Listener)
			{
				if (m_TCP_Listener->IsActive())
				{
					m_TCP_Listener->Stop();
					delete m_TCP_Listener;
					m_TCP_Listener = nullptr;
				}
			}

			for (auto socket : Connections)
			{
				socket->Close();
			}
			Connections.Empty();*/
			if (ListenSocket)
			{
				bShouldListen = false;
				ServerFinishedFuture.Get();

				ListenSocket->Close();
				ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
				ListenSocket = nullptr;

				//OnListenEnd.Broadcast();
			}

			for (auto ClientPair : Clients)
			{
				FTCPClient Client = ClientPair.Value;
				Client.Socket->Close();
				Clients.Remove(Client.Address);
				UE_LOG(LogfBlahMediaCapture, Log, TEXT("Client disconnected"));
				
			}
		}

		//RestoreViewportTextureAlpha(GetCapturingSceneViewport());
	}
}

bool UfBlahMediaCapture::HandleListenerConnectionAccepted(FSocket* ClientSocket, const FIPv4Endpoint& ClientEndpoint)
{
	//Connections.Add(ClientSocket);

	return true;
}