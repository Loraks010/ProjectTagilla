// Fill out your copyright notice in the Description page of Project Settings.


#include "BigDaddyDonut.h"
#include "FPSCharacte.h"


// Sets default values
ABigDaddyDonut::ABigDaddyDonut()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Mesh
	DonutMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DonutMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BigDaddyDonutSkeletalMesh(TEXT("'/Game/BigDaddyDonutSkeletalMesh.BigDaddyDonutSkeletalMesh'"));
	if (BigDaddyDonutSkeletalMesh.Succeeded())
	{
		DonutMesh->SetSkeletalMesh(BigDaddyDonutSkeletalMesh.Object);
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
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetCapsuleComponent()->SetSimulatePhysics(true);
	//GetCapsuleComponent()->SetCollisionObjectType(DonutMesh->GetCollisionObjectType());
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCapsuleHalfHeight(215);
	//GetCapsuleComponent()->SetCapsuleRadius(525);
	DonutMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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
	//UWorld::LineTraceSingleByChannel(nullptr,GetActorLocation(),GetActorForwardVector()*800+ GetActorLocation(), nullptr, nullptr, nullptr);
	MoveForward(1);
	MoveRight(2);
}

// Called to bind functionality to input
void ABigDaddyDonut::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAxis("MoveForward", this, &ABigDaddyDonut::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABigDaddyDonut::MoveRight);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &ABigDaddyDonut::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ABigDaddyDonut::AddControllerPitchInput);

	// Set up "action" bindings.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABigDaddyDonut::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ABigDaddyDonut::StopJump);

}

void ABigDaddyDonut::MoveForward(float Value)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void ABigDaddyDonut::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void ABigDaddyDonut::StartJump()
{
	bPressedJump = true;
}

void ABigDaddyDonut::StopJump()
{
	bPressedJump = false;
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
	AFPSCharacte* Character = Cast<AFPSCharacte>(OtherActor);
	if (Character)
	{

		FVector DonutLocation = this->GetActorLocation();
		FVector CharacterLocation = Character->GetActorLocation();
		FVector ImpulseVector = CharacterLocation - DonutLocation;

		ImpulseVector.Set(ImpulseVector.X, ImpulseVector.Y, ImpulseVector.Z + 500);
		Character->LaunchCharacter(ImpulseVector, false, false);
		FPointDamageEvent pointDamageEvent(bigDaddyDamage, Hit, NormalImpulse, nullptr);
		Character->TakeDamage(bigDaddyDamage, pointDamageEvent, GetInstigatorController(), this);
			
	}
	
	


}




