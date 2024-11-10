// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GasPlayerState.generated.h"

/**
 * AGasPlayerState
 * 此类表示游戏中玩家的状态，包括生命值、法力值和等级等属性。
 * 它实现了 IAbilitySystemInterface 接口，以便与虚幻引擎的能力系统集成。
 */
UCLASS()
class THIRDPERSONGAS_API AGasPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// 构造函数，用于初始化该类的默认值
	AGasPlayerState();

	// 重写函数，返回玩家的能力系统组件，该组件管理玩家的技能和效果
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 返回角色的属性集指针，包含生命值、法力值等属性
	class UCharacterAttributeSetBase* GetAttributeSetBase() const;

	// 判断角色是否存活，基于生命值属性
	UFUNCTION(BlueprintCallable, Category = "Gas|GasPlayerState")
	bool IsAlive() const;

	// 控制是否显示技能确认/取消的文本（UI用途）
	UFUNCTION(BlueprintCallable, Category = "Gas|GasPlayerState|UI")
	void ShowAbilityConfirmCancelText(bool ShowText);

	// 获取当前的生命值
	UFUNCTION(BlueprintCallable, Category = "Gas|GasPlayerState|Attributes")
	float GetHealth() const;

	// 获取最大生命值
	UFUNCTION(BlueprintCallable, Category = "Gas|GasPlayerState|Attributes")
	float GetMaxHealth() const;

	// 获取当前的法力值
	UFUNCTION(BlueprintCallable, Category = "Gas|GasPlayerState|Attributes")
	float GetMana() const;

	// 获取最大法力值
	UFUNCTION(BlueprintCallable, Category = "Gas|GasPlayerState|Attributes")
	float GetMaxMana() const;

	// 获取角色的等级
	UFUNCTION(BlueprintCallable, Category = "Gas|GasPlayerState|Attributes")
	int32 GetCharacterLevel() const;

protected:
	// 游戏开始时调用，用于初始化
	virtual void BeginPlay() override;

	// 生命值发生变化时的回调函数
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	// 最大生命值发生变化时的回调函数
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);

	// 法力值发生变化时的回调函数
	virtual void ManaChanged(const FOnAttributeChangeData& Data);

	// 最大法力值发生变化时的回调函数
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);

	// 角色等级发生变化时的回调函数
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

	// 当角色的“眩晕”状态标签发生变化时的回调函数
	virtual void StunTagChanged(const FGameplayTag Tag, int32 NewCount);

	// 玩家状态的能力系统组件
	UPROPERTY()
	class UCharacterAbilitySystemComponent* AbilitySystemComponent;

	// 玩家属性集，包含角色的各类属性
	UPROPERTY()
	class UCharacterAttributeSetBase* AttributeSetBase;

	// 角色的“死亡”标签，用于标识角色是否处于死亡状态
	FGameplayTag DeadTag;

	// 属性变化的代理句柄，用于处理生命值等属性的变化事件
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;
};
