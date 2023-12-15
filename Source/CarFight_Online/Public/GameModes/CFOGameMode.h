// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CFOGameMode.generated.h"

/**
 * 
 */

UCLASS()
class CARFIGHT_ONLINE_API ACFOGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ACFOGameMode();

public:
	void OnPlayerDefeat(APlayerController* player);
	void OnPlayerDamage(APlayerController* player);
	void OnPlayersListChange() const;

private:
	// AGameModeBase Overrides
	void PostLogin(APlayerController* newPlayer) override;

	TArray<class ACFOPlayerController*> PlayersList;
};
