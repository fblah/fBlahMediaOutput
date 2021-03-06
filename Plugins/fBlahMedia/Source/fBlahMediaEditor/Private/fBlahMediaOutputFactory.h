// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "fBlahMediaOutputFactory.generated.h"

/**
 * Implements a factory for UfBlahMediaOutputFactory objects.
 */
UCLASS()
class FBLAHMEDIAEDITOR_API UfBlahMediaOutputFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual uint32 GetMenuCategories() const override;
	virtual bool ShouldShowInNewMenu() const override;
};
