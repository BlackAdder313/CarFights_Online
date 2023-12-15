// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CFOPlayerController.generated.h"

struct FInputActionValue;

USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FString> PlayerNames;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<float> PlayerHealth;
};

UCLASS()
class CARFIGHT_ONLINE_API ACFOPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	float GetHealth() const { return Health; }

	UFUNCTION(Client, reliable)
	void ClientOnMatchWin();
	
	UFUNCTION(Client, reliable)
	void ClientOnMatchLost();

	UFUNCTION(Server, reliable)
	void ClientDestroyPlayer();

	UFUNCTION(Server, reliable)
	void ServerDestroyPlayer();

	UFUNCTION(BlueprintCallable)
	void OnPlayerFall();
	
	void OnHit(class ACFOCharacter* playerCharacter, float damage);
	
	// Note: This should be better in a custom GameState class
	// Placed here for time efficiency
	UFUNCTION(Client, reliable)
	void ClientOnPlayersUpdate(const FPlayerStats& playerStats) const;

protected:
	//AActor overrides
	void BeginPlay() override;	
	void SetupInputComponent() override;
	void PlayerTick(float deltaTime) override;

	// Enhanced input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateCameraAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction = nullptr;

private:
	void Move(const FInputActionValue& value);
	
	UFUNCTION(Server, reliable)
	void ServerExecuteMove(ACFOPlayerController* controller, const FVector& destination);

	UFUNCTION(Server, reliable)
	void ServerUpdatePlayerStats();
	
	UFUNCTION(Client, reliable)
	void ClientExecuteMove(ACFOPlayerController* controller, const FVector& destination);

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY(EditAnywhere, Replicated)
	float MaxHealth = 5.f;

	UPROPERTY(EditAnywhere, Getter="GetHealth", Replicated, ReplicatedUsing = "OnRep_Health")
	float Health = 0.f;

	void RotateCamera(const FInputActionValue& value);
	void Shoot(const FInputActionValue& value);
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
	// Note: Everything below is essentially a dirty way to coordinate UI
	// instead of putting it to a custom AHUD class.
	// Again, shortcut taken for time efficiency

	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<class UCFOPlayerStatsWidget> playerHUD;

	void SetupPlayerHUD();
	bool ValidateGameStateReference();

	TObjectPtr<class ACFOGameMode> m_gameModeReference;
};
