// Copyright Voulz 2021-2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OutlinerShortcutsEditor.h"


/**
 *
 */
class OUTLINERSHORTCUTSEDITOR_API FSceneOutlinerEvents : public IModuleListenerInterface
{
public:
	virtual void OnStartupModule() override;
	virtual void OnShutdownModule() override;

public:
	void OnMapOpened(const FString& Filename, bool bAsTemplate);

	//class UMapOutlinerData* LatestLoadedData;
	FTSTicker::FDelegateHandle TickUntilActorsLoadedHandle;
	static bool TickUntilActorsLoaded(float DeltaTime);
	/*
	 *
	 */
	static bool ApplyOutlinerActionsFromOnMapOpenedSettings();
	static bool ApplyOutlinerExpansionFromOnMapOpenedSettings(const SSceneOutliner* SceneOutliner);
private:

	/** Private array of event callback releaser data, inspired from FAcquiredResources */
	//struct FReleaser
	//{
	//	TFunction<void()> Callback;
	//};
	//TArray<FReleaser> EventReleasers;
	FDelegateHandle OnMapOpenedHandle;
};
