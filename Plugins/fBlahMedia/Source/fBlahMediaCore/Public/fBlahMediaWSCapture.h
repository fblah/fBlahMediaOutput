// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

#pragma once

#include "MediaCapture.h"

#include "ImageWriteTypes.h"
#include "WebSockets/Public/IWebSocket.h"

#include "fBlahMediaWSCapture.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogfBlahMediaWSCapture, Log, All);


/**
 * 
 */
UCLASS()
class FBLAHMEDIACORE_API UfBlahMediaWSCapture : public UMediaCapture
{
	GENERATED_BODY()

protected:
	virtual void OnFrameCaptured_RenderingThread(const FCaptureBaseData& InBaseData, TSharedPtr<FMediaCaptureUserData, ESPMode::ThreadSafe> InUserData, void* InBuffer, int32 Width, int32 Height) override;
	virtual bool CaptureSceneViewportImpl(TSharedPtr<FSceneViewport>& InSceneViewport) override;
	virtual bool CaptureRenderTargetImpl(UTextureRenderTarget2D* InRenderTarget) override;
	virtual void StopCaptureImpl(bool bAllowPendingFrameToBeProcess) override;

private:
	void CacheMediaOutputValues();
	void OnWebsocketConnected();
	void OnWebsocketConnectError(const FString& ErrorMessage);
	void OnWebsocketClose(int32 StatusCode, const FString& Reason, bool bWasClean);
	void OnWebsocketMessage(const FString& Message);

private:
	
	FCriticalSection RenderThreadCriticalSection;
	TSharedPtr<IWebSocket> WebSocket;	
	FThreadSafeBool bShouldListen;
	bool Connected;
	TFuture<void> ServerFinishedFuture;
};
