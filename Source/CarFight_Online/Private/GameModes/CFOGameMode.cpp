// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameModes/CFOGameMode.h"

#include "GameFramework/PlayerState.h"

#include "Player/CFOCharacter.h"
#include "Player/CFOPlayerController.h"

ACFOGameMode::ACFOGameMode()
{
	static ConstructorHelpers::FClassFinder<ACFOCharacter> PlayerPawnBPClass(TEXT("/Game/Blueprints/Player/BP_CarCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<ACFOPlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprints/Player/BP_PlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	PlayersList.Empty();
}

void ACFOGameMode::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);
	if (auto playerController = Cast<ACFOPlayerController>(newPlayer))
	{
		PlayersList.Push(playerController);
		OnPlayersListChange();
	}
}

void ACFOGameMode::OnPlayerDefeat(APlayerController* player)
{
	if (auto playerController = Cast<ACFOPlayerController>(player))
	{
		playerController->ClientOnMatchLost();
		PlayersList.Remove(playerController);
		
		if (PlayersList.Num() == 1)
		{
			PlayersList.Last()->ClientOnMatchWin();
			return;
		}

		OnPlayersListChange();
	}
}

void ACFOGameMode::OnPlayerDamage(APlayerController* player)
{
  	if (auto playerController = Cast<ACFOPlayerController>(player))
	{
		if (playerController->GetHealth() == 0)
		{
			OnPlayerDefeat(playerController);
			return;
		}
		
		auto pawnAboutToDie = player->GetPawn();
		player->UnPossess();
		pawnAboutToDie->Destroy();

		RestartPlayer(player);
 		
		OnPlayersListChange();
	}
}

void ACFOGameMode::OnPlayersListChange() const
{
	FPlayerStats playerStats;
	for (auto player : PlayersList)
	{
		playerStats.PlayerNames.Push(player->PlayerState->GetPlayerName());
		playerStats.PlayerHealth.Push(player->GetHealth());
	}

	for (auto player : PlayersList)
	{
		player->ClientOnPlayersUpdate(playerStats);
	}
}