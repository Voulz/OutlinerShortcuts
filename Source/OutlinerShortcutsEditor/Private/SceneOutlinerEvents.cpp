// Copyright Voulz 2021-2022. All Rights Reserved.


#include "SceneOutlinerEvents.h"

#include "LevelEditor.h"
//#include "OutlinerShortcutsEditor.h"
#include "UObject/SavePackage.h"
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
	LOG_D(" Map:  `%s`  bAsTemplate: `%s`", *Filename, BtoS(bAsTemplate));

	FTSTicker::GetCoreTicker().RemoveTicker(TickUntilActorsLoadedHandle);
	FTickerDelegate TickDelegate = FTickerDelegate::CreateRaw(this, &FSceneOutlinerEvents::TickUntilActorsLoaded);
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
		// WARN_H("`Scene Outliner` returned null. Retrying next tick...");
		return false;
	}

	if (!FOutlinerShortcutsEditorModule::IsSceneOutlinerReady(SceneOutliner))
	{
		// WARN_H("`Scene Outliner` is not ready. Retrying next tick...");
		return false;
	}
	HERE_D;
	LOG_D("`Scene Outliner` is ready.");

	return ApplyOutlinerExpansionFromOnMapOpenedSettings(SceneOutliner);
}

bool FSceneOutlinerEvents::ApplyOutlinerExpansionFromOnMapOpenedSettings(SSceneOutliner* SceneOutliner)
{
	if (!SceneOutliner)	return false;
	HERE_D;

	if (auto EditorSettings = UOutlinerShortcutsEditorSettings::GetDefaultInstance())
	{
		bool bSuccess = false;
		switch (EditorSettings->DefaultOutlinerExpansionBehaviorOnMapOpen)
		{
		case EOutlinerExpansionBehaviorOnMapOpen::ExpandAll:
#if PRE_UE5_1
			LOG_H(" Editor Settings: Expand All...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerExpandAll();
#else
			LOG_H(" Editor Settings: Expand All Outliners...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerExpandAllOutliners();
#endif
			break;
		case EOutlinerExpansionBehaviorOnMapOpen::CollapseAll:
#if PRE_UE5_1
			LOG_H(" Editor Settings: Collapse All...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAll();
#else
			LOG_H(" Editor Settings: Collapse All Outliners...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerCollapseAllOutliners();
#endif
			break;
		case EOutlinerExpansionBehaviorOnMapOpen::CollapseToRoot:
#if PRE_UE5_1
			LOG_H(" Editor Settings: Collapse To Root...");
			bSuccess = FOutlinerShortcutsEditorModule::SceneOutlinerCollapseToRoot();
#else
			LOG_H(" Editor Settings: Collapse All Outliners To Root...");
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
		WARN_H("Unable to get `UOutlinerShortcutsEditorSettings`...");
	}
	return false;
}