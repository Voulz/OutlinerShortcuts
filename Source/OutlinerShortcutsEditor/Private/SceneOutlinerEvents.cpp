// Copyright Voulz 2021-2022. All Rights Reserved.


#include "SceneOutlinerEvents.h"

#include "SSceneOutliner.h"

#include "Containers/Ticker.h"
#include "GameFramework/WorldSettings.h"
#include "OutlinerShortcutsSettings.h"


void FSceneOutlinerEvents::OnStartupModule()
{
	// --- FEditorDelegates
	//{
		OnMapOpenedHandle = FEditorDelegates::OnMapOpened.AddRaw(this, &FSceneOutlinerEvents::OnMapOpened);
		//FDelegateHandle OnMapOpenedHandle = FEditorDelegates::OnMapOpened.AddRaw(this, &FSceneOutlinerEvents::OnMapOpened);
		//EventReleasers.Add(FReleaser{ [=] { FEditorDelegates::OnMapOpened.Remove(Handle); } });
	//}

}

void FSceneOutlinerEvents::OnShutdownModule()
{
	FEditorDelegates::OnMapOpened.Remove(OnMapOpenedHandle);
	//for (int32 Index = EventReleasers.Num() - 1; Index >= 0; --Index)
	//{
	//	EventReleasers[Index].Callback();
	//}
	//EventReleasers.Empty();

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
		// OUTLINER_SHORTCUTS_WARN_H("`Scene Outliner` returned null. Retrying next tick...");
		return false;
	}

	if (!FOutlinerShortcutsEditorModule::IsSceneOutlinerReady(SceneOutliner))
	{
		// OUTLINER_SHORTCUTS_WARN_H("`Scene Outliner` is not ready. Retrying next tick...");
		return false;
	}
	OUTLINER_SHORTCUTS_HERE_D;
	OUTLINER_SHORTCUTS_LOG_D("`Scene Outliner` is ready.");

	return ApplyOutlinerExpansionFromOnMapOpenedSettings(SceneOutliner);
}

bool FSceneOutlinerEvents::ApplyOutlinerExpansionFromOnMapOpenedSettings(const SSceneOutliner* SceneOutliner)
{
	if (!SceneOutliner)
	{
		return false;
	}
	OUTLINER_SHORTCUTS_HERE_D;

	if (const UOutlinerShortcutsEditorSettings* EditorSettings = UOutlinerShortcutsEditorSettings::GetDefaultInstance())
	{
		bool bSuccess = false;
		switch (EditorSettings->DefaultOutlinerExpansionBehaviorOnMapOpen)
		{
		case EOutlinerExpansionBehaviorOnMapOpen::ExpandAll:
#if PRE_UE5_1
			OUTLINER_SHORTCUTS_LOG_H(" Editor Settings: Expand All...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerExpandAll();
#else
			OUTLINER_SHORTCUTS_LOG_H(" Editor Settings: Expand All Outliners...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerExpandAllOutliners();
#endif
			break;
		case EOutlinerExpansionBehaviorOnMapOpen::CollapseAll:
#if PRE_UE5_1
			OUTLINER_SHORTCUTS_LOG_H(" Editor Settings: Collapse All...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAll();
#else
			OUTLINER_SHORTCUTS_LOG_H(" Editor Settings: Collapse All Outliners...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutliners();
#endif
			break;
		case EOutlinerExpansionBehaviorOnMapOpen::CollapseToRoot:
#if PRE_UE5_1
			OUTLINER_SHORTCUTS_LOG_H(" Editor Settings: Collapse To Root...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerCollapseToRoot();
#else
			OUTLINER_SHORTCUTS_LOG_H(" Editor Settings: Collapse All Outliners To Root...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutlinersToRoot();
#endif
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