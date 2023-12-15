// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CFOCharacter.generated.h"

class ACFOProjectile;

UCLASS()
class CARFIGHT_ONLINE_API ACFOCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACFOCharacter();

	void Shoot();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectiles)
	TObjectPtr<UStaticMeshComponent> ProjectileShooter;

	void BeginPlay() override;

private:
	UFUNCTION(Server, reliable)
	void ServerExecuteShoot(FVector location, FRotator rotation);

	UFUNCTION(Client, reliable)
	void ClientExecuteShoot(FVector location, FRotator rotation);

	UFUNCTION()
	void DamageReceived(AActor* DamagedActor,
						float Damage,
						const class UDamageType* DamageType,
						class AController* InstigatedBy,
						AActor* DamageCauser);

	TSubclassOf<ACFOProjectile> ProjectileClass;
};
