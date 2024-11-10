// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ThirdPersonGAS/Public/Character/Abilities/CharacterAbilitySystemComponent.h"
#include "ThirdPersonGAS/Public/Character/Abilities/CharacterGameplayAbility.h"
#include "ThirdPersonGAS/Public/Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"

// Sets default values
ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	// 设置角色每帧调用Tick函数。若不需要可以禁用以提高性能
	PrimaryActorTick.bCanEverTick = true;

	// 设置角色胶囊体的碰撞响应，使其与可见性通道发生重叠（通常用于AI或射线检测）
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,
	                                                     ECollisionResponse::ECR_Overlap);

	// 确保角色始终在网络上进行同步
	bAlwaysRelevant = true;

	// 定义死亡状态和死亡时移除效果的Gameplay标签
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("State.RemoveOnDeath"));
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	// 获取能力系统组件（GAS支持）
	return AbilitySystemComponent.Get();
}

bool ACharacterBase::IsAlive() const
{
	// 根据生命值判断角色是否存活
	return GetHealth() > 0.0f;
}

int32 ACharacterBase::GetAbilityLevel(EGasAbilityID AbilityID) const
{
	// 当前返回默认等级，后续可扩展为获取实际等级
	return 1;
}

void ACharacterBase::RemoveCharacterAbilities()
{
	// 判断是否在服务端运行、能力系统组件是否有效、角色是否已赋予技能
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->
		CharacterAbilitiesGiven)
	{
		return;
	}

	// 收集要移除的技能句柄
	TArray<FGameplayAbilitySpecHandle> Handles;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		// 检查技能的源对象是否为当前角色，且技能类在角色技能列表中
		if (Spec.SourceObject == this && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			// 添加符合条件的技能句柄
			Handles.Add(Spec.Handle);
		}
	}

	// 移除所有收集到的技能句柄
	for (int32 i = 0; i < Handles.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(Handles[i]);
	}

	// 更新技能赋予标志
	AbilitySystemComponent->CharacterAbilitiesGiven = false;
}

void ACharacterBase::Die()
{
	// 移除角色技能，防止角色在死亡后继续使用技能
	RemoveCharacterAbilities();

	// 禁用胶囊体碰撞，并停止角色的所有移动
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->Velocity = FVector(0);

	// 触发角色死亡事件，通知所有绑定的监听者
	OnCharacterDied.Broadcast(this);

	// 若能力系统组件有效，执行取消技能和添加死亡状态标签等处理
	if (AbilitySystemComponent.IsValid())
	{
		// 取消所有激活的技能
		AbilitySystemComponent->CancelAbilities();

		// 移除带有指定标签的效果，以便清除死亡时无用的增益或状态
		FGameplayTagContainer EffectsTagsToRemove;
		EffectsTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsTagsToRemove);

		// 添加死亡状态标签
		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	// 播放死亡动画蒙太奇，如果未设置动画则直接结束死亡流程
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void ACharacterBase::FinishDying()
{
	Destroy();
}

float ACharacterBase::GetCharacterLevel() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetLevel();
	}

	return 0.0f;
}

float ACharacterBase::GetHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float ACharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float ACharacterBase::GetMana() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMana();
	}

	return 0.0f;
}

float ACharacterBase::GetMaxMana() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxMana();
	}

	return 0.0f;
}

void ACharacterBase::AddCharacterAbilities()
{
	// 判断是否在服务端运行、能力系统组件是否有效、角色是否已赋予技能
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->
		CharacterAbilitiesGiven)
	{
		return;
	}

	// 遍历角色的每个初始技能，将其添加到能力系统中
	for (TSubclassOf<UCharacterGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		// 使用技能ID和角色等级生成技能规范，添加到AbilitySystemComponent中
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID),
			                     static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityID), this));
	}

	// 更新技能赋予标志
	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}

void ACharacterBase::InitializeAttributes()
{
	// 判断基础属性集和默认属性效果是否有效
	if (!AttributeSetBase.IsValid())
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s 缺少 %s 的 DefaultAttributes，请在角色蓝图中填写。"), *FString(__FUNCTION__), *GetName());
		return;
	}

	// 创建效果上下文，将角色本身作为源对象
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	// 生成默认属性效果规范，并将其应用到角色身上
	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
		DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->
			ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void ACharacterBase::AddStartupEffects()
{
	// 判断是否在服务端运行、能力系统组件是否有效、角色是否已应用初始效果
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->
		CharacterAbilitiesGiven)
	{
		return;
	}

	// 创建效果上下文，将角色本身作为源对象
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	// 遍历角色的每个初始效果，将其应用到能力系统组件上
	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
			GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->
				ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	// 更新初始效果标志
	AbilitySystemComponent->StartupEffectsApplied = true;
}

void ACharacterBase::SetHealth(float Health)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetHealth(Health);
	}
}

void ACharacterBase::SetMana(float Mana)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetMana(Mana);
	}
}
