// Copyright Voulz 2021-2022. All Rights Reserved.

#include "OutlinerShortcutsEditor.h"

#include "OutlinerShortcutsEditorDefinitions.h"
#include "LevelEditor.h"
#include "ISceneOutliner.h"
#include "Widgets/Views/STreeView.h"
#include "SceneOutlinerFwd.h"
#include "SSceneOutliner.h"

#include "Editor.h"
#include "SceneOutlinerEvents.h"
#include "ISettingsModule.h"
#include "OutlinerShortcutsSettings.h"

#include "Framework/Docking/TabManager.h"

#define LOCTEXT_NAMESPACE "FOutlinerShortcutsEditorModule"

void FOutlinerShortcutsEditorModule::StartupModule()
{
	OUTLINER_SHORTCUTS_HERE;

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

	OUTLINER_SHORTCUTS_LOG("`FOutlinerShortcutsEditorModule` Loaded");

	// Test of Logs:
	// OUTLINER_SHORTCUTS_WARN(" ==== [LogOutlinerShortcutsEditor Macro test] ==== ")
	// OUTLINER_SHORTCUTS_LOG("OUTLINER_SHORTCUTS_LOG")
	// OUTLINER_SHORTCUTS_LOG_H("OUTLINER_SHORTCUTS_LOG_H")
	// OUTLINER_SHORTCUTS_LOG_D("OUTLINER_SHORTCUTS_LOG_D")
	// OUTLINER_SHORTCUTS_LOGV("OUTLINER_SHORTCUTS_LOGV")
	// OUTLINER_SHORTCUTS_LOGV_H("OUTLINER_SHORTCUTS_LOGV_H")
	// OUTLINER_SHORTCUTS_WARN("OUTLINER_SHORTCUTS_WARN")
	// OUTLINER_SHORTCUTS_WARN_H("OUTLINER_SHORTCUTS_WARN_H")
	// OUTLINER_SHORTCUTS_WARN_D("OUTLINER_SHORTCUTS_WARN_D")
	// OUTLINER_SHORTCUTS_ERROR("OUTLINER_SHORTCUTS_ERROR")
	// OUTLINER_SHORTCUTS_ERROR_H("OUTLINER_SHORTCUTS_ERROR_H")
	// OUTLINER_SHORTCUTS_WHERE
	// OUTLINER_SHORTCUTS_HERE
	// OUTLINER_SHORTCUTS_HERE_D
	// OUTLINER_SHORTCUTS_LOG("OUTLINER_SHORTCUTS_BtoS: TRUE: %s  FALSE: %s", OUTLINER_SHORTCUTS_BtoS(true), OUTLINER_SHORTCUTS_BtoS(false))
	// OUTLINER_SHORTCUTS_WARN(" ====  ==== ")
}

void FOutlinerShortcutsEditorModule::ShutdownModule()
{
	OUTLINER_SHORTCUTS_HERE;

	OutlinerShortcutsCommands::Unregister();

	for (int32 i = 0; i < ModuleListeners.Num(); ++i)
	{
		ModuleListeners[i]->OnShutdownModule();
	}

	OUTLINER_SHORTCUTS_LOG("`FOutlinerShortcutsEditorModule` Unloaded");
}


void FOutlinerShortcutsEditorModule::RegisterSettings()
{
	OUTLINER_SHORTCUTS_HERE;
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");

	SettingsModule.RegisterSettings("Editor", "Plugins", "Outliner Shortcuts",
		LOCTEXT("EditorSettings_Label", "Outliner Shortcuts"),
		LOCTEXT("EditorSettings_Description", "Configure the default settings for Outliner Shortcuts."),
		GetMutableDefault<UOutlinerShortcutsEditorSettings>()
	);

	OUTLINER_SHORTCUTS_LOG_H("Settings Registered");
}

void FOutlinerShortcutsEditorModule::UnregisterSettings()
{
	OUTLINER_SHORTCUTS_HERE;
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "Outliner Shortcuts");
	}
	else
	{
		OUTLINER_SHORTCUTS_WARN_H("`Settings Module` returned null");
	}

	OUTLINER_SHORTCUTS_LOG_H("Settings Unregistered");
}

void FOutlinerShortcutsEditorModule::AddModuleListeners()
{
	ModuleListeners.Add(MakeShareable(new FSceneOutlinerEvents));
}

void FOutlinerShortcutsEditorModule::MapCommands()
{
	OUTLINER_SHORTCUTS_HERE;
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
	const FLevelEditorModule* LevelEditorModule = FModuleManager::LoadModulePtr<FLevelEditorModule>("LevelEditor");
	if (LevelEditorModule)
	{
		LevelEditorModule->GetGlobalLevelEditorActions()->Append(CommandList.ToSharedRef());
	}
	else
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to load `LevelEditor` module");
	}

}

bool FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAll()
{
	OUTLINER_SHORTCUTS_HERE_D;

	if (SSceneOutliner* SOutliner = GetSSceneOutliner())
	{
		if (IsSceneOutlinerReady(SOutliner))
		{
			SOutliner->CollapseAll();
			return true;
		}
#if PRE_UE5_1
		OUTLINER_SHORTCUTS_WARN_H("SceneOutliner is not ready");
#else
		OUTLINER_SHORTCUTS_WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
#endif
	}
	else
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to get `SSceneOutliner`");
	}

	return false;
}

