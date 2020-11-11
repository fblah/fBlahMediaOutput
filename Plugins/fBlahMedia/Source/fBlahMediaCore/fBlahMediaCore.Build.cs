// Copyright (c) 2019-2020 Ajit Christopher  D'Monte (fBlah)

using UnrealBuildTool;

public class fBlahMediaCore : ModuleRules
{
	public fBlahMediaCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"MediaIOCore",
				"Sockets", 
				"Networking",
				"WebSockets",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);

		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}

		PublicDefinitions.Add("FBLAH_MEDIA_PLUGIN_NAME=TEXT(\"fBlahMedia\")");
	}
}
