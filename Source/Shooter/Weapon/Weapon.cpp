
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Shooter/Character/ShooterBase.h"
#include "Net/UnrealNetwork.h"



AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	WeaponCollision = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponCollision"));
	WeaponCollision->SetupAttachment(RootComponent);
	WeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WeaponWidget"));
	WeaponWidget->SetupAttachment(RootComponent);

}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponCollision->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
		WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		WeaponCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}

	if (WeaponWidget) {
		WeaponWidget->SetVisibility(false);
	}

	
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		if (WeaponWidget) {
			WeaponWidget->SetVisibility(false);
		}
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
}

void AWeapon::SeTWeaponState(EWeaponState NewState)
{
	WeaponState = NewState;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		if (WeaponWidget) {
			WeaponWidget->SetVisibility(false);
		}
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
}

void AWeapon::Fire()
{

	if (FireAnimAsset) {
		WeaponMesh->PlayAnimation(FireAnimAsset, false);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterBase* ShooterCharacter = Cast<AShooterBase>(OtherActor);

	if (ShooterCharacter) {
		ShooterCharacter->SetOverlappingWeapon(this);
	}

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AShooterBase* ShooterCharacter = Cast<AShooterBase>(OtherActor);
	if (ShooterCharacter) {
		ShooterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::ShowWidget(bool bShowWidget)
{
	if (WeaponWidget) {
		WeaponWidget->SetVisibility(bShowWidget);
	}
}

