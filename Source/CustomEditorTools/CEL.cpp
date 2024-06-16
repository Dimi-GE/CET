// Fill out your copyright notice in the Description page of Project Settings.


#include "CEL.h"
#include "IDesktopPlatform.h"
#include "Misc/FileHelper.h"
#include "DesktopPlatformModule.h"

#include "Factories/FbxImportUI.h"
#include "Factories/FbxStaticMeshImportData.h"

FString UCEL::OpenFolderDialog(const FString& FileTypes)
{
    FString SelectedFile;

    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        void* ParentWindowHandle = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
        
        // Set the default path if needed
        FString DefaultPath = FPaths::ProjectContentDir();
        
        // Create an array to hold the selected files (only one in this case)
        TArray<FString> OutFiles;
        
        // Open the file dialog with specified filter
        bool bFileSelected = DesktopPlatform->OpenFileDialog(
            ParentWindowHandle,
            TEXT("Select a file"),
            DefaultPath,
            TEXT(""),
            FileTypes, 
            EFileDialogFlags::None,
            OutFiles
        );

        if (bFileSelected && OutFiles.Num() > 0)
        {
            // Get the selected file path
            SelectedFile = OutFiles[0];
            UE_LOG(LogTemp, Warning, TEXT("Selected File: %s"), *SelectedFile);
        }
    }

    return SelectedFile;
}

void UCEL::Renumerator(const FString Spiegel, bool& bIsSucceed)
{
    FString ExpectedHeader = TEXT("line_id\ttimestamp\taction\tparam\tcommand\tdescription\tbeats\t");

    if (Spiegel.IsEmpty())
    {
        bIsSucceed = false;
        return;
    }

    FString ModifiedSpiegel = "";
    TArray<FString> Lines;

    if (!FFileHelper::LoadFileToString(ModifiedSpiegel, *Spiegel))
    {
        bIsSucceed = false;
        return;
    }
    else
    {
        ModifiedSpiegel.ParseIntoArrayLines(Lines);
    }

    FString& FirstLine = Lines[0];

    if (FirstLine != ExpectedHeader)
    {
        // Replace the first line with the expected header
        Lines[0] = ExpectedHeader;
    }

    int Length = Lines.Num();
    
    // Counter for the line_id
    int32 LineIDCounter = 1;

    // Iterate through each line starting from LineIDCounter
    for (int32 i = LineIDCounter; i < Length; ++i)
    {
        FString& Line = Lines[i];

        // Find the position of the first tabulator ('\t')
        int32 TabIndex = Line.Find(TEXT("\t"));

        // If there is a tabulator, prepend the current index to the line
        if (TabIndex != INDEX_NONE)
        {
            Line = FString::FromInt(i) + Line.Mid(TabIndex);
        }
        else
        {
            // Otherwise, find the first numeric sequence at the beginning of the line
            int32 NumberLength = 0;
            while (FChar::IsDigit(Line[NumberLength]))
            {
                ++NumberLength;
            }

            // Replace the numeric sequence with LineIDCounter
            if (NumberLength > 0)
            {
                Line = FString::FromInt(LineIDCounter) + Line.RightChop(NumberLength);
                ++LineIDCounter;
            }
        }
    }

    // Reconstruct the file content
    ModifiedSpiegel = FString::Join(Lines, TEXT("\n"));

    // Save the modified content back to the file
    FFileHelper::SaveStringToFile(ModifiedSpiegel, *Spiegel);
}

UAssetImportTask *UCEL::CreateImportTask(FString SourcePath, FString DestinationPath, UFactory *ExtraFactory, UObject *ExtraOptions, bool &bIsSucceed, FString &OutInfoMessage)
{
    // Create Import Task object
    UAssetImportTask* TaskObj = NewObject<UAssetImportTask>();
    if (TaskObj == nullptr)
    {
        bIsSucceed = false;
        OutInfoMessage = FString::Printf(TEXT("UCEL::CreateImportTask: The TaskObj is a null pointer - returning."));
        return nullptr;
    }

    // Set path information
    TaskObj->Filename = SourcePath;
    TaskObj->DestinationPath = FPaths::GetPath(DestinationPath);
    TaskObj->DestinationName = FPaths::GetCleanFilename(DestinationPath);

    // Set default options
    TaskObj->bSave = false;
    TaskObj->bAutomated = true;
    TaskObj->bAsync = false;
    TaskObj->bReplaceExisting = true;
    TaskObj->bReplaceExistingSettings = false;

    // ExtraFactory and ExtraOptions
    TaskObj->Factory = ExtraFactory;
    TaskObj->Options = ExtraOptions;

    // Return the task object
    bIsSucceed = true;
    OutInfoMessage = FString::Printf(TEXT("UCEL::CreateImportTask: The TaskObj is successfully created."));
    return TaskObj;
}

