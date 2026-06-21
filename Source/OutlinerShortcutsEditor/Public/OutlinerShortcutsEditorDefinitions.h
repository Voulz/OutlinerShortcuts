// Copyright Voulz. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOutlinerShortcutsEditor, Log, All);

#undef DEBUG_OUTLINER_SHORTCUTS
#define DEBUG_OUTLINER_SHORTCUTS 0


#ifndef OUTLINER_SHORTCUTS_LOG
/**
* @brief LOG. Calls UE_LOG(LogOutlinerShortcutsEditor, Display, ...).
*/
#define OUTLINER_SHORTCUTS_LOG(Format, ...) __OUTLINER_SHORTCUTS_LOG__(Format, ##__VA_ARGS__);
#define __OUTLINER_SHORTCUTS_LOG__(Format, ...) UE_LOG(LogOutlinerShortcutsEditor, Display, TEXT(Format), ##__VA_ARGS__);
#endif

#ifndef LOG_H
/**
* @brief LOG + HERE. Calls UE_LOG(LogOutlinerShortcutsEditor, Display, ...).
*/
#define OUTLINER_SHORTCUTS_LOG_H(Format, ...) __OUTLINER_SHORTCUTS_LOGH__(Format, ##__VA_ARGS__);
#define __OUTLINER_SHORTCUTS_LOGH__(Format, ...) UE_LOG(LogOutlinerShortcutsEditor, Display, TEXT("[%hs]  " Format), __FUNCTION__, ##__VA_ARGS__);
#endif

#ifndef OUTLINER_SHORTCUTS_LOG_D
#if DEBUG_OUTLINER_SHORTCUTS
/**
* @brief LOG DEBUG. Calls UE_LOG(LogOutlinerShortcutsEditor, Display, ...) if DEBUG_OUTLINER_SHORTCUTS is set to 1. 
*/
#define OUTLINER_SHORTCUTS_LOG_D(Format, ...) OUTLINER_SHORTCUTS_LOG_H(Format, ##__VA_ARGS__);
#else // not DEBUG_OUTLINER_SHORTCUTS
/**
* @brief LOG DEBUG. Calls UE_LOG(LogOutlinerShortcutsEditor, Display, ...) if DEBUG_OUTLINER_SHORTCUTS is set to 1. 
*/
#define OUTLINER_SHORTCUTS_LOG_D(Format, ...) {}
#endif //DEBUG_OUTLINER_SHORTCUTS
#endif


#ifndef OUTLINER_SHORTCUTS_LOGV
/**
* @brief LOG VERBOSE. Calls UE_LOG(LogOutlinerShortcutsEditor, Log, ...). 
*/
#define OUTLINER_SHORTCUTS_LOGV(Format, ...) __OUTLINER_SHORTCUTS_LOGV__(Format, ##__VA_ARGS__);
#define __OUTLINER_SHORTCUTS_LOGV__(Format, ...) UE_LOG(LogOutlinerShortcutsEditor, Log, TEXT(Format), ##__VA_ARGS__);
#endif

#ifndef OUTLINER_SHORTCUTS_LOGV_H
/**
* @brief LOG VERBOSE + HERE. Calls UE_LOG(LogOutlinerShortcutsEditor, Log, ...).
*/
#define OUTLINER_SHORTCUTS_LOGV_H(Format, ...) __OUTLINER_SHORTCUTS_LOGVH__(Format, ##__VA_ARGS__);
#define __OUTLINER_SHORTCUTS_LOGVH__(Format, ...) UE_LOG(LogOutlinerShortcutsEditor, Log, TEXT("[%hs]  " Format), __FUNCTION__, ##__VA_ARGS__);
#endif

#ifndef OUTLINER_SHORTCUTS_WARN
/**
* @brief WARN. Calls UE_LOG(LogOutlinerShortcutsEditor, Warning, ...). 
*/
#define OUTLINER_SHORTCUTS_WARN(Format, ...) __OUTLINER_SHORTCUTS_WARN__(Format, ##__VA_ARGS__);
#define __OUTLINER_SHORTCUTS_WARN__(Format, ...) UE_LOG(LogOutlinerShortcutsEditor, Warning, TEXT(Format), ##__VA_ARGS__);
#endif

#ifndef OUTLINER_SHORTCUTS_WARN_H
/**
* @brief WARN + HERE. Calls UE_LOG(LogOutlinerShortcutsEditor, Warning, ...).
*/
#define OUTLINER_SHORTCUTS_WARN_H(Format, ...) __OUTLINER_SHORTCUTS_WARNH__(Format, ##__VA_ARGS__);
#define __OUTLINER_SHORTCUTS_WARNH__(Format, ...) UE_LOG(LogOutlinerShortcutsEditor, Warning, TEXT("[%hs]  " Format), __FUNCTION__, ##__VA_ARGS__);
#endif

