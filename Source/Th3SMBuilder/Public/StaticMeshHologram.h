/* SPDX-License-Identifier: MPL-2.0 */

#pragma once

#include "CoreMinimal.h"
#include "Hologram/FGBuildableHologram.h"
#include "StaticMeshHologram.generated.h"

UCLASS()
class TH3SMBUILDER_API AStaticMeshHologram : public AFGBuildableHologram
{
	GENERATED_BODY()
public:
	AStaticMeshHologram();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsValidHitResult(const FHitResult& hitResult) const override;
	virtual void SetHologramLocationAndRotation(const FHitResult& hitResult) override;
	virtual float GetBuildGunRangeOverride_Implementation() const override;
};
