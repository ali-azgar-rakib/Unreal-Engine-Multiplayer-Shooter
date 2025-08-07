// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UCombatComponent();
	friend class AShooterBase;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(class AWeapon* WeaponToEquip);

protected:
	
	virtual void BeginPlay() override;

private:
	class AShooterBase* Character{ nullptr };

	UPROPERTY(ReplicatedUsing = Onrep_Weapon)
	class AWeapon* Weapon{ nullptr };

	UFUNCTION()
	void Onrep_Weapon();

	UPROPERTY(Replicated)
	bool bIsAiming{ false };

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bAiming);

public:	
	void SetAiming(bool bAiming);


		
};
