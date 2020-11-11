// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

#include "fBlahMediaWSOutput.h"

#include "fBlahMediaWSCapture.h"
#include "UnrealEngine.h"

#include "Interfaces/IPv4/IPv4Address.h"


UfBlahMediaWSOutput::UfBlahMediaWSOutput()
	: Super()
{
	Address = "ws://127.0.0.1:6000";
}


bool UfBlahMediaWSOutput::Validate(FString& OutFailureReason) const
{
	//FIPv4Address Addr;
	//FIPv4Address::Parse(IPString, Addr);
	if (!Super::Validate(OutFailureReason))
	{
		return false;
	}

	//if (!Addr.IsLinkLocal())
	//{
	//	OutFailureReason = FString::Printf(TEXT("Can't validate MediaOutput '%s'. The IP address and port is not local or valid"), *GetName());
	//	return false;
	//}

	if (GetRequestedPixelFormat() != PF_B8G8R8A8 )
	{
		OutFailureReason = FString::Printf(TEXT("Can't validate MediaOutput '%s'. Only Supported format is RTF RGBA8 (PF_B8G8R8A8)"), *GetName());
		return false;
	}

	return true;
}


FIntPoint UfBlahMediaWSOutput::GetRequestedSize() const
{
	if (bOverrideDesiredSize)
	{
		return DesiredSize;
	}

	return UMediaOutput::RequestCaptureSourceSize;
}


EPixelFormat UfBlahMediaWSOutput::GetRequestedPixelFormat() const
{
	return PF_B8G8R8A8;
}


EMediaCaptureConversionOperation UfBlahMediaWSOutput::GetConversionOperation(EMediaCaptureSourceType InSourceType) const
{
	// All formats supporting alpha
	if (bFixAlpha)
	{
		// We invert alpha only when alpha channel as valid data when used with "passthrough tone mapper" or using a render target, otherwise we force it to 1.0f.
		static const auto CVarPropagateAlpha = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PostProcessing.PropagateAlpha"));
		EAlphaChannelMode::Type PropagateAlpha = EAlphaChannelMode::FromInt(CVarPropagateAlpha->GetValueOnAnyThread());
		if ((PropagateAlpha == EAlphaChannelMode::AllowThroughTonemapper) || (InSourceType == EMediaCaptureSourceType::RENDER_TARGET))
		{
			return EMediaCaptureConversionOperation::INVERT_ALPHA;
		}
		else
		{
			return EMediaCaptureConversionOperation::SET_ALPHA_ONE;
		}
	}
	else
	{
		return EMediaCaptureConversionOperation::NONE;
	}
}


UMediaCapture* UfBlahMediaWSOutput::CreateMediaCaptureImpl()
{
	UMediaCapture* Result = NewObject<UfBlahMediaWSCapture>();
	if (Result)
	{
		UE_LOG(LogfBlahMediaCore, Log, TEXT("Create fBlah WS Media"));
		Result->SetMediaOutput(this);
	}
	return Result;
}
