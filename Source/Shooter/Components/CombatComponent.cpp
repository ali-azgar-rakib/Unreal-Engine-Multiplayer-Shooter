#include "CombatComponent.h"
#include "Shooter/Character/ShooterBase.h"
#include "Shooter/Weapon/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimationAsset.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


UCombatComponent::UCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();


	
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);



}

void UCombatComponent::TraceUnderCrosshair(FHitResult& OutHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f);

	FVector CrosshairWorldLocation, CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldLocation,
		CrosshairWorldDirection
	);

	if (bScreenToWorld) {

		FVector Start = CrosshairWorldLocation;
		FVector End = Start + (CrosshairWorldDirection * 10000.0f);

		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECC_Visibility);

	}

}

void UCombatComponent::Onrep_Weapon()
{
	if (Weapon) {
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bAiming)
{
	bIsAiming = bAiming;
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::SetAiming(bool bAiming)
{
	bIsAiming = bAiming;
	if (!Character->HasAuthority())
	{
		ServerSetAiming(bAiming);
	}
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}





void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UCombatComponent, Weapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!WeaponToEquip || !Character) return;

	Weapon = WeaponToEquip;
	Weapon->SeTWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* RightHandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket");
	if (RightHandSocket) {
		RightHandSocket->AttachActor(Weapon, Character->GetMesh());
	}
	Weapon->SetOwner(Character);

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

}

void UCombatComponent::FireWeapon(bool bFiredButton)
{
	if (Weapon == nullptr || Character == nullptr) return;
	bFireButtonPressed = bFiredButton;
	if (bFireButtonPressed) {
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		ServerFire(HitResult.ImpactPoint);
	}

}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& HitPoint)
{
	MulticastFire(HitPoint);
}


void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& HitPoint)
{
	if (Character && Weapon)
	{
		Character->PlayFireAnimMontage();
		Weapon->Fire(HitPoint);

	}
}