#ifndef OUTLINER_SHORTCUTS_WARN_D
#if DEBUG_OUTLINER_SHORTCUTS
/**
* @brief WARN DEBUG. Calls UE_LOG(LogOutlinerShortcutsEditor, Warning, ...) if DEBUG_OUTLINER_SHORTCUTS is set to 1.
*/
#define OUTLINER_SHORTCUTS_WARN_D(Format, ...) OUTLINER_SHORTCUTS_WARN_H(Format,##__VA_ARGS__);
#else // not DEBUG_OUTLINER_SHORTCUTS
/**
* @brief WARN DEBUG. Calls UE_LOG(LogOutlinerShortcutsEditor, Warning, ...) if DEBUG_OUTLINER_SHORTCUTS is set to 1.
*/
#define OUTLINER_SHORTCUTS_WARN_D(Format, ...) {}
#endif //DEBUG_OUTLINER_SHORTCUTS
#endif

#ifndef OUTLINER_SHORTCUTS_ERROR
/**
* @brief ERROR. Calls UE_LOG(LogOutlinerShortcutsEditor, Error, ...).
*/
#define OUTLINER_SHORTCUTS_ERROR(Format, ...) __OUTLINER_SHORTCUTS_ERROR__(Format, ##__VA_ARGS__);
#define __OUTLINER_SHORTCUTS_ERROR__(Format, ...) UE_LOG(LogOutlinerShortcutsEditor, Error, TEXT(Format), ##__VA_ARGS__);
#endif

#ifndef OUTLINER_SHORTCUTS_ERROR_H
/**
* @brief ERROR + HERE. Calls UE_LOG(LogOutlinerShortcutsEditor, Error, ...).
*/
#define OUTLINER_SHORTCUTS_ERROR_H(Format, ...) __OUTLINER_SHORTCUTS_ERRORH__(Format, ##__VA_ARGS__);
#define __OUTLINER_SHORTCUTS_ERRORH__(Format, ...) UE_LOG(LogOutlinerShortcutsEditor, Error, TEXT("[%hs]  " Format), __FUNCTION__, ##__VA_ARGS__);
#endif


#ifndef __OUTLINER_SHORTCUTS_FILENAME__
/**
* @brief Returns the current Filename
*/
#define __OUTLINER_SHORTCUTS_FILENAME__ (wcsrchr(TEXT(__FILE__), '\\') ? wcsrchr(TEXT(__FILE__), '\\') + 1 : TEXT(__FILE__))
#endif

#ifndef OUTLINER_SHORTCUTS_WHERE
/**
* @brief Logs where this function has been called.
*/
#define OUTLINER_SHORTCUTS_WHERE OUTLINER_SHORTCUTS_LOG("%s - R%i - %hs", __OUTLINER_SHORTCUTS_FILENAME__, __LINE__, __FUNCTION__);
#endif

#ifndef OUTLINER_SHORTCUTS_HERE
/**
* @brief Prints the current function name in the Output.
*/
#define OUTLINER_SHORTCUTS_HERE OUTLINER_SHORTCUTS_LOGV(" ==== [%hs] ==== ", __FUNCTION__);
#endif

#ifndef OUTLINER_SHORTCUTS_HERE_D
#if DEBUG_OUTLINER_SHORTCUTS
/**
* @brief Prints the current function name in the Output if DEBUG_OUTLINER_SHORTCUTS is set to 1.
*/
#define OUTLINER_SHORTCUTS_HERE_D OUTLINER_SHORTCUTS_HERE;
#else // not def DEBUG_OUTLINER_SHORTCUTS
/**
* @brief Prints the current function name in the Output if DEBUG_OUTLINER_SHORTCUTS is set to 1.
*/
#define OUTLINER_SHORTCUTS_HERE_D {}
#endif //DEBUG_OUTLINER_SHORTCUTS
#endif


#ifndef OUTLINER_SHORTCUTS_BtoS
/**
* @brief Converts a bool to *CHAR for logging
*/
#define OUTLINER_SHORTCUTS_BtoS(var) var ? TEXT("True") : TEXT("False")
#endif



#define DEFINITION_FILE "OutlinerShortcutsEditorDefinitions"
#pragma message("  [" DEFINITION_FILE "]  COMPILING ON UE `" UE_STRINGIZE(ENGINE_MAJOR_VERSION) "`.`" UE_STRINGIZE(ENGINE_MINOR_VERSION) "`.`" UE_STRINGIZE(ENGINE_PATCH_VERSION) "`  [`" UE_STRINGIZE(BRANCH_NAME) "`]")

#undef DEFINITION_FILE
