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

	void FireWeapon(bool bFiredButton);

protected:
	
	virtual void BeginPlay() override;

	void TraceUnderCrosshair(FHitResult& OutHitResult);

	void SetCrosshair(float DeltaTime);

private:
	class AShooterBase* Character{ nullptr };
	class AShooterPlayerController* Controller{ nullptr };

	UPROPERTY(ReplicatedUsing = Onrep_Weapon)
	class AWeapon* Weapon{ nullptr };

	UFUNCTION()
	void Onrep_Weapon();

	UPROPERTY(Replicated)
	bool bIsAiming{ false };

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bAiming);

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AimWalkSpeed{ 300.0f };

	UPROPERTY(EditAnywhere, Category = "Movement")
	float BaseWalkSpeed{ 600.0f };

	bool bFireButtonPressed{ false };

	float CrosshairSpreadVelocityFactor{ 0.f };
	float CrosshairSpreadJumpingFactor{ 0.f };


public:	
	void SetAiming(bool bAiming);

	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& HitPoint);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& HitPoint);

};
