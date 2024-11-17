// Copyright Voulz 2021-2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "SceneOutlinerFwd.h"
#include "OutlinerShortcutsEditorDefinitions.h"
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

#if UE5_1
	// [UE 5.1 Onwards] Collapse all the opened Scene Outliners
	static bool SceneOutlinerCollapseAllOutliners();
	// [UE 5.1 Onwards] Collapse to root all the opened Scene Outliners
	static bool SceneOutlinerCollapseAllOutlinersToRoot();
	// [UE 5.1 Onwards] Expand all the opened Scene Outliners
	static bool SceneOutlinerExpandAllOutliners();
#endif

	static ISceneOutliner* GetISceneOutliner();
	static SSceneOutliner* GetSSceneOutliner();
#if UE5_1
	static TArray<ISceneOutliner*> GetAllISceneOutliners();
	static TArray<SSceneOutliner*> GetAllSSceneOutliners();
#endif

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
//#if PRE_UE5_1
//		FEditorStyle::GetStyleSetName() // Icon Style Set
//#else
		FAppStyle::GetAppStyleSetName()
//#endif
		)
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> SceneOutlinerCollapseAll;
	TSharedPtr<FUICommandInfo> SceneOutlinerCollapseToRoot;
	TSharedPtr<FUICommandInfo> SceneOutlinerExpandAll;
#if UE5_1
	TSharedPtr<FUICommandInfo> SceneOutlinerCollapseAllOutliners;
	TSharedPtr<FUICommandInfo> SceneOutlinerCollapseAllOutlinersToRoot;
	TSharedPtr<FUICommandInfo> SceneOutlinerExpandAllOutliners;
#endif
};