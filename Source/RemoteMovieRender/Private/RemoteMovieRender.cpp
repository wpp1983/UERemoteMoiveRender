// Copyright Epic Games, Inc. All Rights Reserved.

#include "RemoteMovieRender.h"

#include "ISettingsModule.h"
#include "RemoteMovieRenderStyle.h"
#include "RemoteMovieRenderCommands.h"
#include "RMP_Settings.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName RemoteMovieRenderTabName("RemoteMovieRender");

#define LOCTEXT_NAMESPACE "FRemoteMovieRenderModule"

void FRemoteMovieRenderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FRemoteMovieRenderStyle::Initialize();
	FRemoteMovieRenderStyle::ReloadTextures();

	FRemoteMovieRenderCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FRemoteMovieRenderCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FRemoteMovieRenderModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FRemoteMovieRenderModule::RegisterMenus));

	RegisterSettings();
}

void FRemoteMovieRenderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FRemoteMovieRenderStyle::Shutdown();

	FRemoteMovieRenderCommands::Unregister();
	
	UnregisterSettings();
}

void FRemoteMovieRenderModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FRemoteMovieRenderModule::PluginButtonClicked()")),
							FText::FromString(TEXT("RemoteMovieRender.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FRemoteMovieRenderModule::RegisterSettings()
{
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");

	SettingsModule.RegisterSettings("Project", "Plugins", "RMP",
		LOCTEXT("ProjectSettings_Label", "RMP"),
		LOCTEXT("ProjectSettings_Description", "Configure project-wide defaults for theRMP."),
		GetMutableDefault<URMP_Settings>()
	);
}

void FRemoteMovieRenderModule::UnregisterSettings()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "RMP");
	}
}

void FRemoteMovieRenderModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FRemoteMovieRenderCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FRemoteMovieRenderCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRemoteMovieRenderModule, RemoteMovieRender)