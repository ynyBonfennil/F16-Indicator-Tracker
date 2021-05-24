# This script was used to interpolate between key values.
# It prints csv data, and I pasted them to the keyvalue.csv
from scipy.interpolate import interp1d
import numpy as np

frame = np.array([15491, 15595, 15632, 15682, 15711, 15761, 15793, 15808])
alt = np.array([9.4, 9.35, 9.3, 9.25, 9.16, 9.12, 9.6])
air = np.array([31., 33, 34, 35, 35.7, 36.7, 37.2, 37.7])
dir = np.array([16.1, 17, 17.2, 17.3, 18.1, 18.2, 18, 17.7])

f_alt = interp1d(frame, alt, kind="cubic")
f_air = interp1d(frame, air, kind="cubic")
f_dir = interp1d(frame, dir, kind="cubic")

for i in range(frame[0]+1, frame[-1]):
    print("{0},{1:.2f},{2:.2f},{3:.2f}".format(i,f_alt([i])[0], f_air([i])[0], f_dir([i])[0]))
