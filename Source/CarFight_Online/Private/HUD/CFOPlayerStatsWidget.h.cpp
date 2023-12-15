// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CFOPlayerStatsWidget.h"

void UCFOPlayerStatsWidget::SetPlayerStats(const FPlayerStats& playerStats)
{
	PlayerStatsList = playerStats;
	if (m_isReady)
	{
		UpdatePlayerStatsLabels();
	}
}

void UCFOPlayerStatsWidget::OnGameStart(const FString& localPlayerName) {
	m_isReady = true;
	LocalPlayerName = localPlayerName;
	UpdatePlayerStatsLabels();
};

