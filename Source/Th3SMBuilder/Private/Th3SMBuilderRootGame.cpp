/* SPDX-License-Identifier: MPL-2.0 */

#include "Th3SMBuilderRootGame.h"
#include "Th3SMBuilderRootInstance.h"
#include "Th3SMBuilder.h"

#include "Module/GameInstanceModuleManager.h"
#include "Registry/ModContentRegistry.h"
#include "Resources/FGBuildingDescriptor.h"
#include "FGRecipe.h"
#include "FGRecipeManager.h"
#include "FGSchematicManager.h"
#include "Logging/LogMacros.h"
#include "Logging/StructuredLog.h"
#include "Algo/ForEach.h"

UTh3SMBuilderRootGame::UTh3SMBuilderRootGame()
{
	UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Hello Game World %s"), *this->GetPathName());
}

UTh3SMBuilderRootGame::~UTh3SMBuilderRootGame()
{
	UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Goodbye Cruel Game World"));
}

void UTh3SMBuilderRootGame::DispatchLifecycleEvent(ELifecyclePhase Phase)
{
	Super::DispatchLifecycleEvent(Phase);

	UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Dispatching Phase %s on %s"), *LifecyclePhaseToString(Phase), *this->GetPathName());

#if !0
	/* TODO: Unlocking the schematic seems to be really slow */
	if (Phase == ELifecyclePhase::POST_INITIALIZATION) {
		AFGSchematicManager* SchematicManager = AFGSchematicManager::Get(GetWorld());
		if (not SchematicManager) {
			UE_LOG(LogTh3SMBuilderCpp, Error, TEXT("Could not find schematic manager"));
			return;
		}
		UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Making Static Mesh recipes available..."));
		Algo::ForEach(mSchematics, [SchematicManager](const TSubclassOf<UFGSchematic>& Schematic) {
			SchematicManager->GiveAccessToSchematic(Schematic, nullptr);
		});
		UE_LOG(LogTh3SMBuilderCpp, Display, TEXT("Made Static Mesh recipes available"));
	}
#endif
}