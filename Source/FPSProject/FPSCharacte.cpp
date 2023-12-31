// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacte.h"

// Sets default values
AFPSCharacte::AFPSCharacte()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	health = 100.0f;

	// Create a first person camera component.
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FPSCameraComponent != nullptr);

	// Attach the camera component to our capsule component.
	FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));

	// Position the camera slightly above the eyes.
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));

	// Enable the pawn to control camera rotation.
	FPSCameraComponent->bUsePawnControlRotation = true;

	// Create a first person mesh component for the owning player.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FPSMesh != nullptr);

	// Only the owning player sees this mesh.
	FPSMesh->SetOnlyOwnerSee(true);

	// Attach the FPS mesh to the FPS camera.
	FPSMesh->SetupAttachment(FPSCameraComponent);

	// Disable some environmental shadows to preserve the illusion of having a single mesh.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);

	//GetCapsuleComponent()->SetSimulatePhysics(true);

}

// Called when the game starts or when spawned
void AFPSCharacte::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
	
}

// Called every frame
void AFPSCharacte::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSCharacte::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacte::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacte::MoveRight);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacte::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacte::AddControllerPitchInput);

	// Set up "action" bindings.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacte::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacte::StopJump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacte::Fire);

}

void AFPSCharacte::MoveForward(float Value)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFPSCharacte::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AFPSCharacte::StartJump()
{
	bPressedJump = true;
}

void AFPSCharacte::StopJump()
{
	bPressedJump = false;
}

void AFPSCharacte::Fire()
{
	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		// Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
}

float AFPSCharacte::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damageCaused = FMath::Min(Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser), health);
	health -= damageCaused;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, GetDebugName(this) + " damaged!");
	//Super::LaunchCharacter(,false,false);


	if (isDead())
	{
		GetMesh()->SetOwnerNoSee(false);
		GetMesh()->SetSimulatePhysics(true);
		try
		{
			DisableInput(GetWorld()->GetFirstPlayerController());
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, GetDebugName(this) + " IS DEAD!");
		}
		catch (...)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, " CLOWN!");
		}

		
	}

	return damageCaused;
}

float AFPSCharacte::getHealth()
{
	return this->health;
}

void AFPSCharacte::setHealth(float newHealth)
{
	this->health = newHealth;
}

bool AFPSCharacte::isDead()
{
	if (health <= 0)
	{
		return true;
	}
	return false;
}