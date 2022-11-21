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

#include "Framework/Docking/TabManager.h"

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

#if UE5_1
	CommandList->MapAction(Commands.SceneOutlinerCollapseAllOutliners, FExecuteAction::CreateStatic(
		[]() { 
			FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutliners();
		}));
	CommandList->MapAction(Commands.SceneOutlinerCollapseAllOutlinersToRoot, FExecuteAction::CreateLambda(
		[]() {
			FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutlinersToRoot();
		}));
	CommandList->MapAction(Commands.SceneOutlinerExpandAllOutliners, FExecuteAction::CreateLambda(
		[]() {
			FOutlinerShortcutsEditorModule::SceneOutlinerExpandAllOutliners();
		}));
#endif

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
#if PRE_UE5_1
		WARN_H("SceneOutliner is not ready");
#else
		WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
#endif
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
#if PRE_UE5_1
		WARN_H("SceneOutliner is not ready");
#else
		WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
#endif
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
#if PRE_UE5_1
		WARN_H("SceneOutliner is not ready");
#else
		WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
#endif
	}
	else
	{
		ERROR_H("Not able to get `SSceneOutliner`");
	}

	return false;
}

#if UE5_1
bool FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutliners()
{
	HERE_D;

	bool bAllCollapsed = true;

	TArray<SSceneOutliner*> SOutliners = GetAllSSceneOutliners();
	for (SSceneOutliner* SOutliner : SOutliners)
	{
		if (IsSceneOutlinerReady(SOutliner))
		{
			SOutliner->CollapseAll();
			bAllCollapsed &= true;
		}
		else
		{
			bAllCollapsed = false;
			WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
		}
	}

	if (SOutliners.Num() == 0)
	{
		ERROR_H("Not able to get any `SSceneOutliner`");
	}

	return bAllCollapsed;
}

bool FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutlinersToRoot()
{
	HERE_D;

	bool bAllCollapsed = true;

	TArray<SSceneOutliner*> SOutliners = GetAllSSceneOutliners();
	for (SSceneOutliner* SOutliner : SOutliners)
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
			bAllCollapsed &= true;
		}
		else
		{
			bAllCollapsed = false;
			WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
		}
	}

	if (SOutliners.Num() == 0)
	{
		ERROR_H("Not able to get any `SSceneOutliner`");
	}

	return bAllCollapsed;
}

bool FOutlinerShortcutsEditorModule::SceneOutlinerExpandAllOutliners()
{
	HERE_D;

	bool bAllExpanded = true;

	TArray<SSceneOutliner*> SOutliners = GetAllSSceneOutliners();
	for (SSceneOutliner* SOutliner : SOutliners)
	{
		if (IsSceneOutlinerReady(SOutliner))
		{
			SOutliner->ExpandAll();
			bAllExpanded &= true;
		}
		else
		{
			bAllExpanded = false;
			WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
		}
	}

	if (SOutliners.Num() == 0)
	{
		ERROR_H("Not able to get any `SSceneOutliner`");
	}

	return bAllExpanded;
}
#endif

ISceneOutliner* FOutlinerShortcutsEditorModule::GetISceneOutliner()
{
#if PRE_UE5_1
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
				ERROR_H("Not able to get an `ISceneOutliner` through `LevelEditor->GetSceneOutliner()");
			}
		}
		else
		{
			ERROR_H("Not able to get an `ILevelEditor` through `LevelEditorModule->GetFirstLevelEditor()`");
		}
	}
	else
	{
		ERROR_H("Not able to load `LevelEditor` module");
	}

	return nullptr;
#else
	return GetSSceneOutliner();
#endif
}

