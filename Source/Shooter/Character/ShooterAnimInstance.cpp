// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterBase>(TryGetPawnOwner());
	}
}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterBase>(TryGetPawnOwner());
	}

	if (ShooterCharacter == nullptr)
	{
		return;
	}

	Speed = ShooterCharacter->GetVelocity().Size();
	bIsInAir = ShooterCharacter->GetMovementComponent()->IsFalling();
	bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f;

	bIsEquippedWeapon = ShooterCharacter->IsWeaponEquipped();
	bIsCrouched = ShooterCharacter->bIsCrouched;
}
