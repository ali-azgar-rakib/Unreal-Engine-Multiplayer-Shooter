// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGamemode.h"
#include "GameFramework/GameState.h"


void ALobbyGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 PlayerCount = GameState.Get()->PlayerArray.Num();

	if (PlayerCount >= 2) {

		UWorld* World = GetWorld();

		if (World) {
			bUseSeamlessTravel = true;
			World->ServerTravel(TEXT("/Game/Assets/LearningKit_Games/Maps/LearningKit_Games_Showcase?listen"));
		}
	}

}
