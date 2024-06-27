// Copyright (c) 2024 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla/Sensor/LIDARSensor.h"
#include "Carla.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include <algorithm>
#include <vector>
#include <span>

namespace crp = carla::rpc;



void RGLContext::AddStaticMesh(
	rgl_mesh_t& Destination,
	UStaticMesh* Source,
	uint32 LODIndex)
{
	check(IsValid(Source));
	auto RenderData = Source->GetRenderData();
	check(RenderData);
	check(LODIndex < RenderData->LODResources.Num());

	auto& LODResource = RenderData->LODResources[LODIndex];
	auto& VertexBuffers = LODResource.VertexBuffers;
	auto& PositionBuffer = VertexBuffers.PositionVertexBuffer;
	auto& IndexBuffer = LODResource.IndexBuffer;

	const auto IndexCount = IndexBuffer.GetNumIndices();
	const auto VertexCount = PositionBuffer.GetNumVertices();

	std::vector<rgl_vec3i> Indices(IndexCount);
	std::vector<rgl_vec3f> Vertices(VertexCount);

	for (size_t i = 0; i != IndexCount / 3; ++i)
		for (size_t j = 0; j != 3; ++j)
			Indices[i].value[j] = IndexBuffer.GetIndex(i * 3 + j);

	for (size_t i = 0; i != VertexCount; ++i)
	{
		auto Vertex = PositionBuffer.VertexPosition(i);
		for (size_t j = 0; j != 3; ++j)
			Vertices[i].value[j] = Vertex.XYZ[j];
	}
	
	auto Result = rgl_mesh_create(
		&Destination,
		Vertices.data(),
		Vertices.size(),
		Indices.data(),
		Indices.size());
}

void RGLContext::EnumerateStaticMeshes(UWorld* World)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(
		World,
		UStaticMesh::StaticClass(),
		Actors);
	for (auto& Actor : Actors)
	{

	}
}



FActorDefinition ALIDARSensor::GetSensorDefinition()
{
  return UActorBlueprintFunctionLibrary::MakeLidarDefinition(TEXT("lidar"));
}

ALIDARSensor::ALIDARSensor(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;
}

void ALIDARSensor::BeginPlay()
{
}

void ALIDARSensor::Set(const FActorDescription &ActorDescription)
{
  Super::Set(ActorDescription);
}

void ALIDARSensor::Set(const FLidarDescription &LidarDescription)
{
}

void ALIDARSensor::PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaTime)
{
}
