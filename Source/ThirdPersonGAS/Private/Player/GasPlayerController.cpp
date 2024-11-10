// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GasPlayerController.h"

#include "AbilitySystemComponent.h"
#include "Player/GasPlayerState.h"

void AGasPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AGasPlayerState* PlayerState = GetPlayerState<AGasPlayerState>();
	if (PlayerState)
	{
		PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(PlayerState, InPawn);
	}
}
