import numpy as np
from scipy.signal import get_window
from math import ceil
from operator import itemgetter
import json

def cut_frames(data, frame_size, hop_size, window):
  """
  """
  overlap = frame_size - hop_size
  num_frame = (data.size - overlap) // hop_size
  frames = []
  cur = overlap
  i = 0
  while i < num_frame:
      cur = cur - overlap
      j = 0
      frame = np.zeros(frame_size)
      while j < frame_size:
          frame[j] = data[cur]
          cur = cur + 1
          j = j + 1
      i = i + 1
      frame = frame * get_window(window, frame_size)
      frames.append(frame)
  frames = np.array(frames)

  return frames, num_frame

def change_timbre(spectrum, num_frames, frame_size, option):
    """
    """
    precision = 5
    new_spectrum = []
    for j in range(num_frames):
        X = abs(spectrum[j])
        Y = spectrum[j]
        # https://stackoverflow.com/questions/4624970/finding-local-maxima-minima-with-numpy-in-a-1d-numpy-array
        local_maximum = np.r_[True, X[1:] > X[:-1]] & np.r_[X[:-1] > X[1:], True]
        local_maximum_list = []
        for i in range(len(local_maximum)//2):
            if local_maximum[i] == True:
                local_maximum_list.append((i, X[i]))
        # https://stackoverflow.com/questions/10695139/sort-a-list-of-tuples-by-2nd-item-integer-value
        # https://stackoverflow.com/questions/13145368/find-the-maximum-value-in-a-list-of-tuples-in-python
        wow = sorted(local_maximum_list, key=itemgetter(1), reverse=True)
        boss = max(wow, key=itemgetter(1))
        lowest = boss[0]
        for i in range(precision):
            if wow[i][0] < boss[0] and wow[i][1] >= boss[1] / 2:
                lowest = wow[i][0]

        new_X = np.zeros(Y.size)
        new_X[lowest] = Y[lowest]

        # ==================================================

        if option == 1:
            # trumpet
            new_X[lowest*2] = new_X[lowest] * 25.19 / 18.03
            new_X[lowest*3] = new_X[lowest] * 28.12 / 18.03
            new_X[lowest*4] = new_X[lowest] * 16.00 / 18.03
            new_X[lowest*5] = new_X[lowest] * 4.91 / 18.03
            new_X[lowest*6] = new_X[lowest] * 2.74 / 18.03
            new_X[lowest*7] = new_X[lowest] * 2.73 / 18.03
            new_X[lowest*8] = new_X[lowest] * 1.48 / 18.03
            # new_X[lowest*9] = new_X[lowest] * 0.80 / 18.03
        elif option == 2:
            # oboe
            new_X[lowest*2] = new_X[lowest] * 50.92 / 14.62
            new_X[lowest*3] = new_X[lowest] * 21.34 / 14.62
            new_X[lowest*4] = new_X[lowest] * 7.71 / 14.62
            new_X[lowest*5] = new_X[lowest] * 1.72 / 14.62
            new_X[lowest*6] = new_X[lowest] * 2.78 / 14.62
            new_X[lowest*7] = new_X[lowest] * 0.15 / 14.62
            new_X[lowest*8] = new_X[lowest] * 0.70 / 14.62
            new_X[lowest*9] = new_X[lowest] * 0.07 / 14.62
        elif option == 3:
            # marimba
            new_X[lowest*2] = new_X[lowest] * 13.60 / 85.72
        elif option == 4:
            # organ
            new_X[lowest*2] = new_X[lowest] * 13.38 / 22.71
            new_X[lowest*3] = new_X[lowest] * 15.12 / 22.71
            new_X[lowest*4] = new_X[lowest] * 15.36 / 22.71
            new_X[lowest*5] = new_X[lowest] * 0.97 / 22.71
            new_X[lowest*6] = new_X[lowest] * 26.22 / 22.71
            new_X[lowest*7] = new_X[lowest] * 0.49 / 22.71
            new_X[lowest*8] = new_X[lowest] * 3.96 / 22.71
            new_X[lowest*9] = new_X[lowest] * 1.79 / 22.71
        elif option == 5:
            # glockenspiel
            new_X[lowest*3] = new_X[lowest] * 62.06 / 27.56
            new_X[lowest*6] = new_X[lowest] * 7.56 / 27.56
            new_X[lowest*9] = new_X[lowest] * 2.34 / 27.56
        elif option == 6:
            # pure
            None
        elif option == 7:
            with open('data.txt') as json_file:
                data = json.load(json_file)
            new_X[lowest*2] = new_X[lowest] * data['r2'] / data['r1']
            new_X[lowest*3] = new_X[lowest] * data['r3'] / data['r1']
            new_X[lowest*4] = new_X[lowest] * data['r4'] / data['r1']
            new_X[lowest*5] = new_X[lowest] * data['r5'] / data['r1']
            new_X[lowest*6] = new_X[lowest] * data['r6'] / data['r1']
            new_X[lowest*7] = new_X[lowest] * data['r7'] / data['r1']
            new_X[lowest*8] = new_X[lowest] * data['r8'] / data['r1']
            new_X[lowest*9] = new_X[lowest] * data['r9'] / data['r1']
        
        new_spectrum.append(new_X)
    
    new_spectrum = np.array(new_spectrum)
    return new_spectrum