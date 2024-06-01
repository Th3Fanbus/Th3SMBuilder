/* SPDX-License-Identifier: MPL-2.0 */

#include "StaticMeshHologram.h"
#include "Th3SMBuilder.h"

AStaticMeshHologram::AStaticMeshHologram() : AFGBuildableHologram()
{
	mNeedsValidFloor = false;
	mCanLockHologram = true;
	mCanNudgeHologram = true;
	mUseGradualFoundationRotations = true;
	mCanSnapWithAttachmentPoints = true;
}

void AStaticMeshHologram::BeginPlay()
{
	AFGBuildableHologram::BeginPlay();
}

void AStaticMeshHologram::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	AFGBuildableHologram::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool AStaticMeshHologram::IsValidHitResult(const FHitResult& hitResult) const
{
	return IsValid(hitResult.GetActor());
}

void AStaticMeshHologram::SetHologramLocationAndRotation(const FHitResult& hitResult)
{
	SetActorLocation(hitResult.ImpactPoint);

	FVector ImpactNormal = hitResult.ImpactNormal;
	ImpactNormal.Normalize();
	FQuat ImpactPointRotation = FQuat::FindBetweenNormals(FVector::UpVector, ImpactNormal);
	FQuat ScrollAdjustedRotation = ImpactPointRotation * FRotator(0, mScrollRotation - 90, 0).Quaternion();

	SetActorRotation(ScrollAdjustedRotation);
}

float AStaticMeshHologram::GetBuildGunRangeOverride_Implementation() const
{
	return 80000.0;
}
