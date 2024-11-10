// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ThirdPersonGAS/ThirdPersonGAS.h"
#include "CharacterGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONGAS_API UCharacterGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCharacterGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGasAbilityID AbilityInputID = EGasAbilityID::None;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGasAbilityID AbilityID = EGasAbilityID::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
