/* SPDX-License-Identifier: MPL-2.0 */

#pragma once

#include "CoreMinimal.h"
#include "Module/GameInstanceModule.h"
#include "Module/GameWorldModule.h"

#include "Th3SMBuilderRootGame.generated.h"

UCLASS(Abstract)
class TH3SMBUILDER_API UTh3SMBuilderRootGame : public UGameWorldModule {
	GENERATED_BODY()
public:
	UTh3SMBuilderRootGame();
	~UTh3SMBuilderRootGame();
	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;
};
