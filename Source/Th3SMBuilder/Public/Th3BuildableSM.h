/* SPDX-License-Identifier: MPL-2.0 */

#pragma once

#include "CoreMinimal.h"
#include "Buildables/FGBuildable.h"
#include "Th3BuildableSM.generated.h"

UCLASS()
class TH3SMBUILDER_API ATh3BuildableSM : public AFGBuildable
{
	GENERATED_BODY()
	friend class UTh3SMBuilderRootInstance;
public:
	ATh3BuildableSM(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual ~ATh3BuildableSM() noexcept;

	void SetMesh(UStaticMesh* NewMesh);

	UFUNCTION(BlueprintCallable)
	void SetMaterialForIndex(int32 Index, UMaterialInterface* Material);

	UFUNCTION(BlueprintCallable)
	FText GetSearchText() const;

	virtual void BeginPlay() override;

	/*
	 * Begin IFGDismantleInterface
	 */
	void Dismantle_Implementation();
	void StartIsLookedAtForDismantle_Implementation(AFGCharacterPlayer* byCharacter);
	void StopIsLookedAtForDismantle_Implementation(AFGCharacterPlayer* byCharacter);
	/*
	 * End IFGDismantleInterface
	 */

	ATh3BuildableSM* GetSubclassDefault() const
	{
		return CastChecked<ATh3BuildableSM>(GetClass()->GetDefaultObject());
	}

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UMaterialInterface* FallbackMaterial;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FCollisionProfileName CollisionProfile;

	UPROPERTY(BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	int32 MaterialSlotIndex;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString SearchString;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	TArray<UMaterialInterface*> OverriddenMaterials;
};
