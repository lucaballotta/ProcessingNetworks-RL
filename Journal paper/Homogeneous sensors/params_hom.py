import numpy as np
from sensor import Sensor

# Time horizon for optimization
time_horizon = int(5e2)
window_length = int(5e1)                                                    # decision interval
window_num = int(time_horizon // window_length)                             # time horizon = window_num * window_length

# System dynamics
T = .01                                                                     # sampling time
A = np.array([[1, T], [0, 1]])                                              # state dynamics
Q = 1e3*np.array([[0, 0], [0, T ** 2]])                                     # covariance of process noise

# Smart sensors
N = 4                                                                       # amount of sensors
C = np.array([1, 0])                                                        # state-to-output transformation
C = np.transpose(C[:, np.newaxis])                                        
V_raw = 10                                                                  # covariance of measurement noise of raw data 
V_proc = 1                                                                  # covariance of measurement noise of processed data
del_raw = int(100/25)                                                       # computation delay of raw measurements
del_proc = del_raw + 10                                                     # computation delay of processed measurements
comm_del_raw = 1                                                            # communication delay of raw measurements
comm_del_proc = 1                                                           # communication delay of processed measurements
enabled_at_start = False                                                    # sensor starts in processing mode
sensor = Sensor(
    del_raw, V_raw, del_proc, V_proc, enabled_at_start, 
    C, comm_del_raw, comm_del_proc)                                         # smart sensor
sensors = [sensor.copy() for _ in range(N)]                                 # all sensors in network