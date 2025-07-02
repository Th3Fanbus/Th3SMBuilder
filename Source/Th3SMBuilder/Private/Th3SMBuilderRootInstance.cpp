/* SPDX-License-Identifier: MPL-2.0 */

#include "Th3SMBuilderRootInstance.h"
#include "Th3Utilities.h"

#include "Containers/EnumAsByte.h"
#include "Registry/ModContentRegistry.h"
#include "Resources/FGBuildingDescriptor.h"
#include "Module/GameInstanceModuleManager.h"
#include "FGCustomizationRecipe.h"
#include "FGRecipe.h"
#include "FGRecipeManager.h"
#include "FGSchematic.h"
#include "Logging/LogMacros.h"
#include "Logging/StructuredLog.h"
#include "UObject/UObjectGlobals.h"
#include "Algo/Accumulate.h"
#include "Algo/AllOf.h"
#include "Algo/AnyOf.h"
#include "Algo/ForEach.h"
#include "Algo/Reverse.h"
#include "Algo/Transform.h"

UTh3SMBuilderRootInstance::UTh3SMBuilderRootInstance()
{
	UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Hello Game Instance %s"), *this->GetPathName());
}

UTh3SMBuilderRootInstance::~UTh3SMBuilderRootInstance()
{
	UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Goodbye Cruel Game Instance"));
}

TSubclassOf<UFGBuildCategory> UTh3SMBuilderRootInstance::MakeCategory()
{
	const int32 Idx = Priority / EntriesPerCategory;
	
	if (BuildCategories.IsValidIndex(Idx)) {
		return BuildCategories[Idx];
	}
	const FString PackagePath = MOD_TRANSIENT_ROOT / TEXT("Categories");
	const FString ClassName = FString::Printf(TEXT("Cat_%d"), Idx);
	TSubclassOf<UFGBuildCategory> Category = Th3Utilities::GenerateNewClass(PackagePath, ClassName, UFGBuildCategory::StaticClass());
	if (not Category) {
		UE_LOG(LogTh3SMBuilderCpp, Fatal, TEXT("Failed to generate Category for %s %s"), *PackagePath, *ClassName);
		return nullptr;
	}
	UFGBuildCategory* CDO = Category.GetDefaultObject();
	CDO->mDisplayName = FText::FromString(ClassName);
	CDO->mCategoryIcon = UFGCategory::GetCategoryIcon(BuildCategory);
	CDO->mMenuPriority = Idx + 42;
	BuildCategories.Add(Category);
	return Category;
}

void UTh3SMBuilderRootInstance::MakeBuildable(UStaticMesh* Mesh)
{
	const FString PackagePath = MOD_TRANSIENT_ROOT / TEXT("Buildables") / Mesh->GetPackage()->GetName();
	const FString ClassName = FString::Printf(TEXT("Build_%s"), *Mesh->GetName());
	TSubclassOf<ATh3BuildableSM> Buildable = Th3Utilities::GenerateNewClass(PackagePath, ClassName, ATh3BuildableSM::StaticClass());
	if (not Buildable) {
		UE_LOG(LogTh3SMBuilderCpp, Fatal, TEXT("Failed to generate buildable for %s %s"), *PackagePath, *ClassName);
		return;
	}
	ATh3BuildableSM* CDO = Buildable.GetDefaultObject();
	CDO->mDisplayName = FText::FromString(ClassName);
	CDO->mDescription = FText::FromString(Mesh->GetPathName());
	CDO->mHologramClass = HologramClass;
	CDO->mInteractWidgetSoftClass = InteractWidgetClass;
	CDO->FallbackMaterial = FallbackMaterial;
	CDO->CollisionProfile = CollisionProfile;
	CDO->SetMesh(Mesh);

	Buildables.Add(CDO);

	//UE_LOG(LogTh3SMBuilderCpp, Warning, TEXT("[PRIOWTF]\t%d\tBUILD\t%s"), Priority, *Th3::GetPathSafe(Buildable));

	MakeBuildingDescriptor(Buildable);
}

void UTh3SMBuilderRootInstance::MakeBuildingDescriptor(TSubclassOf<ATh3BuildableSM> Buildable)
{
	const FString PackagePath = MOD_TRANSIENT_ROOT / TEXT("BuildingDesc") / Buildable->GetPackage()->GetName();
	const FString ClassName = FString::Printf(TEXT("Desc_%s"), *Buildable->GetName());
	TSubclassOf<UFGBuildingDescriptor> BuildDesc = Th3Utilities::GenerateNewClass(PackagePath, ClassName, UFGBuildingDescriptor::StaticClass());
	if (not BuildDesc) {
		UE_LOG(LogTh3SMBuilderCpp, Fatal, TEXT("Failed to generate desc for %s %s"), *PackagePath, *ClassName);
		return;
	}
	UFGBuildingDescriptor* CDO = BuildDesc.GetDefaultObject();
	CDO->mBuildableClass = Buildable;
	CDO->mSmallIcon = ItemIcon;
	CDO->mPersistentBigIcon = ItemIcon;
	CDO->mCategory = MakeCategory();
	CDO->mSubCategories.Add(BuildSubCategory);
	CDO->mMenuPriority = Priority + 42;

	MakeBuildingRecipe(BuildDesc);
}

