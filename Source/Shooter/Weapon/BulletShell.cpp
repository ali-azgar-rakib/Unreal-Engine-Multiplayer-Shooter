#include "BulletShell.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABulletShell::ABulletShell()
{

	PrimaryActorTick.bCanEverTick = false;

	BulletShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletShellMesh"));
	SetRootComponent(BulletShellMesh);
	BulletShellMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BulletShellMesh->SetSimulatePhysics(true);
	BulletShellMesh->SetEnableGravity(true);
	BulletShellMesh->SetNotifyRigidBodyCollision(true);

}


void ABulletShell::BeginPlay()
{
	Super::BeginPlay();
	BulletShellMesh->OnComponentHit.AddDynamic(this, &ABulletShell::OnHit);
	SetLifeSpan(2.f);
	BulletShellMesh->AddImpulse(GetActorForwardVector() * ShellImpulse);
	
}

void ABulletShell::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalizeAxis, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
}



