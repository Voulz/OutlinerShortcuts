// Copyright Voulz 2021-2022. All Rights Reserved.

#include "SceneOutlinerEvents.h"

#include "SSceneOutliner.h"

#include "Containers/Ticker.h"
#include "GameFramework/WorldSettings.h"
#include "OutlinerShortcutsSettings.h"


void FSceneOutlinerEvents::OnStartupModule()
{
	OnMapOpenedHandle = FEditorDelegates::OnMapOpened.AddRaw(this, &FSceneOutlinerEvents::OnMapOpened);
}

void FSceneOutlinerEvents::OnShutdownModule()
{
	FEditorDelegates::OnMapOpened.Remove(OnMapOpenedHandle);

	FTSTicker::GetCoreTicker().RemoveTicker(TickUntilActorsLoadedHandle);
}

void FSceneOutlinerEvents::OnMapOpened(const FString& Filename, bool bAsTemplate)
{
	OUTLINER_SHORTCUTS_LOG_D(" Map:  `%s`  bAsTemplate: `%s`", *Filename, OUTLINER_SHORTCUTS_BtoS(bAsTemplate));

	FTSTicker::GetCoreTicker().RemoveTicker(TickUntilActorsLoadedHandle);
	const FTickerDelegate TickDelegate = FTickerDelegate::CreateStatic(&FSceneOutlinerEvents::TickUntilActorsLoaded);
	TickUntilActorsLoadedHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);
}

bool FSceneOutlinerEvents::TickUntilActorsLoaded(float DeltaTime)
{
	// need to return true for the function to tick again
	return !ApplyOutlinerActionsFromOnMapOpenedSettings();
}

bool FSceneOutlinerEvents::ApplyOutlinerActionsFromOnMapOpenedSettings()
{

	SSceneOutliner* SceneOutliner = FOutlinerShortcutsEditorModule::GetSSceneOutliner();
	if (!SceneOutliner)
	{
		// Retrying next tick
		return false;
	}

	if (!FOutlinerShortcutsEditorModule::IsSceneOutlinerReady(SceneOutliner))
	{
		// Retrying next tick
		return false;
	}
	OUTLINER_SHORTCUTS_LOG_D("`Scene Outliner` is ready.");

	return ApplyOutlinerExpansionFromOnMapOpenedSettings(SceneOutliner);
}

bool FSceneOutlinerEvents::ApplyOutlinerExpansionFromOnMapOpenedSettings(const SSceneOutliner* SceneOutliner)
{
	if (!SceneOutliner)
	{
		return false;
	}

	if (const UOutlinerShortcutsEditorSettings* EditorSettings = UOutlinerShortcutsEditorSettings::GetDefaultInstance())
	{
		bool bSuccess = false;
		switch (EditorSettings->DefaultOutlinerExpansionBehaviorOnMapOpen)
		{
		case EOutlinerExpansionBehaviorOnMapOpen::ExpandAll:
			OUTLINER_SHORTCUTS_LOG_H(" Editor Settings: Expand All Outliners...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerExpandAllOutliners();
			break;
		case EOutlinerExpansionBehaviorOnMapOpen::CollapseAll:
			OUTLINER_SHORTCUTS_LOG_H(" Editor Settings: Collapse All Outliners...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutliners();
			break;
		case EOutlinerExpansionBehaviorOnMapOpen::CollapseToRoot:
			OUTLINER_SHORTCUTS_LOG_H(" Editor Settings: Collapse All Outliners To Root...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutlinersToRoot();
			break;
		default:
			break;
		}

		return bSuccess;
	}
	else
	{
		OUTLINER_SHORTCUTS_WARN_H("Unable to get `UOutlinerShortcutsEditorSettings`...");
	}
	return false;
}