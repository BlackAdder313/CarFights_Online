// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CFOProjectile.h"

#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

#include "Player/CFOCharacter.h"

DECLARE_LOG_CATEGORY_EXTERN(ProjectileCategory, Log, All);
DEFINE_LOG_CATEGORY(ProjectileCategory);

void ACFOProjectile::BeginPlay()
{
	Super::BeginPlay();
 	
	/*if (GetOwner() && GetOwner()->HasAuthority())
	{*/
		OnActorBeginOverlap.AddDynamic(this, &ACFOProjectile::OnProjectileHit);
	//}
}


void ACFOProjectile::OnProjectileHit(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor == this)
	{
		return;
	}

	if (auto otherPlayer = Cast<ACFOCharacter>(OtherActor))
	{
		if (otherPlayer == GetOwner() || !otherPlayer->GetController())
		{
			return;
		}
		
   		FPointDamageEvent damageEvent(DamageAmount, FHitResult(), FVector(), UDamageType::StaticClass());
		otherPlayer->TakeDamage(DamageAmount, damageEvent, otherPlayer->GetController(), this);
		OnActorBeginOverlap.RemoveAll(this);
		Destroy();
	}
}

void ACFOProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACFOProjectile, DamageAmount, COND_InitialOnly);
}