SSceneOutliner* FOutlinerShortcutsEditorModule::GetSSceneOutliner()
{
#if PRE_UE5_1
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
#else
	FLevelEditorModule* LevelEditorModule = FModuleManager::LoadModulePtr<FLevelEditorModule>("LevelEditor");
	if (LevelEditorModule)
	{
		if (ILevelEditor* LevelEditor = LevelEditorModule->GetFirstLevelEditor().Get())
		{
			// The function `LevelEditor->GetMostRecentlyUsedSceneOutliner()` doesn't actually return the most recently used Outliner,
			// but the most recently opened one! Which can be confusing while using this plugin.
			// To find the most recently used one, we loop through each of the Tab containing an outliner, and we check the last time it was activated.

			if (auto Manager = LevelEditor->GetTabManager().Get())
			{
				WARN_D(" --- Checking all scene Outliners --- ");
				double LastActivated = 0.;
				SSceneOutliner* MostRecentOutliner = nullptr;
				for (auto IOutlinerPtr : LevelEditor->GetAllSceneOutliners())
				{
					if (TSharedPtr<ISceneOutliner> IOutlinerPin = IOutlinerPtr.Pin())
					{
						if (ISceneOutliner* IOutliner = IOutlinerPin.Get())
						{
							if (SSceneOutliner* SOutliner = (SSceneOutliner*)IOutliner)
							{
								FName ID = SOutliner->GetOutlinerIdentifier();
								WARN_D("Outliner `%s`:", *ID.ToString());

								if (auto OutlinerTab = Manager->FindExistingLiveTab(ID).Get())
								{
									LOG_D("  Outliner Tab: `%s` [ID: `%s`] [last activated: %f]", *OutlinerTab->GetTabLabel().ToString(), *OutlinerTab->GetLayoutIdentifier().ToString(), OutlinerTab->GetLastActivationTime());
									if (OutlinerTab->GetLastActivationTime() > LastActivated)
									{
										LastActivated = OutlinerTab->GetLastActivationTime();
										MostRecentOutliner = SOutliner;
									}
								}
								else
								{
									WARN_D("  !! Did NOT find the tab !!");
								}
							}
						}

					}
				}

				if (MostRecentOutliner)
				{
					WARN_D(" --- Most Recent Outliner is: `%s`", *MostRecentOutliner->GetOutlinerIdentifier().ToString());
					return MostRecentOutliner;
				}
			}
			else
			{
				ERROR_H("Not able to get the TabManager throguh `LevelEditor->GetTabManager()`");
			}

			// if all the above fails, we fallback on the default one
			LOG_D("Not able to find the most recently used Outliner");
			return (SSceneOutliner*)LevelEditor->GetMostRecentlyUsedSceneOutliner().Get();
		}
		else
		{
			ERROR_H("Not able to get an `ILevelEditor` through `LevelEditorModule->GetFirstLevelEditor()`");
		}
	}
	else
	{
		ERROR_H("Not able to load `LevelEditor` module");
	}
#endif

	return nullptr;
}

#if UE5_1
TArray<ISceneOutliner*> FOutlinerShortcutsEditorModule::GetAllISceneOutliners()
{
	TArray<ISceneOutliner*> Outliners;
#if PRE_UE5_1
	if (ISceneOutliner* SceneOutliner = GetISceneOutliner())
	{
		Outliners.Add(SceneOutliner);
	}
#else
	FLevelEditorModule* LevelEditorModule = FModuleManager::LoadModulePtr<FLevelEditorModule>("LevelEditor");
	if (LevelEditorModule)
	{
		if (ILevelEditor* LevelEditor = LevelEditorModule->GetFirstLevelEditor().Get())
		{
			for (auto SceneOutliner : LevelEditor->GetAllSceneOutliners())
			{
				if (SceneOutliner.IsValid() && SceneOutliner.Pin().Get())
				{
					Outliners.Add(SceneOutliner.Pin().Get());
				}
				else
				{
					ERROR_H("Not able to get `SceneOutliner`");
				}
			}
		}
		else
		{
			ERROR_H("Not able to get `LevelEditor`");
		}
	}
	else
	{
		ERROR_H("Not able to load `LevelEditor` module");
	}
#endif



	return Outliners;
}

TArray<SSceneOutliner*> FOutlinerShortcutsEditorModule::GetAllSSceneOutliners()
{
	TArray<SSceneOutliner*> SOutliners;

	TArray<ISceneOutliner*> SceneOutliners = GetAllISceneOutliners();
	for (ISceneOutliner* SceneOutliner : SceneOutliners)
	{
		if (SSceneOutliner* SOutliner = (SSceneOutliner*)SceneOutliner)
		{
			SOutliners.Add(SOutliner);
		}
		else
		{
			ERROR_H("Not able to convert `ISceneOutliner` to `SSceneOutliner`");
		}
	}

	if (SOutliners.Num() == 0)
	{
		ERROR_H("Not able to get any `SceneOutliner`");
	}

	return SOutliners;
}
#endif

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
	UI_COMMAND(SceneOutlinerCollapseAll, "Collapse All", "Collapse all Actors and Folders in the current Scene Outliner", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SceneOutlinerCollapseToRoot, "Collapse to Root", "Collapse all Actors and Folders in the current Scene Outliner but keep the root element expanded", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SceneOutlinerExpandAll, "Expand All", "Expand all Actors and Folders in the current Scene Outliner", EUserInterfaceActionType::Button, FInputGesture());
	
#if UE5_1
	UI_COMMAND(SceneOutlinerCollapseAllOutliners, "Collapse All Outliners", "[UE 5.1 Onwards] Collapse all Actors and Folders in ALL Scene Outliners", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SceneOutlinerCollapseAllOutlinersToRoot, "Collapse All Outliners to Root", "[UE 5.1 Onwards] Collapse all Actors and Folders in ALL Scene Outliners but keep their root element expanded", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SceneOutlinerExpandAllOutliners, "Expand All Outliners", "[UE 5.1 Onwards] Expand all Actors and Folders in ALL Scene Outliners", EUserInterfaceActionType::Button, FInputGesture());
#endif

	LOG("Commands Registered");
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOutlinerShortcutsEditorModule, OutlinerShortcuts)