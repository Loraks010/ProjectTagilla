// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Engine/DamageEvents.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Engine/World.h"
#include "BigDaddyDonut.generated.h"

UCLASS()
class FPSPROJECT_API ABigDaddyDonut : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABigDaddyDonut();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category="Parameters")
	float health;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* DonutMesh;

	/*UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent *geometryCollection;*/

	UPROPERTY(EditAnywhere, Category = "Parameters")
	float bigDaddyDamage;

	float getHealth();

	void setHealth(float health);

	bool isDead();

	virtual float TakeDamage
	(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void MoveForward(float Value);

	// Handles input for moving right and left.
	UFUNCTION()
	void MoveRight(float Value);

	// Sets jump flag when key is pressed.
	UFUNCTION()
	void StartJump();

	// Clears jump flag when key is released.
	UFUNCTION()
	void StopJump();
};
