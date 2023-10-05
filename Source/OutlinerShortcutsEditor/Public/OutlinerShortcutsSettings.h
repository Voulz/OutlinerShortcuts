// Copyright Voulz 2021-2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "UObject/ReflectedTypeAccessors.h"

#include "OutlinerShortcutsSettings.generated.h"

UENUM()
enum class EOutlinerExpansionBehaviorOnMapOpen : uint8
{
	// Default Editor Behavior. Expands all the actors in the Scene Outliner.
	ExpandAll,
	// Collapses all the actors in the Scene Outliner.
	CollapseAll,
	// Collapses all the actors in the Scene Outliner but keep the root item expanded.
	CollapseToRoot,
};

/**
 *
 */
UCLASS(config = EditorUserSettings, defaultconfig)
class OUTLINERSHORTCUTSEDITOR_API UOutlinerShortcutsEditorSettings : public UObject
{
	GENERATED_BODY()
public:

	/*
	* Sets the default Outliner Behavior when a map opens.
	*/
	UPROPERTY(EditAnywhere, config, Category = "Outliner Shortcuts")
		EOutlinerExpansionBehaviorOnMapOpen DefaultOutlinerExpansionBehaviorOnMapOpen = EOutlinerExpansionBehaviorOnMapOpen::CollapseToRoot;

	FORCEINLINE static const UOutlinerShortcutsEditorSettings* GetDefaultInstance()
	{
		return GetDefault<UOutlinerShortcutsEditorSettings>();
	}
};
