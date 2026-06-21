// Copyright Voulz. All Rights Reserved.

#include "OutlinerShortcutsEditor.h"

#include "OutlinerShortcutsEditorDefinitions.h"
#include "LevelEditor.h"
#include "ISceneOutliner.h"
#include "SceneOutlinerFwd.h"
#include "SSceneOutliner.h"

#include "Editor.h"
#include "SceneOutlinerEvents.h"
#include "ISettingsModule.h"
#include "OutlinerShortcutsSettings.h"

#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FOutlinerShortcutsEditorModule"

void FOutlinerShortcutsEditorModule::StartupModule()
{
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
}

void FOutlinerShortcutsEditorModule::ShutdownModule()
{
	OutlinerShortcutsCommands::Unregister();

	for (int32 i = 0; i < ModuleListeners.Num(); ++i)
	{
		ModuleListeners[i]->OnShutdownModule();
	}

	OUTLINER_SHORTCUTS_LOG("`FOutlinerShortcutsEditorModule` Unloaded");
}


void FOutlinerShortcutsEditorModule::RegisterSettings()
{
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
	if (SSceneOutliner* SOutliner = GetSSceneOutliner())
	{
		if (IsSceneOutlinerReady(SOutliner))
		{
			SOutliner->CollapseAll();
			return true;
		}
		OUTLINER_SHORTCUTS_WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
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
		OUTLINER_SHORTCUTS_WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
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
		OUTLINER_SHORTCUTS_WARN_H("SceneOutliner `%s` is not ready", *SOutliner->GetOutlinerIdentifier().ToString());
	}
	else
	{
		OUTLINER_SHORTCUTS_ERROR_H("Not able to get `SSceneOutliner`");
	}

	return false;
}

bool FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutliners()
{
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

ISceneOutliner* FOutlinerShortcutsEditorModule::GetISceneOutliner()
{
	return GetSSceneOutliner();
}

SSceneOutliner* FOutlinerShortcutsEditorModule::GetSSceneOutliner()
{
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

	return nullptr;
}

TArray<ISceneOutliner*> FOutlinerShortcutsEditorModule::GetAllISceneOutliners()
{
	TArray<ISceneOutliner*> Outliners;
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

UWorld* FOutlinerShortcutsEditorModule::GetCurrentEditorWorld()
{
	return GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
}

FSceneOutlinerTreeItemPtr FOutlinerShortcutsEditorModule::GetWorldTreeItemPtr(SSceneOutliner* SceneOutliner)
{
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
	UI_COMMAND(SceneOutlinerCollapseAll, "Collapse All", "Collapse all Actors and Folders in the current Scene Outliner", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SceneOutlinerCollapseToRoot, "Collapse to Root", "Collapse all Actors and Folders in the current Scene Outliner but keep the root element expanded", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SceneOutlinerExpandAll, "Expand All", "Expand all Actors and Folders in the current Scene Outliner", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(SceneOutlinerCollapseAllOutliners, "Collapse All Outliners", "[UE 5.1 Onwards] Collapse all Actors and Folders in ALL Scene Outliners", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SceneOutlinerCollapseAllOutlinersToRoot, "Collapse All Outliners to Root", "[UE 5.1 Onwards] Collapse all Actors and Folders in ALL Scene Outliners but keep their root element expanded", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SceneOutlinerExpandAllOutliners, "Expand All Outliners", "[UE 5.1 Onwards] Expand all Actors and Folders in ALL Scene Outliners", EUserInterfaceActionType::Button, FInputChord());

	OUTLINER_SHORTCUTS_LOG("Commands Registered");
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOutlinerShortcutsEditorModule, OutlinerShortcutsEditor)