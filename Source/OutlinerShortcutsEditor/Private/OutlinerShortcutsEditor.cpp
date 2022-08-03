// Copyright Voulz 2021-2022. All Rights Reserved.

#include "OutlinerShortcutsEditor.h"

#include "OutlinerShortcutsEditorDefinitions.h"
#include "LevelEditor.h"
#include "ISceneOutliner.h"
#include "Widgets/Views/STreeView.h"
#include "SceneOutlinerFwd.h"
#include "SSceneOutliner.h"
#include "SceneOutlinerFwd.h"

#include "Editor.h"
#include "SceneOutlinerEvents.h"
#include "ISettingsModule.h"
#include "ISettingsContainer.h"
#include "OutlinerShortcutsSettings.h"

#define LOCTEXT_NAMESPACE "FOutlinerShortcutsEditorModule"

void FOutlinerShortcutsEditorModule::StartupModule()
{
	HERE;

	// -- Register Settings
	RegisterSettings();

	// -- Register Commands
	MapCommands();

	// -- Register sub-modules
	if (!IsRunningCommandlet())
	{
		AddModuleListeners();
		for (int32 i = 0; i < ModuleListeners.Num(); ++i)
		{
			ModuleListeners[i]->OnStartupModule();
		}
	}

	LOG("`FOutlinerShortcutsEditorModule` Loaded");
}

void FOutlinerShortcutsEditorModule::ShutdownModule()
{
	HERE;

	OutlinerShortcutsCommands::Unregister();

	for (int32 i = 0; i < ModuleListeners.Num(); ++i)
	{
		ModuleListeners[i]->OnShutdownModule();
	}

	LOG("`FOutlinerShortcutsEditorModule` Unloaded");
}


void FOutlinerShortcutsEditorModule::RegisterSettings()
{
	HERE;
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");

	SettingsModule.RegisterSettings("Editor", "Plugins", "Outliner Shortcuts",
		LOCTEXT("EditorSettings_Label", "Outliner Shortcuts"),
		LOCTEXT("EditorSettings_Description", "Configure the default settings for Outliner Shortcuts."),
		GetMutableDefault<UOutlinerShortcutsEditorSettings>()
	);

	LOG_H("Settings Registered");
}

void FOutlinerShortcutsEditorModule::UnregisterSettings()
{
	HERE;
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "Outliner Shortcuts");
	}
	else
	{
		WARN_H("`Settings Module` returned null");
	}

	LOG_H("Settings Unregistered");
}

void FOutlinerShortcutsEditorModule::AddModuleListeners()
{
	ModuleListeners.Add(MakeShareable(new FSceneOutlinerEvents));
}

void FOutlinerShortcutsEditorModule::MapCommands()
{
	HERE;
	// initialize and register the commands
	CommandList = MakeShareable(new FUICommandList);
	OutlinerShortcutsCommands::Register();

	// map the commands to actions
	const OutlinerShortcutsCommands& Commands = OutlinerShortcutsCommands::Get();

	CommandList->MapAction(Commands.SceneOutlinerCollapseAll, FExecuteAction::CreateLambda(
		[]() {
			FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAll();
		}));
	CommandList->MapAction(Commands.SceneOutlinerCollapseToRoot, FExecuteAction::CreateLambda(
		[]() {
			FOutlinerShortcutsEditorModule::SceneOutlinerCollapseToRoot();
		}));
	CommandList->MapAction(Commands.SceneOutlinerExpandAll, FExecuteAction::CreateLambda(
		[]() {
			FOutlinerShortcutsEditorModule::SceneOutlinerExpandAll();
		}));

	// register the commands to the Level Editor to make them available to shortcuts
	FLevelEditorModule* LevelEditorModule = FModuleManager::LoadModulePtr<FLevelEditorModule>("LevelEditor");
	if (LevelEditorModule)
	{
		LevelEditorModule->GetGlobalLevelEditorActions()->Append(CommandList.ToSharedRef());
	}
	else
	{
		ERROR_H("Not able to load `LevelEditor` module");
	}

}

bool FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAll()
{
	HERE_D;

	if (SSceneOutliner* SOutliner = GetSSceneOutliner())
	{
		if (IsSceneOutlinerReady(SOutliner))
		{
			SOutliner->CollapseAll();
			return true;
		}
		WARN_H("SceneOutliner is not ready");
	}
	else
	{
		ERROR_H("Not able to get `SSceneOutliner`");
	}

	return false;
}

bool FOutlinerShortcutsEditorModule::SceneOutlinerCollapseToRoot()
{
	HERE_D;

	if (SSceneOutliner* SOutliner = GetSSceneOutliner())
	{
		if (IsSceneOutlinerReady(SOutliner))
		{
			SOutliner->CollapseAll();
			TArray<FSceneOutlinerTreeItemPtr> RootItems = GetSceneOutlinerRootItems(SOutliner);
			for (FSceneOutlinerTreeItemPtr Item : RootItems)
			{
				if (Item)
				{
					SOutliner->SetItemExpansion(Item, true);
				}
			}
			return true;
		}
		WARN_H("SceneOutliner is not ready");
	}
	else
	{
		ERROR_H("Not able to get `SSceneOutliner`");
	}

	return false;
}

