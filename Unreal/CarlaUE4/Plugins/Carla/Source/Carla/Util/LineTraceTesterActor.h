// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LineTraceTesterActor.generated.h"


class FJsonObject;
class FJsonValue;

USTRUCT()
struct FLineTraceData
{
  GENERATED_BODY()

  UPROPERTY()
  FVector StartLocation = FVector::ZeroVector;

  UPROPERTY()
  FVector EndLocation = FVector::ZeroVector;

  UPROPERTY()
  FString ImpactedObjectName = "";
  
};



UCLASS(Blueprintable)
class CARLA_API ALineTraceTesterActor : public AActor
{
  GENERATED_BODY()
  
public:  
  ALineTraceTesterActor();
  
  UFUNCTION(BlueprintCallable, CallInEditor)
  void CastDebugLineTrace();

  // Cast line traces and save their info to a file. Path: Saved/LineTraceDataDebug/Out/TestSave.txt
  UFUNCTION(BlueprintCallable, CallInEditor)
  void CastRandomLineTraces();

  // Reads line traces saved data and cast them. Path: Saved/LineTraceDataDebug/In/TestSave.txt
  UFUNCTION(BlueprintCallable, CallInEditor)
  void ReproduceRandomLineTraces();

  virtual void Tick(float DeltaTime) override;


protected:
  virtual void BeginPlay() override;

  UPROPERTY()
  USceneComponent* SceneComp;

  UPROPERTY()
  USceneComponent* RotationSceneComp;

  UPROPERTY()
  UArrowComponent* ArrowDirComp;

  // Length of the line trace.
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
  float LineTraceLength = 10000.f;

  // Collision channel of the line trace.
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
  TEnumAsByte<ECollisionChannel> TraceCollisionChannel = ECollisionChannel::ECC_Visibility;

  // Draw duration of the line trace.
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config|DebugLineTrace")
  float DrawDuration = 2.f;

  // If launches line traces repeatedly every LineTraceInterval time.
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config|DebugLineTrace")
  bool bLoop = false;
  
  // How often line trace is done.
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config|DebugLineTrace")
  float LineTraceInterval = 3.f;

  // Draw duration of the line trace.
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config|CastRandomLineTraces")
  int NumLineTraces = 10000;

  // Max Start location of the LineTrace. Random location between MaxStartLocation and MinStartLocation.
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config|CastRandomLineTraces")
  FVector MaxStartLocation {-3300.f, 14800.f, 900.f};

  // Min Start location of the LineTrace. Random location between MaxStartLocation and MinStartLocation.
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config|CastRandomLineTraces")
  FVector MinStartLocation {-6300.f, 11800.f, 100.f};

  void GenerateRandomLineTraceData(TArray<FLineTraceData>& LineTraceDataArray);

  static void SaveTraceDataToFile(const TArray<FLineTraceData>& TraceData);

  static void LoadTraceDataFromFile(TArray<FLineTraceData>& TraceData);
  
private:
  static void RoundVectorToTwoDecimals(FVector& InOutVector);
  
  static FVector JsonObjectToVector(const TSharedPtr<FJsonObject>& JsonObject);

  static TSharedPtr<FJsonObject> VectorToJsonObject(const FVector& Vector);
  
  static TSharedPtr<FJsonObject> ConvertTraceDataToJson(const TArray<FLineTraceData>& TraceData);

  static void ConvertJsonToTraceData(const TSharedPtr<FJsonObject> JsonObject, TArray<FLineTraceData>& TraceData);
  
  static bool ReadStringFromFile(const FString& FilePath, FString& DataRead);
  
  static bool WriteStringFromFile(const FString& FilePath, const FString& DataToWrite);
  
  static bool ReadJson(const FString& JsonFilePath, TSharedPtr<FJsonObject>& JsonObject);

  static bool WriteJson(const FString& JsonFilePath, const TSharedPtr<FJsonObject> JsonObject);
};
