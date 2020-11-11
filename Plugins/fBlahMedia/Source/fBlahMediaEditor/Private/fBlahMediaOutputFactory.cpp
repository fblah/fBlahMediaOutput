// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

#include "fBlahMediaOutputFactory.h"
#include "fBlahMediaOutput.h"
#include "AssetTypeCategories.h"


/* UfBlahMediaOutputFactory structors
 *****************************************************************************/

UfBlahMediaOutputFactory::UfBlahMediaOutputFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UfBlahMediaOutput::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}


/* UFactory overrides
 *****************************************************************************/

UObject* UfBlahMediaOutputFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UfBlahMediaOutput>(InParent, InClass, InName, Flags);
}


uint32 UfBlahMediaOutputFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Media;
}


bool UfBlahMediaOutputFactory::ShouldShowInNewMenu() const
{
	return true;
}
