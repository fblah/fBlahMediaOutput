// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

#include "fBlahMediaWSCapture.h"

#include "fBlahMediaWSOutput.h"
#include "Modules/ModuleManager.h"

#include "WebSockets/Public/WebSocketsModule.h"
#include "Async/Async.h"
#include <string>

DEFINE_LOG_CATEGORY(LogfBlahMediaWSCapture);

class FWSWrapperUtility
{
public:
	static TFuture<void> RunLambdaOnBackGroundThread(TFunction< void()> InFunction)
	{
		return Async(EAsyncExecution::Thread, InFunction);
	}
};

void UfBlahMediaWSCapture::OnFrameCaptured_RenderingThread(const FCaptureBaseData& InBaseData, TSharedPtr<FMediaCaptureUserData, ESPMode::ThreadSafe> InUserData, void* InBuffer, int32 Width, int32 Height)
{
	FScopeLock ScopeLock(&RenderThreadCriticalSection);
	
	if (WebSocket)
	{
		if (WebSocket->IsConnected())
		{	
			const char* sdata = static_cast<const char*>(InBuffer);
			std::string data (sdata, sdata + ( 4 * Width * Height));
			UE_LOG(LogfBlahMediaWSCapture, Log, TEXT("%d"), data.length());
			//WebSocket->Send(reinterpret_cast<uint8*>(InBuffer), 4 * Width * Height);
			WebSocket->Send(FString(UTF8_TO_TCHAR(data.c_str())));
		}
	}
	SetState(EMediaCaptureState::Capturing);
}


bool UfBlahMediaWSCapture::CaptureSceneViewportImpl(TSharedPtr<FSceneViewport>& InSceneViewport)
{
	UE_LOG(LogfBlahMediaWSCapture, Log, TEXT("SV fBlah Media WS"));

	CacheMediaOutputValues();

	SetState(EMediaCaptureState::Capturing);
	return true;
}


bool UfBlahMediaWSCapture::CaptureRenderTargetImpl(UTextureRenderTarget2D* InRenderTarget)
{
	UE_LOG(LogfBlahMediaWSCapture, Log, TEXT("RT fBlah Media WS"));

	CacheMediaOutputValues();

	SetState(EMediaCaptureState::Capturing);
	return true;
}


void UfBlahMediaWSCapture::CacheMediaOutputValues()
{
	UfBlahMediaWSOutput* fBlahMediaWSOutput = CastChecked<UfBlahMediaWSOutput>(MediaOutput);
	FWebSocketsModule& WebSocketModule = FWebSocketsModule::Get();

	if (fBlahMediaWSOutput)
	{
		if (WebSocket)
		{
			WebSocket->Close();
			Connected = false;
		}

		bShouldListen = true;

		//Start a lambda thread to handle data
		ServerFinishedFuture = FWSWrapperUtility::RunLambdaOnBackGroundThread([&]()
		{
			while (bShouldListen)
			{
				//sleep for 100microns
				float sleep_time = 1;//0.0001;
				if (Connected) 
				{
					sleep_time = 0.1;
				}
				else
				{
					if (WebSocket)
					{
						WebSocket->Connect();
					}
					else
					{
						WebSocket = WebSocketModule.CreateWebSocket(fBlahMediaWSOutput->Address);

						WebSocket->OnConnectionError().AddUObject(this, &UfBlahMediaWSCapture::OnWebsocketConnectError);
						WebSocket->OnConnected().AddUObject(this, &UfBlahMediaWSCapture::OnWebsocketConnected);
						WebSocket->OnClosed().AddUObject(this, &UfBlahMediaWSCapture::OnWebsocketClose);
						WebSocket->OnMessage().AddUObject(this, &UfBlahMediaWSCapture::OnWebsocketMessage);

						WebSocket->Connect();
					}
				}				
				FPlatformProcess::Sleep(sleep_time);
			}//end while
			if (WebSocket)
			{
				WebSocket->Close();
			}
		});
	}
}

void UfBlahMediaWSCapture::StopCaptureImpl(bool bAllowPendingFrameToBeProcess)
{
	if (!bAllowPendingFrameToBeProcess)
	{
		{
			// Prevent the rendering thread from copying while we are stopping the capture.
			FScopeLock ScopeLock(&RenderThreadCriticalSection);
			bShouldListen = false;
			if (WebSocket)
			{
				WebSocket->Close();
			}
			else {
				UE_LOG(LogfBlahMediaWSCapture, Warning, TEXT("Close failed! WebSocket is Null!"));
			}
		}

		//RestoreViewportTextureAlpha(GetCapturingSceneViewport());
	}
}


void UfBlahMediaWSCapture::OnWebsocketConnected()
{
	if (WebSocket)
	{
		Connected = true;
	}
	else {
		UE_LOG(LogfBlahMediaWSCapture, Warning, TEXT("No WebSocket, tried to connect"));
	}
}

void UfBlahMediaWSCapture::OnWebsocketConnectError(const FString& ErrorMessage)
{
	if (WebSocket)
	{
		UE_LOG(LogfBlahMediaWSCapture, Warning, TEXT("Connection Error, trying again"));
	}
	else {
		UE_LOG(LogfBlahMediaWSCapture, Warning, TEXT("No WebSocket, Connection Error"));
	}
}

void UfBlahMediaWSCapture::OnWebsocketClose(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	if (WebSocket)
	{
		Connected = false;
		UE_LOG(LogfBlahMediaWSCapture, Warning, TEXT("WebSocket closed: %s %s"), *FString::FromInt(StatusCode),*Reason);
	}
	else {
		UE_LOG(LogfBlahMediaWSCapture, Warning, TEXT("No WebSocket Can't close"));
	}
}

void UfBlahMediaWSCapture::OnWebsocketMessage(const FString& Message)
{
	if (WebSocket)
	{
		
	}
	else {
		UE_LOG(LogfBlahMediaWSCapture, Warning, TEXT("Incoming message no websocket"));
	}
}