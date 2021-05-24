import pandas as pd
import numpy as np
from scipy.interpolate import interp1d
from scipy.signal import butter, lfilter

def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5*fs
    normal_cutoff = cutoff /nyq
    b, a = butter(order, normal_cutoff, btype="low", analog=False)
    return b, a

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y


if __name__ == "__main__":
    # load differential data
    # differential data was generated with image processing (opencv etc.)
    df_diff = pd.read_csv("./diff.csv",
                            header=0,
                            dtype={
                                "frame": int,
                                "altitude_diff": float,
                                "airspeed_diff": float,
                                "direction_diff": float
                            })
    
    # load key values
    # key values are the annotation data by hands
    df_keyvalue = pd.read_csv("./keyvalue.csv",
                                header=0,
                                dtype={
                                    "frame": int,
                                    "altitude": float,
                                    "airspeed": float,
                                    "direction": float
                                })
    
    # evaluate data for each frame
    frame_sequence = np.linspace(2010, 16060, num=16060-2010+1, endpoint=True, dtype=np.int32)
    altitude = np.zeros(16060-2010+1)
    airspeed = np.zeros(16060-2010+1)
    direction = np.zeros(16060-2010+1)

    for i, frame in enumerate(frame_sequence):
        if frame in df_keyvalue["frame"].values:
            keyvalue = df_keyvalue.loc[df_keyvalue["frame"] == frame]
            altitude[i] = keyvalue["altitude"].values[0]
            airspeed[i] = keyvalue["airspeed"].values[0]
            direction[i] = keyvalue["direction"].values[0]
            if i > 0 and abs(altitude[i] - altitude[i-1]) > 0.25:
                print("W: altitude between frame {0} and frame {1} is not continuous.".format(int(frame-1), int(frame)))
            if i > 0 and abs(airspeed[i] - airspeed[i-1]) > 2.5:
                print("W: airspeed between frame {0} and frame {1} is not continuous.".format(int(frame-1), int(frame)))
            if i > 0 and abs(direction[i] - direction[i-1]) > 0.5:
                print("W: direction between frame {0} and frame {1} is not continuous.".format(int(frame-1), int(frame)))

        else:
            altitude[i] = altitude[i-1] + df_diff.loc[df_diff["frame"] == frame]["altitude_diff"].values[0]
            airspeed[i] = airspeed[i-1] + df_diff.loc[df_diff["frame"] == frame]["airspeed_diff"].values[0]
            direction[i] = direction[i-1] + df_diff.loc[df_diff["frame"] == frame]["direction_diff"].values[0]
            if direction[i] > 36:
                direction[i] -= 36
            elif direction[i] < 0:
                direction[i] += 36
    
    # apply low pass filter
    order = 6
    fs = 30.0
    cutoff = 1
    f_altitude = butter_lowpass_filter(altitude, cutoff, fs, order)
    f_airspeed = butter_lowpass_filter(airspeed, cutoff, fs, order)
    f_direction = butter_lowpass_filter(direction, cutoff, fs, order)

    # export
    with open("altitude-airspeed-direction.csv", "w") as f:
        f.write("frame,altitude,airspeed,direction\n")
        for frame in range(150):
            f.write("{0},{1},{2},{3}\n".format(frame, 0.08, 0, 28.2))
        for i, frame in enumerate(frame_sequence):
            f.write("{0},{1},{2},{3}\n".format(frame, f_altitude[i], f_airspeed[i], f_direction[i]))
