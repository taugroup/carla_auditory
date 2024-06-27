// Copyright (c) 2024 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.



#pragma once
#include "Carla/Sensor/Sensor.h"
#include "Carla/Sensor/LidarDescription.h"

#include <compiler/disable-ue4-macros.h>
#include "rgl/api/core.h"
#include <compiler/enable-ue4-macros.h>

#include "LIDARSensor.generated.h"



struct RGLContext
{
	std::vector<rgl_mesh_t> meshes;

	void AddStaticMesh(
		rgl_mesh_t& Destination,
		UStaticMesh* Source,
		uint32 LODIndex);
	void EnumerateStaticMeshes(UWorld* World);
};



UCLASS()
class CARLA_API ALIDARSensor : public ASensor
{
  GENERATED_BODY()

protected:

  using FSemanticLidarData = carla::sensor::data::SemanticLidarData;
  using FSemanticDetection = carla::sensor::data::SemanticLidarDetection;

public:

  static FActorDefinition GetSensorDefinition();

  ALIDARSensor(const FObjectInitializer &ObjectInitializer);

  virtual void BeginPlay() override;
  virtual void Set(const FActorDescription &Description) override;
  virtual void Set(const FLidarDescription &LidarDescription);

protected:

  virtual void PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaTime) override;

private:

	std::unique_ptr<RGLContext> Context;

};
