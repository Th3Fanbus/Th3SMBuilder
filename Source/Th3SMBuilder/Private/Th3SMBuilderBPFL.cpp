/* SPDX-License-Identifier: MPL-2.0 */

#include "Th3SMBuilderBPFL.h"
#include "Algo/AllOf.h"

void UTh3SMBuilderBPFL::SplitIntoWords(TArray<FString>& out_SearchWords, const FString& SearchQuery)
{
	SearchQuery.ParseIntoArrayWS(out_SearchWords);
}

bool UTh3SMBuilderBPFL::ContainsAllWords(const FString& Str, const TArray<FString>& SearchWords)
{
	return Algo::AllOf(SearchWords, [Str](const FString& Word) { return Str.Contains(Word); });
}
