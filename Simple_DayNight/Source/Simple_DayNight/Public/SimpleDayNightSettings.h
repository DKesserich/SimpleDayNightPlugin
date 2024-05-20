// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SimpleDayNightSettings.generated.h"

/**
 * 
 */
UCLASS(Config = SimpleDayNight, DefaultConfig)
class SIMPLE_DAYNIGHT_API USimpleDayNightSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(Config, EditAnywhere, Category = "Optimization", meta = (ConsoleVariable = "sdn.SmoothTime", DisplayName = "Use Smooth Sun Update", ToolTip="Disable to use a stepped update for the sun position. Can increase framerate due to lower frequency of VSM rebuilds"))
	bool SmoothTime = true;

	UPROPERTY(Config, EditAnywhere, Category = "Optimization", meta= (ConsoleVariable = "sdn.TimeStep", DisplayName = "Time Step for Stepped Update",  ToolTip="Sets the frequency of sun position updates when using stepped update"))
	float SteppedTimeRate = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category = "General", meta = (ConsoleVariable="sdn.DayLength", DisplayName = "Length of Day", ToolTip="How many real-time minutes equal 24 in-game hours"))
	float LengthOfDay = 10.0f;

	UPROPERTY(Config, EditAnywhere, Category = "General", meta=(ConsoleVariable="sdn.SeasonLength", DisplayName = "Length of Seasons", ToolTip="How many in-game days a season lasts"))
	float SeasonLength = 5.0f;
	
};
