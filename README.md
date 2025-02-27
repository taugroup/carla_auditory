This is a modified repo of the Carla Simulator, from the Jan 8th, 2025 version of the original Carla Repository https://github.com/carla-simulator/carla. This repository aims to add auditory systems to the Carla simulation enviornment for testing auditory perception in autonomous vehicles. 

In order to properly import our changed assets to include audio, perform the following steps after getting the Carla assets form the ./Update.sh command.

The TEMP-CONTENT folder contains the necessary files needed in order to implement audio systems in Unreal, however, they will have to be moved into the following locations after running ./Update.sh 

sounds folder -> carla-root/Unreal/CarlaUE4/Content/Carla 
Ambulance -> carla-root/Unreal/CarlaUE4/Content/Carla/Blueprints/Vehicles (replace or rename the existing ambulance folder)
followButton.uasset & follow_pawn.uasset -> carla-root/Unreal/CarlaUE4/Content/Carla/Game/ (place assets in folder)

Please refer to the documentation of the CARLA Simulator for more information (http://carla.readthedocs.io).

