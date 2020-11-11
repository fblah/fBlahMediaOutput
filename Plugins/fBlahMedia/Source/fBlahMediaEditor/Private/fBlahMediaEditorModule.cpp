// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

#include "fBlahMediaEditorModule.h"

#include "Interfaces/IPluginManager.h"

#include "Brushes/SlateImageBrush.h"
#include "Misc/Paths.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorDelegates.h"
#include "PropertyEditorModule.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FfBlahMediaEditorModule"

DEFINE_LOG_CATEGORY(LogfBlahMediaEditor);

class FfBlahMediaEditorModule : public IModuleInterface
{
	TUniquePtr<FSlateStyleSet> StyleInstance;

	virtual void StartupModule() override
	{
		RegisterCustomizations();
		RegisterStyle();
	}

	virtual void ShutdownModule() override
	{
		if (!UObjectInitialized() && !IsEngineExitRequested())
		{
			UnregisterStyle();
			UnregisterCustomizations();
		}
	}

private:
	/** Register details view customizations. */
	void RegisterCustomizations()
	{
		

	}

	/** Unregister details view customizations. */
	void UnregisterCustomizations()
	{
		
	}

	/** Register slate style */
	void RegisterStyle()
	{
		FString ContentDir = IPluginManager::Get().FindPlugin(FBLAH_MEDIA_PLUGIN_NAME)->GetContentDir() + "/";
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleInstance->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

		StyleInstance = MakeUnique<FSlateStyleSet>(TEXT("fBlahMediaIOStyle"));
		StyleInstance->SetContentRoot(ContentDir / TEXT("Editor/Icons/"));

		StyleInstance->Set("ClassThumbnail.fBlahMediaOutput", new IMAGE_BRUSH("fBlahMediaOutput_64x", FVector2D(64, 64)));
		StyleInstance->Set("ClassIcon.fBlahMediaOutput", new IMAGE_BRUSH("fBlahMediaOutput_16x", FVector2D(16, 16)));
		StyleInstance->Set("ClassThumbnail.fBlahMediaWSOutput", new IMAGE_BRUSH("fBlahMediaOutput_64x", FVector2D(64, 64)));
		StyleInstance->Set("ClassIcon.fBlahMediaWSOutput", new IMAGE_BRUSH("fBlahMediaOutput_16x", FVector2D(16, 16)));

#undef IMAGE_BRUSH

		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance.Get());
	}

	/** Unregister slate style */
	void UnregisterStyle()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance.Get());
	}
};

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FfBlahMediaEditorModule, fBlahMediaEditor)