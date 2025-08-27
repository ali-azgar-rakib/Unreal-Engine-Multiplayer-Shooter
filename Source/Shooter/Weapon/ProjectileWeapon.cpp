#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	if (!HasAuthority()) return;
	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	if (MuzzleSocket && ProjectileClass && OwnerPawn) {
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector MuzzleLocation = SocketTransform.GetLocation();
		FVector ToHitTarget = HitTarget - MuzzleLocation;
		FRotator MuzzleRotation = ToHitTarget.Rotation();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Instigator = OwnerPawn;
		SpawnParameters.Owner = GetOwner();
		GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass,
			MuzzleLocation,
			MuzzleRotation,
			SpawnParameters
		);

	}
}
