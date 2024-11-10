// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/CharacterAbilitySystemComponent.h"

void UCharacterAbilitySystemComponent::ReceiveDamage(UCharacterAbilitySystemComponent* SourceASC,
	float UnmitigatedDamage, float MitigateDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigateDamage);
}
