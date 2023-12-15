// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/CFOPlayerController.h"

#include "CFOPlayerStatsWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARFIGHT_ONLINE_API UCFOPlayerStatsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayerStats(const FPlayerStats& playerStats);
	const FPlayerStats& GetPlayerStats() const { return PlayerStatsList; }

	UFUNCTION(BlueprintImplementableEvent)
	void OnMatchWin();

	UFUNCTION(BlueprintImplementableEvent)
	void OnMatchLost();

	void OnGameStart(const FString& localPlayerName);
	bool IsReady() const { return m_isReady; }

protected:
	UPROPERTY(VisibleAnywhere, Getter = "GetPlayerStats", Setter = "SetPlayerStats", BlueprintReadOnly)
	FPlayerStats PlayerStatsList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString LocalPlayerName;
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePlayerStatsLabels();

private:
	bool m_isReady = false;
};
