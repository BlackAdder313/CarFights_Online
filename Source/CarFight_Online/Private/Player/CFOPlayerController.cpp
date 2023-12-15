// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CFOPlayerController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#include "GameModes/CFOGameMode.h"
#include "HUD/CFOPlayerStatsWidget.h"
#include "Player/CFOCharacter.h"

void ACFOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Setup up mapping context for EnhancedInputSystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}

	Health = MaxHealth;
	
	SetInputMode(FInputModeGameAndUI());
	SetShowMouseCursor(true);

	if (HasAuthority())
	{
		m_gameModeReference = Cast<ACFOGameMode>(UGameplayStatics::GetGameMode(GWorld));
	}

	SetupPlayerHUD();
}

void ACFOPlayerController::SetupPlayerHUD()
{
	// If PlayerState hasn't been replicated on client, try again in 1sec
	if (!PlayerState)
	{
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &ACFOPlayerController::SetupPlayerHUD, 1, false);
		return;
	}

	if (PlayerState && playerHUD)
	{
		auto playerName = PlayerState->GetPlayerName();
		if (!playerName.IsEmpty())
		{
			playerHUD->AddToViewport();
			playerHUD->OnGameStart(playerName);
			ServerUpdatePlayerStats();
		}
	}
}

void ACFOPlayerController::ServerUpdatePlayerStats_Implementation()
{
	if (!ValidateGameStateReference())
	{
		return;
	}
	
	m_gameModeReference->OnPlayersListChange();
}

void ACFOPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Move with mouse click
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &ACFOPlayerController::Move);

		// Looking
		EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this, &ACFOPlayerController::RotateCamera);

		// Looking
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ACFOPlayerController::Shoot);
	}
}

void ACFOPlayerController::PlayerTick(float deltaTime)
{
	Super::PlayerTick(deltaTime);
}

void ACFOPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (ACFOCharacter* currentPawn = Cast<ACFOCharacter>(GetPawn()))
	{
		FVector worldPosition, worldDirection;
		DeprojectMousePositionToWorld(worldPosition, worldDirection);
		
		const FVector traceStart = worldPosition;
		const FVector traceEnd = traceStart + 10000 * worldDirection;

		FCollisionQueryParams traceParams(TEXT("MovePlayerTrace"), true, this);
		FHitResult hitResult;
		GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_WorldStatic, traceParams);
		if (hitResult.bBlockingHit)
		{
			if (!HasAuthority())
			{
				ServerExecuteMove(this, hitResult.Location);
				return;
			}
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, hitResult.Location);
		}
	}
}

void ACFOPlayerController::ServerExecuteMove_Implementation(ACFOPlayerController* controller, const FVector& destination)
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, destination);
	ClientExecuteMove(controller, destination);
}

void ACFOPlayerController::ClientExecuteMove_Implementation(ACFOPlayerController* controller, const FVector& destination)
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, destination);
}

void ACFOPlayerController::RotateCamera(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// add yaw and pitch input to controller
	AddYawInput(-LookAxisVector.X);
}

void ACFOPlayerController::Shoot(const FInputActionValue& Value)
{
	if (ACFOCharacter* currentPawn = Cast<ACFOCharacter>(GetPawn()))
	{
		currentPawn->Shoot();
	}
}

void ACFOPlayerController::OnHit(ACFOCharacter* playerCharacter, float damage)
{
	if (GetPawn() != playerCharacter)
	{
		return;
	}

 	Health -= damage;

	// OnRep_Health isn't called on server
	if(HasAuthority())
	{
		if (!ValidateGameStateReference())
		{
			return;
		}

		m_gameModeReference->OnPlayerDamage(this);
	}
}

void ACFOPlayerController::OnPlayerFall()
{
	Health = 0;

	// OnRep_Health isn't called on server
	if (HasAuthority())
	{
		if (!ValidateGameStateReference())
		{
			return;
		}

		m_gameModeReference->OnPlayerDamage(this);
	}
}


void ACFOPlayerController::ClientOnMatchLost_Implementation()
{
	SetInputMode(FInputModeUIOnly());
	
	if (playerHUD)
	{
		playerHUD->OnMatchLost();
	}
	
	ServerDestroyPlayer();
}

void ACFOPlayerController::ServerDestroyPlayer_Implementation()
{
	SetActorHiddenInGame(true);
	GetPawn()->SetActorHiddenInGame(true);
	
	ClientDestroyPlayer();
}

void ACFOPlayerController::ClientDestroyPlayer_Implementation()
{
	SetActorHiddenInGame(true);
	GetPawn()->SetActorHiddenInGame(true);
}

void ACFOPlayerController::ClientOnMatchWin_Implementation()
{
	SetInputMode(FInputModeUIOnly());

	if (playerHUD)
	{
		playerHUD->OnMatchWin();
	}
}

void ACFOPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACFOPlayerController, MaxHealth, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ACFOPlayerController, Health, COND_None);
}


void ACFOPlayerController::ClientOnPlayersUpdate_Implementation(const FPlayerStats& playerStats) const
{
	if (playerHUD && playerHUD->IsReady())
	{
		playerHUD->SetPlayerStats(playerStats);
	}
}

void ACFOPlayerController::OnRep_Health()
{
	if (!ValidateGameStateReference())
	{
		return;
	}

	m_gameModeReference->OnPlayerDamage(this);
}

bool ACFOPlayerController::ValidateGameStateReference()
{
	if (!m_gameModeReference)
	{
		m_gameModeReference = Cast<ACFOGameMode>(UGameplayStatics::GetGameMode(GWorld));
		if (!m_gameModeReference)
		{
			return false;
		}
	}

	return true;
}