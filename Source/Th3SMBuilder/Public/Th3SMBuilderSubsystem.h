/* SPDX-License-Identifier: MPL-2.0 */

#pragma once

#include "Th3SMBuilder.h"
#include "Th3SMBuilderRootInstance.h"
#include "SMBuilderPhotoBooth.h"
#include "MaterialEntry.h"
#include "CoreMinimal.h"
#include "Subsystem/ModSubsystem.h"
#include "Th3SMBuilderSubsystem.generated.h"

UCLASS(Abstract)
class TH3SMBUILDER_API ATh3SMBuilderSubsystem : public AModSubsystem
{
	GENERATED_BODY()
public:
	static ATh3SMBuilderSubsystem* Get(UObject* WorldContext);

	UFUNCTION(BlueprintCallable)
	bool AreEntriesReady()
	{
		return bEntriesReady;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	void GetFilteredEntries(TArray<UMaterialEntry*>& out_FilteredEntries, const FString& SearchQuery) const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	TArray<UMaterialInterface*> GetMaterialsEditor();
	TArray<UMaterialInterface*> GetMaterialsGame();
	TArray<UMaterialInterface*> GetMaterials();

	UMaterialEntry* MakeMaterialEntry(UMaterialInterface* Material, ASMBuilderPhotoBooth* PhotoBooth) const;

	virtual void BeginPlay() override;

	std::atomic_bool bEntriesReady;

	UPROPERTY(BlueprintReadWrite)
	TMap<UMaterialInterface*, UMaterialEntry*> MaterialEntries;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	const TSubclassOf<ASMBuilderPhotoBooth> PhotoBoothClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	int32 BrushSize = 64;
};
