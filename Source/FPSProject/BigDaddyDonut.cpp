// Fill out your copyright notice in the Description page of Project Settings.


#include "BigDaddyDonut.h"


// Sets default values
ABigDaddyDonut::ABigDaddyDonut()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Mesh
	DonutMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DonutMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BigDaddyDonutStaticMesh(TEXT("'/Game/DonutX7Boss.DonutX7Boss'"));
	if (BigDaddyDonutStaticMesh.Succeeded())
	{
		DonutMesh->SetStaticMesh(BigDaddyDonutStaticMesh.Object);
		DonutMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -215.0f));
	}
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("'/Game/SphereMaterial.SphereMaterial'"));
	if (Material.Succeeded())
	{
		DonutMesh->SetMaterial(0, Material.Object);
	}
	/*static ConstructorHelpers::FObjectFinder<UGeometryCollection> BigDaddyDonutGeometryCollection(TEXT("'/Game/Blueprint/BP_BigDaddyDonut_GeometryCollection.BP_BigDaddyDonut_GeometryCollection'"));
	if (BigDaddyDonutGeometryCollection.Succeeded())
	{
		
	}*/

	health = 100.0f;
	bigDaddyDamage = 10.0f;


	//Physics
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetSimulatePhysics(true);
	//GetCapsuleComponent()->SetCollisionObjectType(DonutMesh->GetCollisionObjectType());
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCapsuleHalfHeight(215);
	//GetCapsuleComponent()->SetCapsuleRadius(525);
	//DonutMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DonutMesh->OnComponentHit.AddDynamic(this, & ABigDaddyDonut::OnHit);
	DonutMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABigDaddyDonut::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABigDaddyDonut::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABigDaddyDonut::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABigDaddyDonut::getHealth()
{
	return this->health;
}

void ABigDaddyDonut::setHealth(float newHealth)
{
	this->health = newHealth;
}

bool ABigDaddyDonut::isDead()
{
	if (health <= 0)
	{
		return true;
	}
	return false;
}

float ABigDaddyDonut::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damageCaused = FMath::Min(Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser),health);
	health -= damageCaused;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, GetDebugName(this) + " damaged!");


	if (isDead())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, GetDebugName(this)+ " dead!");
		
		FVector newLocation = this->GetActorLocation();
		newLocation.SetComponentForAxis(EAxis::Z, this->GetActorLocation().Z + 1000.0f);


		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();


		GetWorld()->SpawnActor<ABigDaddyDonut>(this->GetClass(), newLocation, this->GetActorLocation().Rotation(), SpawnParams);

		
		Destroy();
	}
	return damageCaused;
}


// Function that is called when the BigDaddy hits something.
void ABigDaddyDonut::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{

	if (OtherActor != this)
	{
		OtherComponent->AddImpulseAtLocation(NormalImpulse * 100000.0f, Hit.ImpactPoint);

	}
	
	FPointDamageEvent pointDamageEvent(bigDaddyDamage, Hit, NormalImpulse, nullptr);
	OtherActor->TakeDamage(bigDaddyDamage, pointDamageEvent, GetInstigatorController(), this);


}