bool FOutlinerShortcutsEditorModule::SceneOutlinerCollapseToRoot()
{
	OUTLINER_SHORTCUTS_HERE_D;

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
		OUTLINER_SHORTCUTS_WARN_H("SceneOutliner is not ready");
#else
		OUTLINER_SHORTCUTS_WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
#endif
	}
	else
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to get `SSceneOutliner`");
	}

	return false;
}

bool FOutlinerShortcutsEditorModule::SceneOutlinerExpandAll()
{
	OUTLINER_SHORTCUTS_HERE_D;

	if (SSceneOutliner* SOutliner = GetSSceneOutliner())
	{
		if (IsSceneOutlinerReady(SOutliner))
		{
			SOutliner->ExpandAll();
			return true;
		}
#if PRE_UE5_1
		OUTLINER_SHORTCUTS_WARN_H("SceneOutliner is not ready");
#else
		OUTLINER_SHORTCUTS_WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
#endif
	}
	else
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to get `SSceneOutliner`");
	}

	return false;
}

#if UE5_1
bool FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutliners()
{
	OUTLINER_SHORTCUTS_HERE_D;

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
			OUTLINER_SHORTCUTS_WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
		}
	}

	if (SOutliners.Num() == 0)
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to get any `SSceneOutliner`");
	}

	return bAllCollapsed;
}

bool FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutlinersToRoot()
{
	OUTLINER_SHORTCUTS_HERE_D;

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
			OUTLINER_SHORTCUTS_WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
		}
	}

	if (SOutliners.Num() == 0)
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to get any `SSceneOutliner`");
	}

	return bAllCollapsed;
}

bool FOutlinerShortcutsEditorModule::SceneOutlinerExpandAllOutliners()
{
	OUTLINER_SHORTCUTS_HERE_D;

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
			OUTLINER_SHORTCUTS_WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
		}
	}

	if (SOutliners.Num() == 0)
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to get any `SSceneOutliner`");
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
				OUTLINER_SHORTCUTS_ERROR_H("Not able to get an `ISceneOutliner` through `LevelEditor->GetSceneOutliner()");
			}
		}
		else
		{
			OUTLINER_SHORTCUTS_ERROR_H("Not able to get an `ILevelEditor` through `LevelEditorModule->GetFirstLevelEditor()`");
		}
	}
	else
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to load `LevelEditor` module");
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
			OUTLINER_SHORTCUTS_ERROR_H("Not able to convert `ISceneOutliner` to `SSceneOutliner`");
		}
	}
	else
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to get `SceneOutliner`");
	}
#else
	const FLevelEditorModule* LevelEditorModule = FModuleManager::LoadModulePtr<FLevelEditorModule>("LevelEditor");
	if (LevelEditorModule)
	{
		if (ILevelEditor* LevelEditor = LevelEditorModule->GetFirstLevelEditor().Get())
		{
			// The function `LevelEditor->GetMostRecentlyUsedSceneOutliner()` doesn't actually return the most recently used Outliner,
			// but the most recently opened one! Which can be confusing while using this plugin.
			// To find the most recently used one, we loop through each of the Tab containing an outliner, and we check the last time it was activated.

			if (const FTabManager* Manager = LevelEditor->GetTabManager().Get())
			{
				OUTLINER_SHORTCUTS_WARN_D(" --- Checking all scene Outliners --- ");
				double LastActivated = 0.;
				SSceneOutliner* MostRecentOutliner = nullptr;
				for (TWeakPtr<ISceneOutliner> IOutlinerPtr : LevelEditor->GetAllSceneOutliners())
				{
					if (TSharedPtr<ISceneOutliner> IOutlinerPin = IOutlinerPtr.Pin())
					{
						if (ISceneOutliner* IOutliner = IOutlinerPin.Get())
						{
							if (SSceneOutliner* SOutliner = static_cast<SSceneOutliner*>(IOutliner))
							{
								const FName ID = SOutliner->GetOutlinerIdentifier();
								OUTLINER_SHORTCUTS_WARN_D("Outliner `%s`:", *ID.ToString());

								if (SDockTab* OutlinerTab = Manager->FindExistingLiveTab(ID).Get())
								{
									OUTLINER_SHORTCUTS_LOG_D("  Outliner Tab: `%s` [ID: `%s`] [last activated: %f]", *OutlinerTab->GetTabLabel().ToString(), *OutlinerTab->GetLayoutIdentifier().ToString(), OutlinerTab->GetLastActivationTime());
									if (OutlinerTab->GetLastActivationTime() > LastActivated)
									{
										LastActivated = OutlinerTab->GetLastActivationTime();
										MostRecentOutliner = SOutliner;
									}
								}
								else
								{
									OUTLINER_SHORTCUTS_WARN_D("  !! Did NOT find the tab !!");
								}
							}
						}

					}
				}

				if (MostRecentOutliner)
				{
					OUTLINER_SHORTCUTS_WARN_D(" --- Most Recent Outliner is: `%s`", *MostRecentOutliner->GetOutlinerIdentifier().ToString());
					return MostRecentOutliner;
				}
			}
			else
			{
				OUTLINER_SHORTCUTS_ERROR_H("Not able to get the TabManager throguh `LevelEditor->GetTabManager()`");
			}

			// if all the above fails, we fallback on the default one
			OUTLINER_SHORTCUTS_LOG_D("Not able to find the most recently used Outliner");
			return static_cast<SSceneOutliner*>(LevelEditor->GetMostRecentlyUsedSceneOutliner().Get());
		}
		else
		{
			OUTLINER_SHORTCUTS_ERROR_H("Not able to get an `ILevelEditor` through `LevelEditorModule->GetFirstLevelEditor()`");
		}
	}
	else
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to load `LevelEditor` module");
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
	const FLevelEditorModule* LevelEditorModule = FModuleManager::LoadModulePtr<FLevelEditorModule>("LevelEditor");
	if (LevelEditorModule)
	{
		if (const ILevelEditor* LevelEditor = LevelEditorModule->GetFirstLevelEditor().Get())
		{
			for (TWeakPtr<ISceneOutliner> SceneOutliner : LevelEditor->GetAllSceneOutliners())
			{
				if (SceneOutliner.IsValid() && SceneOutliner.Pin().Get())
				{
					Outliners.Add(SceneOutliner.Pin().Get());
				}
				else
				{
					OUTLINER_SHORTCUTS_ERROR_H("Not able to get `SceneOutliner`");
				}
			}
		}
		else
		{
			OUTLINER_SHORTCUTS_ERROR_H("Not able to get `LevelEditor`");
		}
	}
	else
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to load `LevelEditor` module");
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
		if (SSceneOutliner* SOutliner = static_cast<SSceneOutliner*>(SceneOutliner))
		{
			SOutliners.Add(SOutliner);
		}
		else
		{
			OUTLINER_SHORTCUTS_ERROR_H("Not able to convert `ISceneOutliner` to `SSceneOutliner`");
		}
	}

	if (SOutliners.Num() == 0)
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to get any `SceneOutliner`");
	}

	return SOutliners;
}
#endif