bool FOutlinerShortcutsEditorModule::SceneOutlinerExpandAll()
{
	HERE_D;

	if (SSceneOutliner* SOutliner = GetSSceneOutliner())
	{
		if (IsSceneOutlinerReady(SOutliner))
		{
			SOutliner->ExpandAll();
			return true;
		}
		WARN_H("SceneOutliner is not ready");
	}
	else
	{
		ERROR_H("Not able to get `SSceneOutliner`");
	}

	return false;
}

ISceneOutliner* FOutlinerShortcutsEditorModule::GetISceneOutliner()
{
	FLevelEditorModule* LevelEditorModule = FModuleManager::LoadModulePtr<FLevelEditorModule>("LevelEditor");
	if (LevelEditorModule)
	{
		if (ILevelEditor* LevelEditor = LevelEditorModule->GetFirstLevelEditor().Get())
		{
			if (ISceneOutliner* SceneOutliner = LevelEditor->GetSceneOutliner().Get())
			{
				return SceneOutliner;
			}
			else
			{
				ERROR_H("Not able to get `SceneOutliner`");
			}
		}
		else
		{
			//LevelEditorModule->leve
			ERROR_H("Not able to get `LevelEditor`");
			// TODO: check LevelEditorCreatedEvent
		}
	}
	else
	{
		ERROR_H("Not able to load `LevelEditor` module");
	}

	return nullptr;
}

SSceneOutliner* FOutlinerShortcutsEditorModule::GetSSceneOutliner()
{
	if (ISceneOutliner* SceneOutliner = GetISceneOutliner())
	{
		if (SSceneOutliner* SOutliner = (SSceneOutliner*)SceneOutliner)
		{
			return SOutliner;
		}
		else
		{
			ERROR_H("Not able to convert `ISceneOutliner` to `SSceneOutliner`");
		}
	}
	else
	{
		ERROR_H("Not able to get `SceneOutliner`");
	}

	return nullptr;
}

UWorld* FOutlinerShortcutsEditorModule::GetCurrentEditorWorld()
{
	return GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
}

FSceneOutlinerTreeItemPtr FOutlinerShortcutsEditorModule::GetWorldTreetItemPtr(SSceneOutliner* SceneOutliner)
{
	//HERE;
	if (SceneOutliner)
	{
		if (UWorld* World = GetCurrentEditorWorld())
		{
			FObjectKey ID(World);
			auto TreeItemPtr = SceneOutliner->GetTreeItem(ID);
			return TreeItemPtr;
		}
		else
		{
			WARN_H("Editor World is null");
		}
	}
	else
	{
		WARN_H("`SceneOutliner` is null");
	}
	return FSceneOutlinerTreeItemPtr(nullptr);
}

bool FOutlinerShortcutsEditorModule::IsSceneOutlinerReady(SSceneOutliner* SceneOutliner)
{
	//HERE;
	return GetWorldTreetItemPtr(SceneOutliner).IsValid();
}


TArray<FSceneOutlinerTreeItemPtr> FOutlinerShortcutsEditorModule::GetSceneOutlinerRootItems(SSceneOutliner* SOutliner)
{
	TArray<FSceneOutlinerTreeItemPtr> RootItems;

	if (SOutliner)
	{
		// The only way so far to access all the items of the TreeView is via "SetSelection"
		// So we are first saving the selection, then going through all of it via SetSelection, returning the same results

		TArray<FSceneOutlinerTreeItemPtr> Selection = SOutliner->GetSelectedItems();

		// As we are not able to compare ISceneOutlinerTreeItems, we get their IDs.
		TSet<FSceneOutlinerTreeItemID> SelectionID;
		for (FSceneOutlinerTreeItemPtr SelectedItemPtr : Selection)
		{
			if (ISceneOutlinerTreeItem* SelectedItem = SelectedItemPtr.Get())
			{
				SelectionID.Add(SelectedItem->GetID());
			}
		}


		//WARN("Setting Selection");

		SOutliner->SetSelection([&RootItems, SOutliner, SelectionID](ISceneOutlinerTreeItem& Item)
			{
				FSceneOutlinerTreeItemID ItemID = Item.GetID();

				//LOG(" - `%s`", *Item.GetDisplayString());
				if (ISceneOutlinerTreeItem* Parent = Item.GetParent().Get())
				{
					//LOG("    Parent: `%s`", *Parent->GetDisplayString());
				}
				else
				{
					//LOG_H("  Root Item: `%s`", *Item.GetDisplayString());
					auto ItemPtr = SOutliner->GetTreeItem(ItemID);
					if (ItemPtr)
					{
						RootItems.Add(ItemPtr); // if the item does not have a parent, it is a root
					}
					else
					{
						ERROR_H("     => Impossible to find its reference");
					}
				}

				return SelectionID.Contains(ItemID);
			});

		//SOutliner->SetItemSelection(Selection, true, ESelectInfo::Direct);
	}
	else
	{
		ERROR_H("`SSceneOutliner` given is null");
	}
	return RootItems;
}

void OutlinerShortcutsCommands::RegisterCommands()
{
	UI_COMMAND(SceneOutlinerCollapseAll, "Collapse All", "Collapse all Actors and Folders in the Scene Outliner", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SceneOutlinerCollapseToRoot, "Collapse to Root", "Collapse all Actors and Folders in the Scene Outliner but keep the root element expanded", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SceneOutlinerExpandAll, "Expand All", "Expand all Actors and Folders in the Scene Outliner", EUserInterfaceActionType::Button, FInputGesture());

	LOG("Commands Registered");
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOutlinerShortcutsEditorModule, OutlinerShortcuts)