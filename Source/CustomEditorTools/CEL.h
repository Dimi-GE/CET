// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CEL.generated.h"


class IDesktopPlatform;
class FDesktopPlatformModule;
/**
 * 
 */
UCLASS()
class CUSTOMEDITORTOOLS_API UCEL : public UObject
{
	GENERATED_BODY()


public:

    UFUNCTION(BlueprintCallable, Category = "SpiegelEditorLibrary")
    static FString OpenFolderDialog(const FString& FileTypes);

	UFUNCTION(BlueprintCallable, Category = "SpiegelEditorLibrary")
	static void Renumerator(const FString Spiegel, bool& bIsSucceed);
};
