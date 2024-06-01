/* SPDX-License-Identifier: MPL-2.0 */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTh3SMBuilderCpp, Log, All);

class FTh3SMBuilderModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

namespace Th3
{
	template<typename T>
	FORCEINLINE FString GetPathSafe(const T* Obj)
	{
		return Obj ? Obj->GetPathName() : FString::Printf(TEXT("NULL %s"), *T::StaticClass()->GetName());
	}

	template<typename T>
	FORCEINLINE FString GetPathSafe(const TSubclassOf<T> Class)
	{
		return GetPathSafe(Class.Get());
	}
}