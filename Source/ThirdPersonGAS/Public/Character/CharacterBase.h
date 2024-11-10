// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "ThirdPersonGAS/ThirdPersonGAS.h"
#include "CharacterBase.generated.h"

// 动态多播代理，用于在角色死亡时触发事件，将死亡的角色作为参数传递
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ACharacterBase*, Character);

UCLASS()
class THIRDPERSONGAS_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	// 获取角色的 AbilitySystemComponent，支持 GAS（Gameplay Ability System）
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 检查角色是否存活
	UFUNCTION(BlueprintCallable, Category="Gas|Character")
	virtual bool IsAlive() const;

	// 根据 AbilityID 获取角色的特定技能等级
	UFUNCTION(BlueprintCallable, Category="Gas|Character")
	virtual int32 GetAbilityLevel(EGasAbilityID AbilityID) const;

	// 移除角色的所有技能
	virtual void RemoveCharacterAbilities();

	// 执行角色死亡逻辑
	virtual void Die();

	// 结束角色死亡过程，完成清理等操作
	UFUNCTION(BlueprintCallable, Category="Gas|Character")
	virtual void FinishDying();

	// 获取角色的等级
	UFUNCTION(BlueprintCallable, Category="Gas|Character|Abilities")
	float GetCharacterLevel() const;

	// 获取角色的当前生命值
	UFUNCTION(BlueprintCallable, Category="Gas|Character|Abilities")
	float GetHealth() const;

	// 获取角色的最大生命值
	UFUNCTION(BlueprintCallable, Category="Gas|Character|Abilities")
	float GetMaxHealth() const;

	// 获取角色的当前法力值
	UFUNCTION(BlueprintCallable, Category="Gas|Character|Abilities")
	float GetMana() const;

	// 获取角色的最大法力值
	UFUNCTION(BlueprintCallable, Category="Gas|Character|Abilities")
	float GetMaxMana() const;

	// 角色死亡事件的代理，供蓝图使用
	UPROPERTY(BlueprintAssignable, Category="Gas|Character")
	FCharacterDiedDelegate OnCharacterDied;

protected:
	// 当游戏开始或角色生成时调用
	virtual void BeginPlay() override;

	// 为角色添加基础技能
	virtual void AddCharacterAbilities();

	// 初始化角色的基础属性
	virtual void InitializeAttributes();

	// 为角色添加初始效果（例如增益、状态效果）
	virtual void AddStartupEffects();

	// 设置角色的生命值
	virtual void SetHealth(float Health);

	// 设置角色的法力值
	virtual void SetMana(float Mana);

	// 角色的名称，显示在界面上
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gas|Character")
	FText CharacterName;

	// 角色死亡时播放的动画蒙太奇
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gas|Animation")
	UAnimMontage* DeathMontage;

	// 角色拥有的基础技能数组
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gas|Abilities")
	TArray<TSubclassOf<class UCharacterGameplayAbility>> CharacterAbilities;

	// 角色的默认属性设置（通常是初始生命值、法力值等）
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gas|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// 角色的初始效果（例如增益或状态效果）的数组
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gas|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	// 弱指针，指向角色的能力系统组件
	TWeakObjectPtr<class UCharacterAbilitySystemComponent> AbilitySystemComponent;

	// 弱指针，指向角色的属性集组件
	TWeakObjectPtr<class UCharacterAttributeSetBase> AttributeSetBase;

	// 标签，用于标识角色是否处于死亡状态
	FGameplayTag DeadTag;

	// 标签，用于标记在角色死亡时需要移除的效果
	FGameplayTag EffectRemoveOnDeathTag;
};
