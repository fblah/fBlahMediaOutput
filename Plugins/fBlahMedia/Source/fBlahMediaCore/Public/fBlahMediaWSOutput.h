// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

#pragma once

#include "MediaOutput.h"

#include "Engine/RendererSettings.h"
#include "ImageWriteBlueprintLibrary.h"

#include "fBlahMediaWSOutput.generated.h"

/** Texture format supported by UfBlahMediaOutput. */
UENUM()
enum class EfBlahMediaWSOutputPixelFormat
{
	B8G8R8A8					UMETA(DisplayName = "8bit RGBA"),
};


/**
 * Output information for a fBlah Media WS media capture.
 * @note	'Frame Buffer Pixel Format' must be set to RGBA8
 * @note	'Enable alpha channel support in post-processing' must be set to 'Allow through tonemapper' to enabled the Key.
 */
UCLASS(BlueprintType)
class FBLAHMEDIACORE_API UfBlahMediaWSOutput : public UMediaOutput
{
	GENERATED_BODY()

public:
	UfBlahMediaWSOutput();

public:
	/** Options on how to save the images. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Media")
	bool bFixAlpha;

	/** The base file name of the images. The frame number will be append to the base file name. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Socket", meta=(RelativePath))
	FString Address;

	/** Use the default back buffer size or specify a specific size to capture. */
	UPROPERTY(BlueprintReadWrite, Category="Media")
	bool bOverrideDesiredSize;

	/** Use the default back buffer size or specify a specific size to capture. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Media", meta=(EditCondition="bOverrideDesiredSize"))
	FIntPoint DesiredSize;

	/** Use the default back buffer pixel format or specify a specific the pixel format to capture. */
	UPROPERTY(BlueprintReadWrite, Category="Media")
	bool bOverridePixelFormat;

	/** Use the default back buffer pixel format or specify a specific the pixel format to capture. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Media", meta=(EditCondition="bOverridePixelFormat"))
	EfBlahMediaWSOutputPixelFormat DesiredPixelFormat;

	//~ UMediaOutput interface
public:
	virtual bool Validate(FString& FailureReason) const override;
	virtual FIntPoint GetRequestedSize() const override;
	virtual EPixelFormat GetRequestedPixelFormat() const override;
	virtual EMediaCaptureConversionOperation GetConversionOperation(EMediaCaptureSourceType InSourceType) const override;

protected:
	virtual UMediaCapture* CreateMediaCaptureImpl() override;
	//~ End UMediaOutput interface
};
