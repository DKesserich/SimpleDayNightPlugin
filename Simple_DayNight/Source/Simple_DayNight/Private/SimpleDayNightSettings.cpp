// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleDayNightSettings.h"


FName USimpleDayNightSettings::GetContainerName() const
{
	static const FName ContainerName("Project");
	return ContainerName;
}

FName USimpleDayNightSettings::GetCategoryName() const
{
	static const FName EditorCategoryName("Plugins");
	return EditorCategoryName;
}

FName USimpleDayNightSettings::GetSectionName() const
{
	static const FName EditorSectionName("Simple Day Night System");
	return EditorSectionName;
}

void USimpleDayNightSettings::PostInitProperties()
{
	Super::PostInitProperties();

#if WITH_EDITOR
	if (IsTemplate())
	{
		ImportConsoleVariableValues();
	}
#endif
}

#if WITH_EDITOR


void USimpleDayNightSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		ExportValuesToConsoleVariables(PropertyChangedEvent.Property);
	}
}

#endif