void UTh3SMBuilderRootInstance::MakeBuildingRecipe(TSubclassOf<UFGBuildingDescriptor> BuildDesc)
{
	const FString PackagePath = MOD_TRANSIENT_ROOT / TEXT("Recipes") / BuildDesc->GetPackage()->GetName();
	const FString ClassName = FString::Printf(TEXT("Recipe_%s"), *BuildDesc->GetName());
	TSubclassOf<UFGRecipe> Recipe = Th3Utilities::GenerateNewClass(PackagePath, ClassName, UFGRecipe::StaticClass());
	if (not Recipe) {
		UE_LOG(LogTh3SMBuilderCpp, Fatal, TEXT("Failed to generate recipe for %s %s"), *PackagePath, *ClassName);
		return;
	}
	UFGRecipe* CDO = Recipe.GetDefaultObject();
	CDO->mProduct.Add(FItemAmount(BuildDesc, 1));
	CDO->mProducedIn.Add(BuildGunClass);

	ModifiedUnlock->mRecipes.Add(Recipe);
}

void UTh3SMBuilderRootInstance::ProcessOneSM(const TSoftObjectPtr<UStaticMesh>& MeshPtr)
{
	UStaticMesh* Mesh = MeshPtr.Get();
	if (not Mesh) {
		//UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("Got nullptr StaticMesh"));
		return;
	}
	if (Mesh->HasAnyFlags(RF_ClassDefaultObject)) {
		return;
	}
	StaticMeshes.Add(Mesh);
	MakeBuildable(Mesh);
	Priority++;
}

void UTh3SMBuilderRootInstance::ProcessOneMat(const FSoftObjectPath& MatPath)
{
	UMaterialInterface* Mat = TSoftObjectPtr<UMaterialInterface>(MatPath).Get();
	if (not Mat) {
		//UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("Got nullptr Material"));
		return;
	}
	if (Mat->HasAnyFlags(RF_ClassDefaultObject)) {
		return;
	}
	//UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Processing Material %s"), *Mat->GetName());

	Materials.Add(Mat);
}

void UTh3SMBuilderRootInstance::ProcessStaticMeshes()
{
	const auto store_paths = [this](const TArray<FSoftObjectPath>& InPaths) {
		Algo::Transform(InPaths, SMPtrs, &ToSoftObjectPtr<UStaticMesh>);
	};
	const auto proc_paths = [this]() {
		Algo::ForEach(SMPtrs, TH3_PROJECTION_THIS(ProcessOneSM));
		bBuildablesReady = true;
		UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Buildableabled %d static meshes"), StaticMeshes.Num());
	};
	ProcessAllOf(UStaticMesh::StaticClass(), store_paths, proc_paths);
}

void UTh3SMBuilderRootInstance::ProcessMaterialInterfaces()
{
	LoadAsync(UMaterialInterface::StaticClass(), [this](const TArray<FSoftObjectPath>& Paths) {
		Algo::ForEach(Paths, TH3_PROJECTION_THIS(ProcessOneMat));
		bMaterialsReady = true;
	});
}

void UTh3SMBuilderRootInstance::DispatchLifecycleEvent(ELifecyclePhase Phase)
{
	Super::DispatchLifecycleEvent(Phase);

	UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Dispatching Phase %s on %s"), *LifecyclePhaseToString(Phase), *this->GetPathName());

	if (Phase == ELifecyclePhase::POST_INITIALIZATION) {
		ModifiedSchematicCDO = SchematicClass.GetDefaultObject();
		if (not ModifiedSchematicCDO or ModifiedSchematicCDO->mUnlocks.Num() < 1) {
			UE_LOG(LogTh3SMBuilderCpp, Fatal, TEXT("COULD NOT GET SCHEMATIC UNLOCK, THINGS WILL NOT WORK"));
			return;
		}
		ModifiedUnlock = Cast<UFGUnlockRecipe>(ModifiedSchematicCDO->mUnlocks[0]);

		fgcheck(ModifiedUnlock);
		ProcessStaticMeshes();
		ProcessMaterialInterfaces();
	}
}

UTh3SMBuilderRootInstance* UTh3SMBuilderRootInstance::Get(UWorld* World)
{
	if (not World) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("  - nullptr World"));
		return nullptr;
	}
	UGameInstance* GameInstance = World->GetGameInstance();
	if (not GameInstance) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("  - nullptr GameInstance"));
		return nullptr;
	}
	UGameInstanceModuleManager* ModuleManager = GameInstance->GetSubsystem<UGameInstanceModuleManager>();
	if (not ModuleManager) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("  - nullptr ModuleManager"));
		return nullptr;
	}
	return Cast<UTh3SMBuilderRootInstance>(ModuleManager->FindModule(TEXT("Th3SMBuilder")));
}

UTh3SMBuilderRootInstance* UTh3SMBuilderRootInstance::Get(UObject* WorldContext)
{
	if (not WorldContext) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("  - nullptr WorldContext"));
		return nullptr;
	}
	return UTh3SMBuilderRootInstance::Get(WorldContext->GetWorld());
}
