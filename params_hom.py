import numpy as np
from sensor import Sensor

# Time horizon for optimization
time_horizon = int(5e2)
window_length = int(5e1)                                                    # decision interval
window_num = int(time_horizon // window_length)                             # time horizon = window_num * window_length

# System dynamics
T = .01                                                                     # sample time
A = np.array([[1, T], [0, 1]])                                              # linearized state dynamics
Q = 1e3*np.array([[0, 0], [0, T ** 2]])                                     # process noise covariance matrix
trP_bins = 5                                                                # bins for state quantization

# Smart sensors
sensors = []                                                                # all sensors
N = 4                                                                       # amount of sensors
C = np.array([1, 0])                                                        # state-output matrix
C = np.transpose(C[:, np.newaxis])                                        
V_raw = 10                                                                  # raw measurement noise covariance matrix
V_proc = 1                                                                  # processed measurement noise covariance matrix
del_raw = int(100/25)                                                       # raw measurements delay
del_proc = del_raw + 10                                                     # processing delay
enabled_at_start = False                                                    # sensor processing at start
comm_del = 1
sensor = Sensor(del_raw, V_raw, del_proc, V_proc, enabled_at_start, C, \
    comm_del, comm_del)                                                     # network sensor
sensors = [sensor.copy() for _ in range(N)]