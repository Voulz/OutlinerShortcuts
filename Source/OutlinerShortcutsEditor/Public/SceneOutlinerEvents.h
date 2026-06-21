// Copyright Voulz. All Rights Reserved.

#pragma once

#include "OutlinerShortcutsEditor.h"
#include "Containers/Ticker.h"


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

	FTSTicker::FDelegateHandle TickUntilActorsLoadedHandle;
	static bool TickUntilActorsLoaded(float DeltaTime);

	static bool ApplyOutlinerActionsFromOnMapOpenedSettings();
	static bool ApplyOutlinerExpansionFromOnMapOpenedSettings(const SSceneOutliner* SceneOutliner);
private:

	FDelegateHandle OnMapOpenedHandle;
};
