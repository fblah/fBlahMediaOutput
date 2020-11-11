// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

#include "fBlahMediaWSOutputFactory.h"
#include "fBlahMediaWSOutput.h"
#include "AssetTypeCategories.h"


/* UfBlahMediaWSOutputFactory structors
 *****************************************************************************/

UfBlahMediaWSOutputFactory::UfBlahMediaWSOutputFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UfBlahMediaWSOutput::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}


/* UFactory overrides
 *****************************************************************************/

UObject* UfBlahMediaWSOutputFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UfBlahMediaWSOutput>(InParent, InClass, InName, Flags);
}


uint32 UfBlahMediaWSOutputFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Media;
}


bool UfBlahMediaWSOutputFactory::ShouldShowInNewMenu() const
{
	return true;
}
