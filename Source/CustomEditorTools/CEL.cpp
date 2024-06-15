// Fill out your copyright notice in the Description page of Project Settings.


#include "CEL.h"
#include "IDesktopPlatform.h"
#include "Misc/FileHelper.h"
#include "DesktopPlatformModule.h"

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
    for (int32 i = LineIDCounter; i < Lines.Num(); ++i)
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