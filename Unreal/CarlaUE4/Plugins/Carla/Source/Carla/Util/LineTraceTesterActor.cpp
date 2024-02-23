// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.


#include "Util/LineTraceTesterActor.h"

#include "JsonObjectConverter.h"
#include "Components/ArrowComponent.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Serialization/JsonSerializer.h"

DEFINE_LOG_CATEGORY_STATIC(LogLineTraceTesterActor, Verbose, All);

ALineTraceTesterActor::ALineTraceTesterActor()
{
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = LineTraceInterval;

  SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
  RootComponent = SceneComp;

  RotationSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RotationSceneComp"));
  RotationSceneComp->SetupAttachment(SceneComp);

  ArrowDirComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
  ArrowDirComp->SetupAttachment(RotationSceneComp);
  ArrowDirComp->bHiddenInGame = false;
  ArrowDirComp->SetRelativeScale3D(FVector(2.f));
}

void ALineTraceTesterActor::BeginPlay()
{
  Super::BeginPlay();

  SetActorTickEnabled(bLoop);
  SetActorTickInterval(LineTraceInterval);
}

void ALineTraceTesterActor::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  CastDebugLineTrace();
}

void ALineTraceTesterActor::CastDebugLineTrace()
{
  const UWorld* World = GetWorld();
  if(!World)
  {
    return;
  }
  
  const FVector TraceStart = GetActorLocation();
  const FVector TraceEnd = TraceStart + RotationSceneComp->GetForwardVector() * LineTraceLength;
  FCollisionQueryParams CollisionQueryParams;
  CollisionQueryParams.AddIgnoredActor(this);

  FHitResult OutHit;
  const bool bResult = World->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, TraceCollisionChannel, CollisionQueryParams);

  if(bResult && OutHit.bBlockingHit)
  {
    UKismetSystemLibrary::DrawDebugLine(World, TraceStart, OutHit.ImpactPoint, FLinearColor::Yellow, DrawDuration, 4.f);

    AActor* HitActor = OutHit.GetActor();
    if(HitActor && GEngine)
    {
      GEngine->AddOnScreenDebugMessage(-1, DrawDuration, FColor::Cyan, FString::Printf(TEXT("ALineTraceTesterActor::CastDebugLineTrace HitActor: %s"), *HitActor->GetName()));
    }
  }
}

void ALineTraceTesterActor::CastRandomLineTraces()
{
  TRACE_CPUPROFILER_EVENT_SCOPE(ALineTraceTesterActor::CastRandomLineTraces);
  const UWorld* World = GetWorld();
  if(!World)
  {
    return;
  }

  TArray<FLineTraceData> LineTracesArray;
  LineTracesArray.Reserve(NumLineTraces);
  GenerateRandomLineTraceData(LineTracesArray);
  
  FCollisionQueryParams CollisionQueryParams;
  CollisionQueryParams.AddIgnoredActor(this);

  {
    TRACE_CPUPROFILER_EVENT_SCOPE(ALineTraceTesterActor::CastRandomLineTraces_Traces);
    for(FLineTraceData& LineTrace : LineTracesArray)
    {
      FHitResult OutHit;
      const bool bResult = World->LineTraceSingleByChannel(OutHit, LineTrace.StartLocation, LineTrace.EndLocation, TraceCollisionChannel, CollisionQueryParams);

      if(bResult && OutHit.bBlockingHit && OutHit.GetActor() && OutHit.GetActor()->GetClass())
      {
        LineTrace.ImpactedObjectName = OutHit.GetActor()->GetClass()->GetName();
      }
    }
  }

  SaveTraceDataToFile(LineTracesArray);
}

void ALineTraceTesterActor::ReproduceRandomLineTraces()
{
  TRACE_CPUPROFILER_EVENT_SCOPE(ALineTraceTesterActor::ReproduceRandomLineTraces);
  const UWorld* World = GetWorld();
  if(!World)
  {
    return;
  }

  TArray<FLineTraceData> LineTracesArray;
  LineTracesArray.Reserve(NumLineTraces);
  LoadTraceDataFromFile(LineTracesArray);

  FCollisionQueryParams CollisionQueryParams;
  CollisionQueryParams.AddIgnoredActor(this);

  {
    TRACE_CPUPROFILER_EVENT_SCOPE(ALineTraceTesterActor::ReproduceRandomLineTraces_Traces);
    for(FLineTraceData& LineTrace : LineTracesArray)
    {
      FHitResult OutHit;
      const bool bResult = World->LineTraceSingleByChannel(OutHit, LineTrace.StartLocation, LineTrace.EndLocation, TraceCollisionChannel, CollisionQueryParams);

      if(bResult && OutHit.bBlockingHit && OutHit.GetActor() && OutHit.GetActor()->GetClass())
      {
        LineTrace.ImpactedObjectName = OutHit.GetActor()->GetClass()->GetName();
      }
    }
  }

  SaveTraceDataToFile(LineTracesArray);
}

