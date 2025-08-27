#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletShell.generated.h"

UCLASS()
class SHOOTER_API ABulletShell : public AActor
{
	GENERATED_BODY()
	
public:	

	ABulletShell();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComponent,FVector NormalizeAxis,const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BulletShellMesh;

	UPROPERTY(EditAnywhere)
	float ShellImpulse{ 5.f };

	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;


};
