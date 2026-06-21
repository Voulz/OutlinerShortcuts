// Copyright Voulz. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#include "SceneOutlinerFwd.h"
#include "OutlinerShortcutsEditorDefinitions.h"
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"

class IModuleListenerInterface
{
public:
	/**
	 * Note: Even though this is an interface class we need a virtual destructor here because modules are deleted via a pointer to this interface
	 */
	virtual ~IModuleListenerInterface() {};

	virtual void OnStartupModule() {};
	virtual void OnShutdownModule() {};
};

class FOutlinerShortcutsEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	static void RegisterSettings();
	static void UnregisterSettings();

	TArray<TSharedRef<IModuleListenerInterface>> ModuleListeners;
	void AddModuleListeners();

	TSharedPtr<FUICommandList> CommandList;

	void MapCommands();

public:
	// Collapse the most recently used Scene Outliner
	static bool SceneOutlinerCollapseAll();
	// Collapse to root the most recently used Scene Outliner
	static bool SceneOutlinerCollapseToRoot();
	// Expand the most recently used Scene Outliner
	static bool SceneOutlinerExpandAll();

	// [UE 5.1 Onwards] Collapse all the opened Scene Outliners
	static bool SceneOutlinerCollapseAllOutliners();
	// [UE 5.1 Onwards] Collapse to root all the opened Scene Outliners
	static bool SceneOutlinerCollapseAllOutlinersToRoot();
	// [UE 5.1 Onwards] Expand all the opened Scene Outliners
	static bool SceneOutlinerExpandAllOutliners();

	static ISceneOutliner* GetISceneOutliner();
	static SSceneOutliner* GetSSceneOutliner();
	static TArray<ISceneOutliner*> GetAllISceneOutliners();
	static TArray<SSceneOutliner*> GetAllSSceneOutliners();

	FORCEINLINE static UWorld* GetCurrentEditorWorld();
	static FSceneOutlinerTreeItemPtr GetWorldTreeItemPtr(SSceneOutliner* SceneOutliner);
	static bool IsSceneOutlinerReady(SSceneOutliner* SceneOutliner);
	static TArray<FSceneOutlinerTreeItemPtr> GetSceneOutlinerRootItems(SSceneOutliner* SOutliner);
};

class OUTLINERSHORTCUTSEDITOR_API OutlinerShortcutsCommands : public TCommands<OutlinerShortcutsCommands>
{
public:
	// look at FLevelEditorCommands for inspiration
	OutlinerShortcutsCommands() : TCommands<OutlinerShortcutsCommands>(
		TEXT("OutlinerShortcutsCommands"), // Context name for fast lookup
		FText::FromString("Outliner Shortcuts"), // Context name for displaying
		NAME_None,   // No parent context
		FAppStyle::GetAppStyleSetName()
		)
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> SceneOutlinerCollapseAll;
	TSharedPtr<FUICommandInfo> SceneOutlinerCollapseToRoot;
	TSharedPtr<FUICommandInfo> SceneOutlinerExpandAll;
	TSharedPtr<FUICommandInfo> SceneOutlinerCollapseAllOutliners;
	TSharedPtr<FUICommandInfo> SceneOutlinerCollapseAllOutlinersToRoot;
	TSharedPtr<FUICommandInfo> SceneOutlinerExpandAllOutliners;
};