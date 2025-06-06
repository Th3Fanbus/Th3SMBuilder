/* SPDX-License-Identifier: MPL-2.0 */

#include "Th3SMBuilderSubsystem.h"
#include "Algo/AllOf.h"
#include "Algo/Transform.h"
#include "Kismet/GameplayStatics.h"
#include "MaterialDomain.h"

ATh3SMBuilderSubsystem* ATh3SMBuilderSubsystem::Get(UObject* WorldContext)
{
	return Cast<ATh3SMBuilderSubsystem>(UGameplayStatics::GetActorOfClass(WorldContext, ATh3SMBuilderSubsystem::StaticClass()));
}

void ATh3SMBuilderSubsystem::GetFilteredEntries(TArray<UMaterialEntry*>& out_FilteredEntries, const FString& SearchQuery) const
{
	TArray<FString> SearchWords;
	SearchQuery.ParseIntoArrayWS(SearchWords);
	
	if (not bEntriesReady) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("ENTRIES STILL NOT READY, THERE ARE %d ENTRIES"), MaterialEntries.Num());
	}
	
	const TFunction<bool(const TPair<UMaterialInterface*, UMaterialEntry*>&)> predicate_none = [](const TPair<UMaterialInterface*, UMaterialEntry*>& Pair) {
		return true;
	};
	const TFunction<bool(const TPair<UMaterialInterface*, UMaterialEntry*>&)> predicate_find = [&SearchWords](const TPair<UMaterialInterface*, UMaterialEntry*>& Pair) {
		const FString Path = Pair.Key->GetPathName();
		return Algo::AllOf(SearchWords, [Path](const FString& Word) { return Path.Contains(Word); });
	};
	const auto transform = [](const TPair<UMaterialInterface*, UMaterialEntry*>& Pair) {
		return Pair.Value;
	};
	Algo::TransformIf(MaterialEntries, out_FilteredEntries, SearchWords.IsEmpty() ? predicate_none : predicate_find, transform);
}

TArray<UMaterialInterface*> ATh3SMBuilderSubsystem::GetMaterialsGame()
{
	UTh3SMBuilderRootInstance* RootInstance = UTh3SMBuilderRootInstance::Get(this);
	if (not RootInstance) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("Got nullptr RootInstance"));
		return TArray<UMaterialInterface*>();
	}
	return RootInstance->Materials;
}

TArray<UMaterialInterface*> ATh3SMBuilderSubsystem::GetMaterials()
{
	if (GIsEditor) {
		return GetMaterialsEditor();
	} else {
		return GetMaterialsGame();
	}
}

UMaterialEntry* ATh3SMBuilderSubsystem::MakeMaterialEntry(UMaterialInterface* Material, ASMBuilderPhotoBooth* PhotoBooth) const
{
	const EMaterialDomain Domain = Material->GetBaseMaterial()->MaterialDomain;
	//UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Making %s Material entry for '%s'"), *UEnum::GetValueAsString(Domain), *Material->GetPathName());

	UMaterialEntry* MaterialEntry = NewObject<UMaterialEntry>();

	switch (Domain) {
	case MD_Surface:
		MaterialEntry->Brush = PhotoBooth->RenderSurfaceMaterial(Material, BrushSize);
		break;
	default:
		MaterialEntry->Brush.SetResourceObject(Material);
		MaterialEntry->Brush.ImageSize = FVector2D(BrushSize, BrushSize);
		break;
	}
	MaterialEntry->Material = Material;
	return MaterialEntry;
}

void ATh3SMBuilderSubsystem::BeginPlay()
{
	Super::BeginPlay();

	TArray<UMaterialInterface*> MaterialInterfaces = GetMaterials();

	if (MaterialInterfaces.IsEmpty()) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("No materials to process"));
		return;
	}
	UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Sorting %d materials..."), MaterialInterfaces.Num());

	Algo::Sort(MaterialInterfaces, [](const UMaterialInterface* A, const UMaterialInterface* B) {
		return A->GetFName().Compare(B->GetFName()) < 0;
	});

	UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Processing %d materials..."), MaterialInterfaces.Num());

	ASMBuilderPhotoBooth* PhotoBooth = Cast<ASMBuilderPhotoBooth>(GetWorld()->SpawnActor(PhotoBoothClass.Get()));
	if (not PhotoBooth) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("Got nullptr PhotoBooth"));
		return;
	}
	for (UMaterialInterface* Material : MaterialInterfaces) {
		MaterialEntries.Add(Material, MakeMaterialEntry(Material, PhotoBooth));
	}
	UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Done processing materials"));

	bEntriesReady = true;
	PhotoBooth->Destroy();
}
