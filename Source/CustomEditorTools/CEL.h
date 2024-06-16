// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Editor/UnrealEd/Public/AssetImportTask.h"
#include "AssetToolsModule.h"
#include "CEL.generated.h"


class IDesktopPlatform;
class FDesktopPlatformModule;
class UAssetImportTask;
class UFactory;
class UStaticMesh;
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

	UFUNCTION(BlueprintCallable, Category = "AssetsImporterLibrary")
	static UAssetImportTask* CreateImportTask(FString SourcePath, FString DestinationPath, UFactory* ExtraFactory, UObject* ExtraOptions, bool& bIsSucceed, FString& OutInfoMessage);

	UFUNCTION(BlueprintCallable, Category = "AssetsImporterLibrary")
	static UObject* ProcessImportTask(UAssetImportTask* ImportTask, bool& bIsSucceed, FString& OutInfoMessage);

	UFUNCTION(BlueprintCallable, Category = "AssetsImporterLibrary")
	static UObject* ImportAsset(FString SourcePath, FString DestinationPath, bool& bIsSucceed, FString& OutInfoMessage);

	UFUNCTION(BlueprintCallable, Category = "AssetsImporterLibrary")
	static UStaticMesh* ImportStaticMesh(FString SourcePath, FString DestinationPath, bool& bIsSucceed, FString& OutInfoMessage);


};
