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

## Installation

1. Clone this repository:
   ```
   git clone https://github.com/taugroup/carla.git
   cd carla
   ```

2. Install the required Python packages:
   ```
   pip install -r requirements.txt
   ```

3. Import the modified vehicle assets into your CARLA installation:
   ```
   cd /path/to/carla
   ./ImportAssets.sh /path/to/downloaded/assets.tar.gz
   ```

## Speaker Configuration

Before running the simulation, you need to identify the correct speaker output:

1. Manually: Check each connected speaker to determine which one is used for simulation output
2. Automatically: Use the provided script to cycle through available speakers
   ```
   python find_speaker.py
   ```
3. Note the speaker ID for use in the simulation

## Usage

1. Start the CARLA server:
   ```
   ./CarlaUE4.sh -quality-level=Low
   ```

2. Run the auditory perception client:
   ```
   python auditory_client.py --speaker-id YOUR_SPEAKER_ID
   ```

3. The simulation will start with an ego vehicle that can detect emergency sirens and pull over when necessary.

## Data Generation

This repository can also be used to generate training data for more sophisticated audio classification models:

1. Enable recording in the client script:
   ```
   python auditory_client.py --speaker-id YOUR_SPEAKER_ID --record
   ```

2. Recorded audio clips will be saved in the `recordings` directory.

## Model Training

The SVM classification model was trained using:
- Emergency Vehicle Siren Sounds dataset by Vishnu
- Large-Scale Dataset for Emergency Vehicle Siren and Road Noises by Dr. Muhammad Asif
- Google's AudioSet (emergency vehicle category)

To train your own model, refer to the `train_model.py` script.

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

[MIT License](LICENSE)

## Contact

For questions or support, please open an issue on GitHub or contact the authors directly.

Citations:
[1] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/15879952/f0530029-e403-49f5-be70-10e86b303308/Carla_Paper.pdf

---
Answer from Perplexity: pplx.ai/share

