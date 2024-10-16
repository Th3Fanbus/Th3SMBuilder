/* SPDX-License-Identifier: MPL-2.0 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMBuilderPhotoBooth.generated.h"

UCLASS(Abstract)
class TH3SMBUILDER_API ASMBuilderPhotoBooth : public AActor
{
	GENERATED_BODY()
public:
	ASMBuilderPhotoBooth();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	FSlateBrush RenderSurfaceMaterial(UMaterialInterface* Material, int32 Size);
};
