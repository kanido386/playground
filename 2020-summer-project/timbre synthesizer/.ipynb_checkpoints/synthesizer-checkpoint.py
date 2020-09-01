import numpy as np
from numpy import sin, pi
from scipy.io.wavfile import write
import matplotlib.pyplot as plt
import librosa

def sinusoid(f, A, t):
    """
    # https://en.wikipedia.org/wiki/Sine_wave
    Input:
        f: ordinary frequency
        A: amplitude
        t: time span
    Output:
        return a sinusoid
    """
    w = 2 * pi * f      # angular frequency
    y = A * sin(w * t)  # sinusoid
    return y

def frame_data(A, f0, t0, fs, N, r1, r2, r3, r4, r5, r6, r7, r8, r9):
    """
    Input:
        A: amplitude
        f0: fundamental frequency
        t0: start time
        fs: sampling rate
        N: frame size
        r1: ratio of fundamental frequency
        r2: ratio of 1st harmonic wave
        r3: ratio of 2nd harmonic wave
        r4: ratio of 3rd harmonic wave
        r5: ratio of 4th harmonic wave
        r6: ratio of 5th harmonic wave
        r7: ratio of 6th harmonic wave
        r8: ratio of 7th harmonic wave
        r9: ratio of 8th harmonic wave
    Output:
        return data
    """
    T = 1/fs                   # sampling interval
    D = T * N                  # frame duration
    t = np.linspace(t0, D, N)  # time span
    f0w = sinusoid(f0 * 1, A, t)   # fundamental frequency wave
    h1w = sinusoid(f0 * 2, A, t)   # 1st harmonic wave
    h2w = sinusoid(f0 * 3, A, t)   # 2nd harmonic wave
    h3w = sinusoid(f0 * 4, A, t)   # 3rd harmonic wave
    h4w = sinusoid(f0 * 5, A, t)   # 4th harmonic wave
    h5w = sinusoid(f0 * 6, A, t)   # 4th harmonic wave
    h6w = sinusoid(f0 * 7, A, t)   # 4th harmonic wave
    h7w = sinusoid(f0 * 8, A, t)   # 4th harmonic wave
    h8w = sinusoid(f0 * 9, A, t)   # 4th harmonic wave
    data = f0w * r1 + h1w * r2 + h2w * r3 + h3w * r4 + h4w * r5 + h5w * r6 + h6w * r7 + h7w * r8 + h8w * r9
    return data

def sound_synthesizer(mode, inputItem, frame_size, config, output_filename):
    """
    # https://librosa.org/librosa/master/generated/librosa.core.piptrack.html
    Input:
        mode: (1) frequency as input   (2) frequency_list as input   (3) filename as input
        inputItem: filename or frequency_list or frequency
        frame_size: frame size (useless for mode 3)
        config: modify timbre
        output_filename: output filename
    Output:
        write data to output_filename
    """
    t0 = 0          # start time
    fs = 48000      # sampling rate
    N = frame_size  # frame size
    
    r1 = config['r1']
    r2 = config['r2']
    r3 = config['r3']
    r4 = config['r4']
    r5 = config['r5']
    r6 = config['r6']
    r7 = config['r7']
    r8 = config['r8']
    r9 = config['r9']
    
    if mode == 1:
        
        A = 0.5
        f0 = inputItem
        
    elif mode == 2:
        
        A = 0.5
        # f0 = [261, 293, 329, 349, 392, 440, 493, 523]   C4~C5
        f0 = inputItem
        
    elif mode == 3:
        
        A = []
        f0 = []

        y, sr = librosa.load(inputItem, sr=None)
        pitches, magnitudes = librosa.core.piptrack(y=y, sr=sr, fmin=75, fmax=1600)
        
        for i in range(int(magnitudes.size / 1025)):
            index = magnitudes[:, i].argmax()
            pitch = pitches[index, i]
            volume = magnitudes[index, i]
            A.append(volume)
            f0.append(pitch)
        
        maximum = max(A)
        for i in range(len(A)):
            A[i] = A[i] / maximum / 2
        
    result = []
    
    if mode == 1:
        result = frame_data(A, f0, t0, fs, N, r1, r2, r3, r4, r5, r6, r7, r8, r9)
    elif mode == 2:
        for f in f0:
            result.extend(frame_data(A, f, t0, fs, N, r1, r2, r3, r4, r5, r6, r7, r8, r9))
    elif mode == 3:
        for i in range(len(A)):
            result.extend(frame_data(A[i], f0[i], t0, fs, 512, r1, r2, r3, r4, r5, r6, r7, r8, r9))
        
    result = np.array(result)
    
    if mode == 3:
        write(output_filename, sr, result)
    else:
        write(output_filename, fs, result)