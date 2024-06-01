/* SPDX-License-Identifier: MPL-2.0 */

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Th3SMBuilderBPFL.generated.h"

UCLASS()
class TH3SMBUILDER_API UTh3SMBuilderBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Th3SMBuilderBPFL")
	static void SplitIntoWords(TArray<FString>& out_SearchWords, const FString& SearchQuery);

	UFUNCTION(BlueprintCallable, Category = "Th3SMBuilderBPFL")
	static bool ContainsAllWords(const FString& Str, const TArray<FString>& SearchWords);
};
