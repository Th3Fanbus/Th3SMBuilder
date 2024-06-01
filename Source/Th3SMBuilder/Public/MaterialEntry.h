/* SPDX-License-Identifier: MPL-2.0 */

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Th3BuildableSM.h"
#include "MaterialEntry.generated.h"

UCLASS(BlueprintType)
class TH3SMBUILDER_API UMaterialEntry : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	UMaterialInterface* Material;

	UPROPERTY(BlueprintReadWrite)
	FSlateBrush Brush;
};