UWorld* FOutlinerShortcutsEditorModule::GetCurrentEditorWorld()
{
	return GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
}

FSceneOutlinerTreeItemPtr FOutlinerShortcutsEditorModule::GetWorldTreeItemPtr(SSceneOutliner* SceneOutliner)
{
	//OUTLINER_SHORTCUTS_HERE;
	if (SceneOutliner)
	{
		if (UWorld* World = GetCurrentEditorWorld())
		{
			const FObjectKey ID(World);
			FSceneOutlinerTreeItemPtr TreeItemPtr = SceneOutliner->GetTreeItem(ID);
			return TreeItemPtr;
		}
		else
		{
			OUTLINER_SHORTCUTS_WARN_H("Editor World is null");
		}
	}
	else
	{
		OUTLINER_SHORTCUTS_WARN_H("`SceneOutliner` is null");
	}
	return FSceneOutlinerTreeItemPtr(nullptr);
}

bool FOutlinerShortcutsEditorModule::IsSceneOutlinerReady(SSceneOutliner* SceneOutliner)
{
	//OUTLINER_SHORTCUTS_HERE;
	return GetWorldTreeItemPtr(SceneOutliner).IsValid();
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
			if (const ISceneOutlinerTreeItem* SelectedItem = SelectedItemPtr.Get())
			{
				SelectionID.Add(SelectedItem->GetID());
			}
		}

		//OUTLINER_SHORTCUTS_WARN("Setting Selection");

		SOutliner->SetSelection([&RootItems, SOutliner, SelectionID](const ISceneOutlinerTreeItem& Item)
			{
				const FSceneOutlinerTreeItemID ItemID = Item.GetID();

				//OUTLINER_SHORTCUTS_LOG(" - `%s`", *Item.GetDisplayString());
				if (ISceneOutlinerTreeItem* Parent = Item.GetParent().Get())
				{
					//OUTLINER_SHORTCUTS_LOG("    Parent: `%s`", *Parent->GetDisplayString());
				}
				else
				{
					//OUTLINER_SHORTCUTS_LOG_H("  Root Item: `%s`", *Item.GetDisplayString());
					const FSceneOutlinerTreeItemPtr ItemPtr = SOutliner->GetTreeItem(ItemID);
					if (ItemPtr)
					{
						RootItems.Add(ItemPtr); // if the item does not have a parent, it is a root
					}
					else
					{
						OUTLINER_SHORTCUTS_ERROR_H("     => Impossible to find its reference");
					}
				}

				return SelectionID.Contains(ItemID);
			});

		//SOutliner->SetItemSelection(Selection, true, ESelectInfo::Direct);
	}
	else
	{
		OUTLINER_SHORTCUTS_ERROR_H("`SSceneOutliner` given is null");
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

	OUTLINER_SHORTCUTS_LOG("Commands Registered");
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOutlinerShortcutsEditorModule, OutlinerShortcuts)