/* SPDX-License-Identifier: MPL-2.0 */

#pragma once

#include "CoreMinimal.h"
#include "Th3Utilities.h"
#include "Th3SMBuilder.h"
#include "Th3BuildableSM.h"

#include "Module/GameInstanceModule.h"
#include "Resources/FGItemDescriptor.h"
#include "Resources/FGBuildingDescriptor.h"
#include "FGBuildCategory.h"
#include "FGRecipe.h"
#include "FGSchematic.h"
#include "Unlocks/FGUnlock.h"
#include "Unlocks/FGUnlockRecipe.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "Th3SMBuilderRootInstance.generated.h"

UCLASS(Abstract)
class TH3SMBUILDER_API UTh3SMBuilderRootInstance : public UGameInstanceModule
{
	GENERATED_BODY()
	friend class UTh3SMBuilderRootGame;
public:
	/* Marked as UPROPERTY because it holds CDO edits */
	UPROPERTY()
	UFGSchematic* ModifiedSchematicCDO;

	UPROPERTY()
	UFGUnlockRecipe* ModifiedUnlock;

	UPROPERTY(BlueprintReadWrite)
	TArray<UStaticMesh*> StaticMeshes;

	UPROPERTY(BlueprintReadWrite)
	TArray<UMaterialInterface*> Materials;

	UPROPERTY(BlueprintReadWrite)
	TArray<ATh3BuildableSM*> Buildables;

	UPROPERTY(BlueprintReadWrite)
	TArray<TSoftObjectPtr<UStaticMesh>> SMPtrs;

	UPROPERTY(BlueprintReadWrite)
	TArray<TSoftObjectPtr<UMaterialInterface>> MatPtrs;
public:
	std::atomic_bool bMaterialsReady;
	std::atomic_bool bBuildablesReady;
public:
	/* All generated classes are somewhere in here */
	const FString MOD_TRANSIENT_ROOT = TEXT("/Th3SMBuilder");

	UTh3SMBuilderRootInstance();
	~UTh3SMBuilderRootInstance();
	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;

	static UTh3SMBuilderRootInstance* Get(UWorld* World);
	static UTh3SMBuilderRootInstance* Get(UObject* WorldContext);
protected:
	int32 Priority = 0;
	
	UPROPERTY()
	TArray<TSubclassOf<UFGBuildCategory>> BuildCategories;

	TSubclassOf<UFGBuildCategory> MakeCategory();
	void MakeBuildable(UStaticMesh* Mesh);
	void MakeBuildingDescriptor(TSubclassOf<ATh3BuildableSM> Buildable);
	void MakeBuildingRecipe(TSubclassOf<UFGBuildingDescriptor> BuildDesc);

	void ProcessOneSM(const TSoftObjectPtr<UStaticMesh>& MeshPtr);
	void ProcessStaticMeshes();

	void ProcessOneMat(const FSoftObjectPath& MatPath);
	void ProcessMaterialInterfaces();

	void ProcessAllOf(UClass* BaseClass, const TFunction<void(const TArray<FSoftObjectPath>&)> StoreList, const TFunction<void()> Callback)
	{
		const FString ClassName = BaseClass->GetName();
		UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Looking for '%s'..."), *ClassName);
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByClass(FTopLevelAssetPath(BaseClass), AssetData, true);
		TArray<FSoftObjectPath> SoftPaths;
		const auto asset_predicate = [](const FAssetData& Asset) { return not Asset.PackageName.ToString().StartsWith(TEXT("/ControlRig")); };
		const auto asset_transform = [](const FAssetData& Asset) { return Asset.GetSoftObjectPath(); };
		Algo::TransformIf(AssetData, SoftPaths, asset_predicate, asset_transform);
		Invoke(StoreList, SoftPaths);
		UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Processing %d '%s'..."), SoftPaths.Num(), *ClassName);
		const double Begin = FPlatformTime::Seconds();
		UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPaths, [Begin, ClassName, Callback]() {
			const double Middle = FPlatformTime::Seconds();
			UE_LOG(LogTh3SMBuilderCpp, Warning, TEXT("Took %f ms to load '%s'"), (Middle - Begin) * 1000, *ClassName);
			Invoke(Callback);
			const double End = FPlatformTime::Seconds();
			UE_LOG(LogTh3SMBuilderCpp, Warning, TEXT("Took %f ms to process '%s'"), (End - Middle) * 1000, *ClassName);
		}, FStreamableManager::AsyncLoadHighPriority);
	}

	template<typename T>
	static TSoftObjectPtr<T> ToSoftObjectPtr(const FSoftObjectPath& Path)
	{
		return TSoftObjectPtr<T>(Path);
	}

	void LoadAsync(UClass* BaseClass, const TFunction<void(const TArray<FSoftObjectPath>&)> Callback)
	{
		const FString ClassName = BaseClass->GetName();
		UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Looking for '%s'..."), *ClassName);
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByClass(FTopLevelAssetPath(BaseClass), AssetData, true);
		TArray<FSoftObjectPath> SoftPaths;
		const auto asset_predicate = [](const FAssetData& Asset) { return not Asset.PackageName.ToString().StartsWith(TEXT("/ControlRig")); };
		const auto asset_transform = [](const FAssetData& Asset) { return Asset.GetSoftObjectPath(); };
		Algo::TransformIf(AssetData, SoftPaths, asset_predicate, asset_transform);
		UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Loading %d '%s'..."), SoftPaths.Num(), *ClassName);
		const double Begin = FPlatformTime::Seconds();
		UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPaths, [Begin, ClassName, SoftPaths, Callback]() {
			const double Middle = FPlatformTime::Seconds();
			UE_LOG(LogTh3SMBuilderCpp, Warning, TEXT("Took %f ms to load %d '%s'"), (Middle - Begin) * 1000, SoftPaths.Num(), *ClassName);
			Invoke(Callback, SoftPaths);
			const double End = FPlatformTime::Seconds();
			UE_LOG(LogTh3SMBuilderCpp, Warning, TEXT("Took %f ms to process %d '%s'"), (End - Middle) * 1000, SoftPaths.Num(), *ClassName);
		}, FStreamableManager::AsyncLoadHighPriority);
	}
public:
	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	const TSubclassOf<UFGCategory> BuildCategory;

	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	const TSubclassOf<UFGCategory> BuildSubCategory;

	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	int32 EntriesPerCategory = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	UTexture2D* ItemIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	const TSubclassOf<AFGHologram> HologramClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration", Meta = (MustImplement = "/Script/FactoryGame.FGRecipeProducerInterface"))
	const TSoftClassPtr<UObject> BuildGunClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	const TSubclassOf<UFGInteractWidget> InteractWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	const TSubclassOf<UFGSchematic> SchematicClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	UMaterialInterface* FallbackMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Mod Configuration")
	FCollisionProfileName CollisionProfile;
};
