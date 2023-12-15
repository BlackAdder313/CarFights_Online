// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CFOProjectile.generated.h"

UCLASS()
class CARFIGHT_ONLINE_API ACFOProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACFOProjectile() = default;

	void BeginPlay() override;

	UFUNCTION()
	void OnProjectileHit(AActor* OverlappedActor, AActor* OtherActor);


private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Replicated)
	float DamageAmount = 1.f;
};