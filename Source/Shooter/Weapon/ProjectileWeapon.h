#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess="true"))
	TSubclassOf<class AProjectile> ProjectileClass;

public:
	virtual void Fire(const FVector& HitTarget) override;
	
};
