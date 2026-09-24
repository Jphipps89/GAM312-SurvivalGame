// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//Updates the Health, Hunger, and Stamina bars in the player HUD.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateBars(float Health, float Hunger, float Stamina);
};
