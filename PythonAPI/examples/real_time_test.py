from sys import byteorder
from array import array
from struct import pack
import numpy as np

import pyaudio
import wave
import time

import matplotlib.pyplot as plt
from scipy.io import wavfile
from pydub import AudioSegment

from pyAudioAnalysis import audioTrainTest as aT

'''
note: model used is based on code from module pyAudioAnalysis and trained on limited 
dataset found here: https://www.kaggle.com/vishnu0399/emergency-vehicle-siren-sounds.
'''


# based off this script with modifications to better suit speaker input
# https://stackoverflow.com/questions/892199/detect-record-audio-in-python

THRESHOLD = 500
CHUNK_SIZE = 1024
FORMAT = pyaudio.paInt16
RATE = 44100
#RATE = 48000

def is_silent(snd_data):
    "Returns 'True' if below the 'silent' threshold"
    return max(snd_data) < THRESHOLD

def normalize(snd_data):
    "Average the volume out"
    MAXIMUM = 16384
    times = float(MAXIMUM)/max(abs(i) for i in snd_data)

    r = array('h')
    for i in snd_data:
        r.append(int(i*times))
    return r

def trim(snd_data):
    "Trim the blank spots at the start and end"
    def _trim(snd_data):
        snd_started = False
        r = array('h')

        for i in snd_data:
            if not snd_started and abs(i)>THRESHOLD:
                snd_started = True
                r.append(i)

            elif snd_started:
                r.append(i)
        return r

    # Trim to the left
    snd_data = _trim(snd_data)

    # Trim to the right
    snd_data.reverse()
    snd_data = _trim(snd_data)
    snd_data.reverse()
    return snd_data

def add_silence(snd_data, seconds):
    "Add silence to the start and end of 'snd_data' of length 'seconds' (float)"
    silence = [0] * int(seconds * RATE)
    r = array('h', silence)
    r.extend(snd_data)
    r.extend(silence)
    return r

def get_clip_size(t):
    return time.time() - t

def record():
    """
    Record a word or words from the microphone and 
    return the data as an array of signed shorts.

    Normalizes the audio, trims silence from the 
    start and end, and pads with 0.5 seconds of 
    blank sound to make sure VLC et al can play 
    it without getting chopped off.
    """
    p = pyaudio.PyAudio()
    

    ##### Replace this stuff to linux specific stuff 

    # #use find_stereo to find speaker device
    # device_info = p.get_device_info_by_index(3)
    # print(device_info)
    # print(int(device_info['defaultSampleRate']))

    # stream = p.open(
    #         format = FORMAT,
    #         channels = 2,
    #         rate = int(device_info['defaultSampleRate']),
    #         input=True,
    #         output=False,
    #         frames_per_buffer=CHUNK_SIZE,
    #         input_device_index=3,
    #         as_loopback=True
    # )

    ##########################

    #use find_stereo to find speaker device
    # appropriate device for linux is pulse, which is the loopback
    device_info = p.get_device_info_by_index(3)

    stream = p.open(
            format = FORMAT,
            channels = 1,
            rate = RATE,
            input=True,
            output=False,
            frames_per_buffer=CHUNK_SIZE
    )
    

    num_silent = 0
    snd_started = False
    clip_start = time.time()

    r = array('h')

    while 1:
        # little endian, signed short

        # checks if silent, if it is increases the number of time where the audio is silent
        snd_data = array('h', stream.read(CHUNK_SIZE))

        if byteorder == 'big':
            snd_data.byteswap()
        r.extend(snd_data)

        silent = is_silent(snd_data)

        if silent and snd_started:
            num_silent += 1
        elif not silent and not snd_started:
            snd_started = True
            clip_start = time.time()
        
        if snd_started and (num_silent > 30 or get_clip_size(clip_start) >= 2):
            print(get_clip_size(clip_start))
            break
        
    sample_width = p.get_sample_size(FORMAT)
    stream.stop_stream()
    stream.close()
    p.terminate()

    return sample_width, r

def record_2_and_classify(path):
    p = pyaudio.PyAudio()

    info = p.get_default_input_device_info()
    # numdevices = info.get('deviceCount')
    # for i in range(0, numdevices):
    #     if (p.get_devisce_info_by_host_api_device_index(0, i).get('maxInputChannels')) > 0:
    #         print("Input Device id ", i, " - ", p.get_device_info_by_host_api_device_index(0, i).get('name'))
    print(info)
    #use find_stereo to find speaker device
    device_info = p.get_device_info_by_index(1)
    print(int(device_info['defaultSampleRate']))

    stream = p.open(
            format = FORMAT,
            channels = 2,
            rate = int(device_info['defaultSampleRate']),
            input=True,
            output=False,
            frames_per_buffer=CHUNK_SIZE,
            input_device_index=1,
            as_loopback=True
    )
    
    print('2 second recording is beginning.')
    seconds = 5

    frames = []
    for i in range(0, int(RATE / CHUNK_SIZE * seconds)):
        data = stream.read(CHUNK_SIZE)
        frames.append(data)
    
    print('done recording')
    stream.stop_stream()
    stream.close()
    p.terminate()

    wf = wave.open(path, 'wb')
    wf.setnchannels(2)
    wf.setsampwidth(p.get_sample_size(FORMAT))
    wf.setframerate(RATE)
    wf.writeframes(b''.join(frames))
    wf.close()

    ret = aT.file_classification(path, 'svmSiren', 'svm')

    return (ret)


def record_to_file(path):
    "Records from the microphone and outputs the resulting data to 'path'"
    sample_width, data = record()
    data = pack('<' + ('h'*len(data)), *data)

    wf = wave.open(path, 'wb')
    wf.setnchannels(2)
    wf.setsampwidth(sample_width)
    wf.setframerate(RATE)
    wf.writeframes(data)
    wf.close()

if __name__ == '__main__':
    
    print("please speak a word into the microphone")
    record_to_file('demo0.wav')
    print('done - result written to demo0.wav')

    sound = AudioSegment.from_wav('demo0.wav')
    sound = sound.set_channels(1)
    sound.export('demo1.wav', format = 'wav')

    print(aT.file_classification('demo0.wav', 'svmSiren', 'svm'))

    # print(aT.file_classification('demo1.wav', 'svmSiren', 'svm'))
    
    #record_2_and_classify('demo0.wav')

    #used for spectrogram analysis - but pyaudioanalysis doesnt need
    '''
    samplingFrequency, signalData = wavfile.read('demo1.wav')

    plt.specgram(signalData, Fs=samplingFrequency, NFFT=512)
    plt.ylabel('Frequency [Hz]')
    plt.xlabel('Time [sec]')
    plt.show()
    '''