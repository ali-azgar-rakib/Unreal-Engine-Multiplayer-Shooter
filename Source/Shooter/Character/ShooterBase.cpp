// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Weapon/Weapon.h"
#include "Shooter/Components/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

AShooterBase::AShooterBase()
{

	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.0f; 
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}



// Called when the game starts or when spawned
void AShooterBase::BeginPlay()
{
	Super::BeginPlay();
	
}
void AShooterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AShooterBase, OverlappingWeapon, COND_OwnerOnly);
}

void AShooterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComponent) {
		CombatComponent->Character = this;
	}
}


void AShooterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(EQuipAction, ETriggerEvent::Started, this, &ThisClass::EquipWeapon);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::CrouchEvent);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ThisClass::SetAiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ThisClass::ReleaseAiming);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ThisClass::FireButtonReleased);



		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterBase::Move);
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterBase::Look);
	}


}


void AShooterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);

}


void AShooterBase::Move(const FInputActionValue& Value)
{

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AShooterBase::Look(const FInputActionValue& Value)
{
	FVector2D LookAxis = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// Add controller yaw and pitch input
		AddControllerYawInput(LookAxis.X);
		AddControllerPitchInput(LookAxis.Y);
	}

}

void AShooterBase::EquipWeapon()
{
	if (CombatComponent && HasAuthority()) {
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
	else {
		ServerEquipWeapon();
	}

}

void AShooterBase::CrouchEvent()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AShooterBase::SetAiming()
{
	if (CombatComponent) {
		CombatComponent->SetAiming(true);
	}
}

void AShooterBase::ReleaseAiming()
{
	if (CombatComponent) {
		CombatComponent->SetAiming(false);
	}
}

void AShooterBase::FireButtonPressed()
{
	if (CombatComponent == nullptr || CombatComponent->Weapon == nullptr) return;
	CombatComponent->FireWeapon(true);
}

void AShooterBase::FireButtonReleased()
{
	if (CombatComponent == nullptr || CombatComponent->Weapon == nullptr) return;
	CombatComponent->FireWeapon(false);
}

void AShooterBase::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AShooterBase::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{

	if (OverlappingWeapon) {
		OverlappingWeapon->ShowWidget(true);
	}
	if (LastWeapon) {
		LastWeapon->ShowWidget(false);
	}
}

void AShooterBase::ServerEquipWeapon_Implementation()
{
	EquipWeapon();
}

void AShooterBase::AimOffset(float DeltaTime)
{
	if (CombatComponent && CombatComponent->Weapon == nullptr) return;

	float speed = GetVelocity().Size();
	bool bIsInAir = GetMovementComponent()->IsFalling();

	if (speed == 0.f && !bIsInAir) {
		bUseControllerRotationYaw = true;

		FRotator CurrentBaseAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentBaseAimRotation, StartingAimRotation);
		AO_Yaw = DeltaRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::None)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		
		TurningInPlaceHandler(DeltaTime);

	}

	if ( speed > 0.f || bIsInAir) {
		bUseControllerRotationYaw = true;
		StartingAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		AO_Yaw = 0.f;
		TurningInPlace = ETurningInPlace::None;

	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 180.f) {
		AO_Pitch -= 360.f;
	}
	else if (AO_Pitch < -180.f) {
		AO_Pitch += 360.f;
	}

}

void AShooterBase::TurningInPlaceHandler(float DeltaTime)
{
	if (AO_Yaw > 90.f) {
		TurningInPlace = ETurningInPlace::Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::Left;
	}

	if (TurningInPlace != ETurningInPlace::None) {
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f) {
			TurningInPlace = ETurningInPlace::None;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void AShooterBase::SetOverlappingWeapon(AWeapon* Weapon)
{

	if (OverlappingWeapon) {
		OverlappingWeapon->ShowWidget(false);
	}
	OverlappingWeapon = Weapon;

	if (OverlappingWeapon) {

		if (IsLocallyControlled()) {
			OverlappingWeapon->ShowWidget(true);
		}
	}
}

bool AShooterBase::IsWeaponEquipped() const
{

	return (CombatComponent && CombatComponent->Weapon);
}

bool AShooterBase::IsAiming() const
{
	return CombatComponent && CombatComponent->bIsAiming;
}

AWeapon* AShooterBase::GetWeapon()
{
	if(CombatComponent == nullptr) return nullptr;
	return CombatComponent->Weapon;
}

void AShooterBase::PlayFireAnimMontage()
{
	if (CombatComponent == nullptr || CombatComponent->Weapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && RifleFireAnimMontage)
	{
		AnimInstance->Montage_Play(RifleFireAnimMontage);
		FName SectionName = CombatComponent->bIsAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}




void AShooterBase::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	
	}
}



