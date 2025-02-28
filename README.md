# CARLA Auditory Perception System

## Overview

This repository contains a proof-of-concept implementation for integrating real-time audio classification into autonomous vehicle systems using the CARLA simulator. Our system enables simulated autonomous vehicles to detect emergency sirens in urban environments and respond appropriately by pulling over to the right shoulder.

## Features

- 3D positional audio support in CARLA using Unreal Engine's audio system
- Modified ambulance vehicles with realistic siren sounds, Doppler effect, and sound attenuation
- Real-time audio classification using Support Vector Machines (SVM)
- Autonomous pull-over behavior when emergency vehicles are detected
- Multithreaded implementation for efficient performance

## Requirements

- CARLA 0.9.15
- Modified Unreal Engine 4.26 fork (for development)
- Python 3.7+
- PyAudioAnalysis
- PyGame
- NumPy
- SciPy
- Ubuntu (tested on version 20.04)
- [Pulse Audio](https://www.freedesktop.org/wiki/Software/PulseAudio/) for Linux 

## Installation

For specific instructions on building carla please refer to https://carla.readthedocs.io/en/latest/build_carla/. 

For installing our contributions:

1. Move contents of TEMP-CONTENT into carla files:
   ```
   sounds folder -> carla-root/Unreal/CarlaUE4/Content/Carla 
   Ambulance -> carla-root/Unreal/CarlaUE4/Content/Carla/Blueprints/Vehicles (replace or rename the existing ambulance folder)
   followButton.uasset & follow_pawn.uasset -> carla-root/Unreal/CarlaUE4/Content/Carla/Game/ (place assets in folder)
   ```

2. Install the required Python packages:
   ```
   pip install -r carla-root/PythonAPI/examples/requirements-audio.txt
   (If issues arise with open3d regarding sklearn being deprecated, pip install scikit-learn is a sufficient fix)
   ```
   
## Usage

1. Add follow_pawn asset to scene:
   ```
   Drag and drop follow_pawn.uasset to unreal world that you wish
   to enable auditory perception in
   ```

2. Spawn Ego Vehicle in World:
   ```
   python automatic_control_siren.py
   ```
   
3. Enable Camera Follow
   ```
   In the Unreal Editor viewport click follow car button on bottom left of viewport
   ```

4. Spawn Ambulance with sound:
   ```
   python ambulance-spawn.py
   ```
## Potential Issue
- If carla cannot be found when running python scripts, ensure that you have pip installed carla's .whl under carla-root/PythonAPI/carla/dist/


## Future Improvements

- Add ambient noise to create more realistic soundscapes
- Improve classification accuracy through better algorithms and data preprocessing
- Enhance vehicle movement during pull-over maneuvers
- Distinguish between different types of emergency vehicles

## Citation

If you use this code in your research, please cite our paper:
```
@article{priest2025auditory,
  title={Auditory Perception In Open-Source Driving Simulator CARLA},
  author={Priest, Erik and Cassity, Alyssa and Kang, Nina and Tao, Jian},
  year={2025}
}
```

## License

- [MIT License](LICENSE)
- Ambulance Files under TEMP-CONTENT/Ambulance are distrubted under the CC-BY License, original production of assets contributed to Carla development team. 

## Contact

For questions or support, please open an issue on GitHub or contact the authors directly.
