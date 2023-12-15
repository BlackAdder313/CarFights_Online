// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CFOCharacter.h"

#include "Kismet/GameplayStatics.h"

#include "Player/CFOPlayerController.h"
#include "Player/CFOProjectile.h"

DECLARE_LOG_CATEGORY_EXTERN(ProjectileCategory, Log, All);

// Keep this code in mind

//if (MyActor && !ActorInfo->IsNetAuthority())
//{
//	ACharacter* MyCharacter = Cast<ACharacter>(MyActor);
//	if (MyCharacter)
//	{
//		UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(MyCharacter->GetMovementComponent());
//
//		if (CharMoveComp)
//		{
//			CharMoveComp->FlushServerMoves();
//		}
//	}
//}

ACFOCharacter::ACFOCharacter()
{
	static ConstructorHelpers::FClassFinder<ACFOProjectile> ProjectileBPClass(TEXT("/Game/Blueprints/Player/BP_Projectile"));
	if (ProjectileBPClass.Class != NULL)
	{
		ProjectileClass = ProjectileBPClass.Class;
	}

	SetCanBeDamaged(true);
}

void ACFOCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnTakeAnyDamage.AddDynamic(this, &ACFOCharacter::DamageReceived);
}

void ACFOCharacter::DamageReceived(AActor* DamagedActor,
								   float Damage,
								   const class UDamageType* DamageType,
								   class AController* InstigatedBy,
								   AActor* DamageCauser)
{
 	Cast<ACFOPlayerController>(GetController())->OnHit(this, Damage);
}

void ACFOCharacter::Shoot()
{
	if (!HasAuthority())
	{
		ServerExecuteShoot(ProjectileShooter.Get()->GetComponentLocation(),
						   ProjectileShooter.Get()->GetComponentRotation());
		return;
	}
	
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = this;
	GetWorld()->SpawnActor<ACFOProjectile>(ProjectileClass,
										   ProjectileShooter.Get()->GetComponentLocation(),
										   ProjectileShooter.Get()->GetComponentRotation(),
										   spawnParameters);
}


void ACFOCharacter::ServerExecuteShoot_Implementation(FVector location,
													  FRotator rotation)
{
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = this;
	GWorld->SpawnActor<ACFOProjectile>(ProjectileClass, location, rotation, spawnParameters);
	ClientExecuteShoot(location, rotation);
}

void ACFOCharacter::ClientExecuteShoot_Implementation(FVector location,
													  FRotator rotation)
{
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = this;
	GWorld->SpawnActor<ACFOProjectile>(ProjectileClass,
									   location,
									   rotation,
									   spawnParameters);
}