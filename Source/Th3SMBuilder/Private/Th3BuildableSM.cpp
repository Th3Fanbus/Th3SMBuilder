/* SPDX-License-Identifier: MPL-2.0 */

#include "Th3BuildableSM.h"
#include "Th3SMBuilder.h"
#include "FGCharacterPlayer.h"

#define COLLISION_CHANNEL_BUILDGUN	(ECollisionChannel::ECC_GameTraceChannel5)
#define COLLISION_CHANNEL_INTERACT	(ECollisionChannel::ECC_GameTraceChannel13)

ATh3BuildableSM::ATh3BuildableSM(const FObjectInitializer& ObjectInitializer) : AFGBuildable(ObjectInitializer)
{
	mIsUseable = true;
	mSkipBuildEffect = true;
	SetActorEnableCollision(true);

	if (not HasAnyFlags(RF_ClassDefaultObject)) {
		/* Ugly, but CDO property propagation doesn't work for runtime generated classes */
		ATh3BuildableSM* CDO = GetSubclassDefault();
		Mesh = CDO->Mesh;
		mDisplayName = CDO->mDisplayName;
		mHologramClass = CDO->mHologramClass;
		mInteractWidgetClass = CDO->mInteractWidgetClass;
		FallbackMaterial = CDO->FallbackMaterial;
		CollisionProfile = CDO->CollisionProfile;
		if (not Mesh) {
			UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("[%s] MESH NOT SET FOR %s"), *FString(__func__), *this->GetPathName());
		}
		if (not FallbackMaterial) {
			UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("[%s] FALLBACK MAT NOT SET FOR %s"), *FString(__func__), *this->GetPathName());
		}
	}

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent0"));
	MeshComponent->Mobility = EComponentMobility::Movable;
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetStaticMesh(Mesh);
	MeshComponent->SetForceDisableNanite(true);
	//MeshComponent->SetCollisionResponseToChannel(COLLISION_CHANNEL_BUILDGUN, ECollisionResponse::ECR_Block);
	//MeshComponent->SetCollisionResponseToChannel(COLLISION_CHANNEL_INTERACT, ECollisionResponse::ECR_Overlap);
	//MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//MeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	MeshComponent->SetCollisionProfileName(CollisionProfile.Name, true);
	MeshComponent->UpdateCollisionFromStaticMesh();
}

ATh3BuildableSM::~ATh3BuildableSM()
{
	//UE_LOG(LogTh3SMBuilderCpp, Warning, TEXT("DESTRUCTOR %s"), *FString(__func__));
}

void ATh3BuildableSM::SetMesh(UStaticMesh* NewMesh)
{
	if (not MeshComponent) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("[%s] MESH COMP NOT SET"), *FString(__func__));
		return;
	}
	Mesh = NewMesh;
	MeshComponent->SetStaticMesh(Mesh);
	for (int32 Index = 0; Index < OverriddenMaterials.Num(); Index++) {
		MeshComponent->SetMaterial(Index, OverriddenMaterials[Index]);
	}
}

void ATh3BuildableSM::SetMaterialForIndex(int32 Index, UMaterialInterface* InMaterial)
{
	if (not MeshComponent) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("[%s] MESH COMP NOT SET"), *FString(__func__));
		return;
	}
	UMaterialInterface* Material = IsValid(InMaterial) ? InMaterial : FallbackMaterial;
	MeshComponent->SetMaterial(Index, Material);

	/*
	 * Grow the array if the new index is too large,
	 * but make sure we initialize all new elements.
	 */
	if (OverriddenMaterials.Num() <= Index) {
		const int32 OldCount = OverriddenMaterials.Num();
		const int32 NumToAdd = Index - OldCount;
		OverriddenMaterials.AddZeroed(NumToAdd + 1);
		for (int32 Idx = 0; Idx < NumToAdd; Idx++) {
			OverriddenMaterials[OldCount + Idx] = FallbackMaterial;
		}
	}
	OverriddenMaterials[Index] = Material;

	UE_LOG(LogTh3SMBuilderCpp, Warning, TEXT("[%s] Materials for %s:"), *FString(__func__), *this->GetPathName());
	for (int32 Idx = 0; Idx < OverriddenMaterials.Num(); Idx++) {
		UE_LOG(LogTh3SMBuilderCpp, Warning, TEXT("  - [%d] %s"), Idx, *Th3::GetPathSafe(OverriddenMaterials[Index]));
	}
}

void ATh3BuildableSM::BeginPlay()
{
	AFGBuildable::BeginPlay();

	if (not MeshComponent) {
		UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("[%s] MESH COMP NOT SET"), *FString(__func__));
		return;
	}

	if (OverriddenMaterials.IsEmpty()) {
		OverriddenMaterials = MeshComponent->GetMaterials();
	}
	UE_LOG(LogTh3SMBuilderCpp, Warning, TEXT("Got %d material slots for %s"), OverriddenMaterials.Num(), *this->GetPathName());
	for (int32 Index = 0; Index < OverriddenMaterials.Num(); Index++) {
		UMaterialInterface* Material = OverriddenMaterials[Index];
		if (not IsValid(Material)) {
			Material = FallbackMaterial;
		}
		MeshComponent->SetMaterial(Index, Material);
	}
}

FText ATh3BuildableSM::GetSearchText() const
{
	return FText::FromString(SearchString);
}

void ATh3BuildableSM::Dismantle_Implementation()
{
	//UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("[%s] %s"), *FString(__func__), *Th3::GetPathSafe(this));

	AFGBuildable::Dismantle_Implementation();
}

void ATh3BuildableSM::StartIsLookedAtForDismantle_Implementation(AFGCharacterPlayer* byCharacter)
{
	//UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("[%s] %s by %s"), *FString(__func__), *Th3::GetPathSafe(this), *Th3::GetPathSafe(byCharacter));

	AFGBuildable::StartIsLookedAtForDismantle_Implementation(byCharacter);
}

void ATh3BuildableSM::StopIsLookedAtForDismantle_Implementation(AFGCharacterPlayer* byCharacter)
{
	//UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("[%s] %s by %s"), *FString(__func__), *Th3::GetPathSafe(this), *Th3::GetPathSafe(byCharacter));

	AFGBuildable::StopIsLookedAtForDismantle_Implementation(byCharacter);
}