void ALineTraceTesterActor::GenerateRandomLineTraceData(TArray<FLineTraceData>& LineTraceDataArray)
{
  FLineTraceData LineTraceData;
  FVector TraceDirection;
  for(int i = 0; i < NumLineTraces; i++)
  {
    LineTraceData.StartLocation.X = FMath::FRandRange(MinStartLocation.X, MaxStartLocation.X);
    LineTraceData.StartLocation.Y = FMath::FRandRange(MinStartLocation.Y, MaxStartLocation.Y);
    LineTraceData.StartLocation.Z = FMath::FRandRange(MinStartLocation.Z, MaxStartLocation.Z);

    TraceDirection.X = FMath::FRandRange(-1.f, 1.f);
    TraceDirection.Y = FMath::FRandRange(-1.f, 1.f);
    TraceDirection.Z = FMath::FRandRange(-1.f, 1.f);

    LineTraceData.EndLocation = LineTraceData.StartLocation + TraceDirection * LineTraceLength;

    // Round to two decimals hack
    RoundVectorToTwoDecimals(LineTraceData.StartLocation);
    RoundVectorToTwoDecimals(LineTraceData.EndLocation);
    
    LineTraceDataArray.Add(LineTraceData);
  }
}

void ALineTraceTesterActor::SaveTraceDataToFile(const TArray<FLineTraceData>& TraceData)
{
  const TSharedPtr<FJsonObject> JsonObject = ConvertTraceDataToJson(TraceData);

  const FString FilePath =  FPaths::Combine(FPaths::ProjectSavedDir(), "LineTraceDataDebug", "TestSave_Out.txt");
  const bool bWriteSuccess = WriteJson(FilePath, JsonObject);
  UE_LOG(LogLineTraceTesterActor, Log, TEXT("ALineTraceTesterActor::SaveTraceDataToFile Write: %s"), bWriteSuccess ? TEXT("Success") : TEXT("Failure"));
}

void ALineTraceTesterActor::LoadTraceDataFromFile(TArray<FLineTraceData>& TraceData)
{
  const FString FilePath =  FPaths::Combine(FPaths::ProjectSavedDir(), "LineTraceDataDebug", "TestSave_In.txt");
  
  TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
  const bool bReadSuccess = ReadJson(FilePath, JsonObject);
  UE_LOG(LogLineTraceTesterActor, Log, TEXT("ALineTraceTesterActor::LoadTraceDataFromFile Read: %s, Valid Json: %s"), bReadSuccess ? TEXT("Success") : TEXT("Failure"),
    JsonObject.IsValid() ? TEXT("Yes") : TEXT("No"));

  if(!bReadSuccess || !JsonObject.IsValid())
  {
    return;
  }
  
  ConvertJsonToTraceData(JsonObject, TraceData);
}

void ALineTraceTesterActor::RoundVectorToTwoDecimals(FVector& InOutVector)
{
  InOutVector *= 100.f;
  InOutVector.X = FGenericPlatformMath::RoundToFloat(InOutVector.X);
  InOutVector.Y = FGenericPlatformMath::RoundToFloat(InOutVector.Y);
  InOutVector.Z = FGenericPlatformMath::RoundToFloat(InOutVector.Z);
  InOutVector /= 100.f;
}

TSharedPtr<FJsonObject> ALineTraceTesterActor::ConvertTraceDataToJson(const TArray<FLineTraceData>& TraceData)
{
  TSharedPtr<FJsonObject> FinalJsonValue = MakeShared<FJsonObject>();
  
  TArray<TSharedPtr<FJsonValue>> ArrayJsonValue;
  for (const auto TraceElement : TraceData)
  {
    const TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetObjectField(TEXT("StartLoc"), VectorToJsonObject(TraceElement.StartLocation));
    JsonObject->SetObjectField( TEXT("EndLoc"), VectorToJsonObject(TraceElement.EndLocation));
    JsonObject->SetStringField(TEXT("ImpactedObjectName"), TraceElement.ImpactedObjectName);

    TSharedRef<FJsonValue> PropJsonValue = MakeShareable(new FJsonValueObject(JsonObject));
    ArrayJsonValue.Add(PropJsonValue);
  }

  FinalJsonValue->SetArrayField(TEXT("LineTraceStructArray"), ArrayJsonValue);
  
  return FinalJsonValue;
}