UObject *UCEL::ProcessImportTask(UAssetImportTask *ImportTask, bool &bIsSucceed, FString &OutInfoMessage)
{
    // Ensuring the TaskObj is not a null pointer
    if (ImportTask == nullptr)
    {
        bIsSucceed = false;
        OutInfoMessage = FString::Printf(TEXT("UCEL::ProcessImportTask: The TaskObj is a null pointer argument - returning."));
        return nullptr;
    }

    // Get the Asset Tools Module
    FAssetToolsModule* AssetTools = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools");
    if (AssetTools == nullptr)
    {
        bIsSucceed = false;
        OutInfoMessage = FString::Printf(TEXT("UCEL::ProcessImportTask: The AssetTools module is a null pointer - returning."));
        return nullptr;
    }

    // Import the task
    AssetTools->Get().ImportAssetTasks({ ImportTask });
    if (ImportTask->GetObjects().Num() == 0)
    {
        bIsSucceed = false;
        OutInfoMessage = FString::Printf(TEXT("UCEL::ProcessImportTask: The ImportTask has nothing to import - returning."));
        return nullptr;
    }

    UObject* ImportedAsset = StaticLoadObject(UObject::StaticClass(), nullptr, *FPaths::Combine(ImportTask->DestinationPath, ImportTask->DestinationName));

    // Return the imported asset
    bIsSucceed = true;
    OutInfoMessage = FString::Printf(TEXT("UCEL::ProcessImportTask: The ImportedAsset is successfully created."));
    return ImportedAsset;
}

UObject *UCEL::ImportAsset(FString SourcePath, FString DestinationPath, bool &bIsSucceed, FString &OutInfoMessage)
{
    UAssetImportTask* Task = CreateImportTask(SourcePath, DestinationPath, nullptr, nullptr, bIsSucceed, OutInfoMessage);
    if(!bIsSucceed)
    {
        return nullptr;
    }

    UObject* Asset = ProcessImportTask(Task, bIsSucceed, OutInfoMessage);
    if(!bIsSucceed)
    {
        return nullptr;
    }

    return Asset;
}

UStaticMesh *UCEL::ImportStaticMesh(FString SourcePath, FString DestinationPath, bool &bIsSucceed, FString &OutInfoMessage)
{

    UFbxImportUI* Options = NewObject<UFbxImportUI>();
    // Default options for static mesh
    Options->bAutomatedImportShouldDetectType = false;
    Options->MeshTypeToImport = EFBXImportType::FBXIT_StaticMesh;
    Options->bImportMesh = true;
    
    // No skeletal mesh or animation
    Options->bImportAsSkeletal = false;
    Options->bImportAnimations = false;
    Options->bCreatePhysicsAsset = false;

    // More options
    Options->bImportTextures = true;
    Options->bImportMaterials = true;
    Options->bResetToFbxOnMaterialConflict = true;
    Options->LodNumber = 0;

    // UFbxAssetImportData
    Options->StaticMeshImportData->ImportTranslation = FVector(0.0f);
    Options->StaticMeshImportData->ImportRotation = FRotator(0.0f);
    Options->StaticMeshImportData->ImportUniformScale = 1.0f;
    Options->StaticMeshImportData->bConvertScene = true;
    Options->StaticMeshImportData->bForceFrontXAxis = true;
    Options->StaticMeshImportData->bConvertSceneUnit = true;

    // UFbxMeshImportData
    Options->StaticMeshImportData->bTransformVertexToAbsolute = false;
    Options->StaticMeshImportData->bBakePivotInVertex = false;
    Options->StaticMeshImportData->bImportMeshLODs = true;
    Options->StaticMeshImportData->NormalImportMethod = EFBXNormalImportMethod::FBXNIM_ComputeNormals;
    Options->StaticMeshImportData->NormalGenerationMethod = EFBXNormalGenerationMethod::BuiltIn;
    Options->StaticMeshImportData->bComputeWeightedNormals = true;
    Options->StaticMeshImportData->bReorderMaterialToFbxOrder = false;

    // UFbxStaticMeshImportData
    Options->StaticMeshImportData->StaticMeshLODGroup = FName();
    Options->StaticMeshImportData->VertexColorImportOption = EVertexColorImportOption::Replace;
    Options->StaticMeshImportData->bRemoveDegenerates = true;
    Options->StaticMeshImportData->bBuildReversedIndexBuffer = true;
    Options->StaticMeshImportData->bBuildNanite = true;
    Options->StaticMeshImportData->bGenerateLightmapUVs = true;
    Options->StaticMeshImportData->bOneConvexHullPerUCX = true;
    Options->StaticMeshImportData->bAutoGenerateCollision = true;
    Options->StaticMeshImportData->bCombineMeshes = true;
    Options->StaticMeshImportData->DistanceFieldResolutionScale = 0.0f;


    UAssetImportTask* Task = CreateImportTask(SourcePath, DestinationPath, nullptr, Options, bIsSucceed, OutInfoMessage);
    if(!bIsSucceed)
    {
        return nullptr;
    }

    UStaticMesh* Asset = Cast<UStaticMesh>(ProcessImportTask(Task, bIsSucceed, OutInfoMessage));
    if(!bIsSucceed)
    {
        return nullptr;
    }

    return Asset;
}
