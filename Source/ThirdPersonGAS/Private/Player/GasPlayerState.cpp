// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GasPlayerState.h"

#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"

AGasPlayerState::AGasPlayerState()
{
	// 创建并初始化能力系统组件，负责管理角色的技能和效果
	AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true); // 启用网络同步
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // 设置为混合的同步模式

	// 创建并初始化角色属性集，包含生命值、法力值、等级等属性
	AttributeSetBase = CreateDefaultSubobject<UCharacterAttributeSetBase>(TEXT("AttributeSetBase"));

	// 设置网络更新频率，控制数据同步的速度
	NetUpdateFrequency = 100.0f;

	// 获取并缓存“死亡”状态标签，用于判断角色的死亡状态
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent* AGasPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCharacterAttributeSetBase* AGasPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool AGasPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void AGasPlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
}

float AGasPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AGasPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AGasPlayerState::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float AGasPlayerState::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

int32 AGasPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetLevel();
}

void AGasPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// 注册生命值变化的委托，绑定到 HealthChanged 函数
		HealthChangedDelegateHandle = AbilitySystemComponent->
		                              GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).
		                              AddUObject(this, &AGasPlayerState::HealthChanged);

		// 注册最大生命值变化的委托，绑定到 MaxHealthChanged 函数
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->
		                                 GetGameplayAttributeValueChangeDelegate(
			                                 AttributeSetBase->GetMaxHealthAttribute()).AddUObject(
			                                 this, &AGasPlayerState::MaxHealthChanged);

		// 注册法力值变化的委托，绑定到 ManaChanged 函数
		ManaChangedDelegateHandle = AbilitySystemComponent->
		                            GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).
		                            AddUObject(this, &AGasPlayerState::ManaChanged);

		// 注册最大法力值变化的委托，绑定到 MaxManaChanged 函数
		MaxManaChangedDelegateHandle = AbilitySystemComponent->
		                               GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxManaAttribute()).
		                               AddUObject(this, &AGasPlayerState::MaxManaChanged);

		// 注册角色等级变化的委托，绑定到 CharacterLevelChanged 函数
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->
		                                      GetGameplayAttributeValueChangeDelegate(
			                                      AttributeSetBase->GetLevelAttribute()).AddUObject(
			                                      this, &AGasPlayerState::CharacterLevelChanged);

		// 注册“眩晕”状态标签事件，绑定到 StunTagChanged 函数
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))).
		                        AddUObject(this, &AGasPlayerState::StunTagChanged);
	}
}

void AGasPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Health Changed"));
}

void AGasPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Max Health Changed"));
}

void AGasPlayerState::ManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Mana Changed"));
}

void AGasPlayerState::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Max Mana Changed"));
}

void AGasPlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Character Level Changed"));
}

void AGasPlayerState::StunTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		// 定义需要被取消的技能标签容器
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		// 定义忽略取消的技能标签容器
		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NoCanceledByStun")));

		// 取消所有非忽略标签的技能
		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}
