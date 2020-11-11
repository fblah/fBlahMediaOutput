// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

using UnrealBuildTool;

public class fBlahMediaEditor : ModuleRules
{
	public fBlahMediaEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"MediaIOCore",
					"MediaIOEditor",
					"Slate",
					"SlateCore",
					"TimeManagement",
					"UnrealEd",
					"fBlahMediaCore",
				});
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				// ... add private dependencies that you statically link with here ...	
			}
			);
	}
}
