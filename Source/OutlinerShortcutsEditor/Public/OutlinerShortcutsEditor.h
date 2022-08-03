// Copyright Voulz 2021-2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "SceneOutlinerFwd.h"
#include "UObject/ObjectSaveContext.h"
#include "OutlinerShortcutsEditorDefinitions.h"

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
	void RegisterSettings();
	void UnregisterSettings();

	TArray<TSharedRef<IModuleListenerInterface>> ModuleListeners;
	void AddModuleListeners();

	TSharedPtr<FUICommandList> CommandList;

	void MapCommands();

public:
	static bool SceneOutlinerCollapseAll();
	static bool SceneOutlinerCollapseToRoot();
	static bool SceneOutlinerExpandAll();

	static ISceneOutliner* GetISceneOutliner();
	static SSceneOutliner* GetSSceneOutliner();
	FORCEINLINE static UWorld* GetCurrentEditorWorld();
	static FSceneOutlinerTreeItemPtr GetWorldTreetItemPtr(SSceneOutliner* SceneOutliner);
	FORCEINLINE static bool IsSceneOutlinerReady(SSceneOutliner* SceneOutliner);
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
		FEditorStyle::GetStyleSetName() // Icon Style Set
		)
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> SceneOutlinerCollapseAll;
	TSharedPtr<FUICommandInfo> SceneOutlinerCollapseToRoot;
	TSharedPtr<FUICommandInfo> SceneOutlinerExpandAll;
};