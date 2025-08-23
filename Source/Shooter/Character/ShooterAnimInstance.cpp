// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shooter/Weapon/Weapon.h"

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
	bIsAiming = ShooterCharacter->IsAiming();
	Weapon = ShooterCharacter->GetWeapon();
	TurningInPlace = ShooterCharacter->GetTurningInPlace();

	FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, MovementRotation);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float TargetLean = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(lean, TargetLean, DeltaSeconds, 6.0f);
	lean = FMath::Clamp(Interp, -90.0f, 90.0f);

	AO_Yaw = ShooterCharacter->GetAO_Yaw();
	AO_Pitch = ShooterCharacter->GetAO_Pitch();

	if (bIsEquippedWeapon && Weapon && Weapon->GetWeaponMesh() && ShooterCharacter->GetMesh()) {
		LeftHandTransform = Weapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;

		ShooterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"),LeftHandTransform.GetLocation(),FRotator::ZeroRotator,OutPosition,OutRotation );
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
	}
}