void ALineTraceTesterActor::ConvertJsonToTraceData(const TSharedPtr<FJsonObject> JsonObject,
  TArray<FLineTraceData>& TraceData)
{
  TArray<TSharedPtr<FJsonValue>> ArrayValue = JsonObject->GetArrayField(TEXT("LineTraceStructArray"));

  FLineTraceData LineTraceData;
  for (const auto Element : ArrayValue)
  {
    if(!Element.IsValid())
    {
      continue;
    }

    const TSharedPtr<FJsonObject> ElementObject = Element->AsObject();
    TSharedPtr<FJsonObject> StartLocObj = ElementObject->GetObjectField(TEXT("StartLoc"));
    TSharedPtr<FJsonObject> EndLocObj = ElementObject->GetObjectField(TEXT("EndLoc"));
    
    LineTraceData.StartLocation  = JsonObjectToVector(StartLocObj);
    LineTraceData.EndLocation = JsonObjectToVector(EndLocObj);
    LineTraceData.ImpactedObjectName = ElementObject->GetStringField(TEXT("ImpactedObjectName"));
    
    TraceData.Add(LineTraceData);
  }
}

FVector ALineTraceTesterActor::JsonObjectToVector(const TSharedPtr<FJsonObject>& JsonObject)
{
  FVector Vector = FVector::ZeroVector;
  if(!JsonObject.IsValid())
  {
    return Vector;
  }

  const FString VectorStringX = JsonObject->GetStringField(TEXT("X"));
  const FString VectorStringY = JsonObject->GetStringField(TEXT("Y"));
  const FString VectorStringZ = JsonObject->GetStringField(TEXT("Z"));
  Vector = FVector{ FCString::Atof(*VectorStringX),FCString::Atof(*VectorStringY),FCString::Atof(*VectorStringZ) };
  
  /* Same result than the previous.
  float ValueParsedX, ValueParsedY, ValueParsedZ;
  FDefaultValueHelper::ParseFloat(JsonObject->GetStringField(TEXT("X")), ValueParsedX);
  Vector.X = ValueParsedX;
  FDefaultValueHelper::ParseFloat(JsonObject->GetStringField(TEXT("Y")), ValueParsedY);
  Vector.Y = ValueParsedY;
  FDefaultValueHelper::ParseFloat(JsonObject->GetStringField(TEXT("Z")), ValueParsedZ);
  Vector.Z = ValueParsedZ;*/
  
  RoundVectorToTwoDecimals(Vector);
  
  return Vector;
}

TSharedPtr<FJsonObject> ALineTraceTesterActor::VectorToJsonObject(const FVector& Vector)
{
  const TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
  JsonObject->SetStringField(TEXT("X"), FString::SanitizeFloat(Vector.X));
  JsonObject->SetStringField(TEXT("Y"), FString::SanitizeFloat(Vector.Y));
  JsonObject->SetStringField(TEXT("Z"), FString::SanitizeFloat(Vector.Z));
  
  return JsonObject;
}

bool ALineTraceTesterActor::ReadStringFromFile(const FString& FilePath, FString& DataRead)
{
  if(!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
  {
    DataRead = "";
    return false;
  }

  if(!FFileHelper::LoadFileToString(DataRead, *FilePath))
  {
    DataRead = "";
    return false;
  }

  return true;
}

bool ALineTraceTesterActor::WriteStringFromFile(const FString& FilePath, const FString& DataToWrite)
{
  if(!FFileHelper::SaveStringToFile(DataToWrite, *FilePath))
  {
    return false;
  }

  return true;
}

bool ALineTraceTesterActor::ReadJson(const FString& JsonFilePath, TSharedPtr<FJsonObject>& JsonObject)
{
  FString JsonString;
  const bool bReadSuccess = ALineTraceTesterActor::ReadStringFromFile(JsonFilePath, JsonString);
  if(!bReadSuccess)
  {
    JsonObject = nullptr;
    return false;
  }

  if(!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), JsonObject))
  {
    JsonObject = nullptr;
    return false;
  }
  
  return true;
}

bool ALineTraceTesterActor::WriteJson(const FString& JsonFilePath, const TSharedPtr<FJsonObject> JsonObject)
{
  if(!JsonObject.IsValid())
  {
    return false;
  }
  
  FString JsonString;
  if(!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JsonString)))
  {
    return false;
  }
  
  const bool bWriteSuccess = ALineTraceTesterActor::WriteStringFromFile(JsonFilePath, JsonString);
  if(!bWriteSuccess)
  {
    return false;
  }
  
  return true;
} 
 