// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

#pragma once

#include "MediaCapture.h"

#include "ImageWriteTypes.h"
#include "Common/TcpListener.h"

#include "fBlahMediaCapture.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogfBlahMediaCapture, Log, All);

struct FTCPClient
{
	FSocket* Socket;
	FString Address;
	bool Connected;
	int Fails;

	bool operator==(const FTCPClient& Other)
	{
		return Address == Other.Address;
	}
};

/**
 * 
 */
UCLASS()
class FBLAHMEDIACORE_API UfBlahMediaCapture : public UMediaCapture
{
	GENERATED_BODY()

protected:
	virtual void OnFrameCaptured_RenderingThread(const FCaptureBaseData& InBaseData, TSharedPtr<FMediaCaptureUserData, ESPMode::ThreadSafe> InUserData, void* InBuffer, int32 Width, int32 Height) override;
	virtual bool CaptureSceneViewportImpl(TSharedPtr<FSceneViewport>& InSceneViewport) override;
	virtual bool CaptureRenderTargetImpl(UTextureRenderTarget2D* InRenderTarget) override;
	virtual void StopCaptureImpl(bool bAllowPendingFrameToBeProcess) override;

private:
	void CacheMediaOutputValues();

private:
	bool HandleListenerConnectionAccepted(FSocket* Socket, const FIPv4Endpoint& Endpoint);
	
	FCriticalSection RenderThreadCriticalSection;	

	TMap<FString, FTCPClient> Clients;
	FSocket* ListenSocket;
	TSharedPtr<FInternetAddr> RemoteAdress;
	FThreadSafeBool bShouldListen;
	TFuture<void> ServerFinishedFuture;

};
