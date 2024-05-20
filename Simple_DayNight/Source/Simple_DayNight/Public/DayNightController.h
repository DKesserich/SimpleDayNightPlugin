//MIT License
//
//Copyright(c) 2024 David Lejeune
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayNightController.generated.h"

class UDirectionalLightComponent;
class UStaticMeshComponent;
class USceneComponent;
class UArrowComponent;
class USkyAtmosphereComponent;

UCLASS(Config=SimpleDayNight, DefaultConfig)
class SIMPLE_DAYNIGHT_API ADayNightController : public AActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ADayNightController(const FObjectInitializer& ObjectInitializer);

	USceneComponent* Root;

	UPROPERTY()
	USceneComponent* Axis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Category="Sun"))
	UDirectionalLightComponent* Sun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Category="Stars"))
	UStaticMeshComponent* SkySphere;

	UPROPERTY(EditAnywhere, meta=(Category="Atmosphere"))
	USkyAtmosphereComponent* Atmosphere;

	//Latitude of the level. Positive for Northern Hemisphere, Negative for Southern
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config)
	float Latitude = 34;

	//Tilt of the planet's axis off the ecliptic. Earth is 23.5 degrees.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config)
	float AxialTilt = 23.5;

	//Time of Day in hours. 24hr clock. Defaults to 6am on the day of the Winter Solstice
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeOfDay = 6.0f;

	//Number of real-time minutes for 24 hours in-game
	UPROPERTY(EditAnywhere, Config, meta=(ConsoleVariable="sdn.DayLength"))
	float LengthOfDay;

	//Number of in-game days a season lasts
	UPROPERTY(EditAnywhere, Config, meta=(ConsoleVariable="sdn.SeasonLength"))
	float SeasonLength;

	//How many seconds between sun position updates when using stepped sun position updates
	UPROPERTY(EditAnywhere, Config, meta=(ConsoleVariable="sdn.TimeStep"))
	float SteppedTimeRate;

	//Whether to use a Smooth sun position update (update sun position on every frame) or a stepped sun update.
	//(Due to VSM rebuilds Smooth update can result in a lower average framerate. Stepped update will have higher average framerate, but less consistent frame pacing)
	UPROPERTY(EditAnywhere, Config, meta=(ConsoleVariable="sdn.SmoothTime"))
	bool SmoothTime;

protected:
	FTimerHandle SteppedTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitProperties() override;

	void UpdateSunPosition();

	void UpdateSunPositionTimer();

	void UpdateStarPosition();

	UFUNCTION()
	void SmoothSinkFunction();
};