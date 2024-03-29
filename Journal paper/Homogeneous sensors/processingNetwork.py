import os.path
import numpy as np
import pickle

from math import ceil
from typing import List, Tuple
from numpy.core.fromnumeric import size

from sensor import Sensor

# Environment: star network with sensors transmitting data to master node

class ProcessingNetwork:
    
    def __init__(self, sensors: List[Sensor], A: np.ndarray, Q: np.ndarray, window_length: int, window_num: int):
        
        self.sensors = sensors                        # list of sensors transmitting to master
        self.sens_num = len(self.sensors)             # amounts of sensors
        self.state_bins = []                          # bins for state space quantization
        self.state_dim = 0                            # state space dimension
        self.action_dim = (self.sens_num + 2) * \
            (self.sens_num + 1) // 2                  # amount of actions
        action_map = []
        for i in range(self.sens_num + 1):
           for j in range(i, self.sens_num + 1): 
               action_map.append(i*100+j)

        self.action_map = action_map
        self.tx_sens = 0                              # amount of sensors currently transmitting
        self.proc_sens = 0                            # amount of sensors currently processing
        self.A = A.copy()                             # state matrix
        self.Q = Q.copy()                             # stored_covariance of process noise
        self.n = size(A, 1)                           # state dimension
        self.window_length = int(window_length)       # length of time window
        self.window_num = window_num
        self.window_curr = 0                          # tracks number of windows run so far
        self.STORAGE_DIM = 100                        # amount of measurements that can be stored
        self.stored_delays = None                     # delays of stored measurements
        self.stored_info_mat = None                   # information matrices of stored measurements
        self.P0 = 10 * np.identity(self.n)            # error covarance initial condition
        self.stored_cov = None                        # stored error covariances
        self.P = None                                 # current error covariance
        self.Ps = []                                  # error covariance history
        
        # variables for base station processing
        self.processing_factor = 1.0                  # sensor-wise processing factor
        self.msgs_to_process = 0                      # amount of messages to be processed
        self.is_processing = False                    # true if the base station is currently processing
        self.start_processing = 0                     # time instant processing starts at
        self.previous_new_data = False                # true if data are received while the base station is processing


    ### Apply environment transition
    def step(self, action_ID: int) -> Tuple[int, float, bool]:
        '''
        Applies environment transition.
        Sensors are set in the self.sensors list as follows:

        [ 0|1| ...|P-1|...|T-1| ...|N -1]
        [ PROCESSING  |  RAW  |  SLEEP  ]

        with:
            self.proc_sens = P = #PROCESSING
            self.tx_sens = T = #PROCESSING + #RAW
        
        Input
        -----
        action_ID: int,
            code for number of sensors in raw, sleep, and processing mode 
            
        Returns
        -------
        state: int,
            updated bin of the trace of the estimation error covariance matrix (quantized)
        reward: float,
            average trace of the estimation error covariance matrix across episode (time window)
        done: bool,
            True if all time windows within one time horizon have been run
        '''
        # decode action from action map
        action = self.action_map[action_ID]
        tx_sens_new = action % 100
        proc_sens_new = action // 100
        
        # update sensor modes
        if proc_sens_new < self.proc_sens:
            for to_enable_raw in range(proc_sens_new, self.proc_sens):
                self.sensors[to_enable_raw].set_raw()
                self.sensors[to_enable_raw].reset()

        if tx_sens_new < self.tx_sens:
            
            # set sensors asleep
            for to_disable in range(tx_sens_new, self.tx_sens):
                self.sensors[to_disable].set_sleep()
                self.sensors[to_disable].reset()
       
        elif tx_sens_new > self.tx_sens:
            
            # set sensors raw 
            for to_enable_raw in range(self.tx_sens, tx_sens_new):
                self.sensors[to_enable_raw].set_raw()
                self.sensors[to_enable_raw].reset()
        
        if proc_sens_new > self.proc_sens:
            
            # set sensors processing
            for to_enable_proc in range(self.proc_sens, proc_sens_new):
                self.sensors[to_enable_proc].set_proc()
                self.sensors[to_enable_proc].reset()

        self.tx_sens = tx_sens_new
        self.proc_sens = proc_sens_new

        # compute reward for this episode
        state, reward = self._compute_reward()

        # check condition for episode termination
        self.window_curr += 1
        done = False
        if self.window_curr == self.window_num:
            done = True

        return state, reward, done


    def reset(self, action_init: int=None) -> int:
        '''
        Resets environment.
        
        Input
        -----
        action_init: int,
            code for initial number of sensors in raw, sleep, and processing mode
            
        Returns
        -------
        state_init: int,
            initial bin of the trace of the estimation error covariance matrix (quantized)
        '''
        self.stored_delays = [np.inf for _ in range(self.STORAGE_DIM)]
        self.stored_info_mat = [np.zeros((self.n, self.n)) for _ in range(self.STORAGE_DIM)]
        self.stored_cov = [self.P0.copy() for _ in range(self.STORAGE_DIM)]
        self.P = self.P0.copy()
        
        if action_init is None:
            action = self.action_map[np.random.randint(0, self.action_dim)]
            self.tx_sens = action % 100
            self.proc_sens = action // 100

        else:
            action = self.action_map[action_init]
            self.tx_sens = action % 100
            self.proc_sens = action // 100
           
        for to_enable_process in range(self.proc_sens):
            self.sensors[to_enable_process].set_proc()
            self.sensors[to_enable_process].reset()
                
        for to_enable_raw in range(self.proc_sens, self.tx_sens):
            self.sensors[to_enable_raw].set_raw()
            self.sensors[to_enable_raw].reset()
        
        for to_disable in range(self.tx_sens, self.sens_num):
            self.sensors[to_disable].set_sleep()
            self.sensors[to_disable].reset()
        
        self.msgs_to_process = 0
        self.is_processing = False
        self.start_processing = 0
        self.previous_new_data = False
        self.window_curr = 0
        self.Ps = []
        state_init = self._quantize(np.trace(self.P0))
        
        return state_init
    
    def get_Ps(self) -> np.ndarray:
        '''
        Get history of error variance.
        
        Returns
        -------
        Ps_array: numpy array,
            all values of the trace of the estimation error variance recorded since the latest reset.
        '''
        Ps_array = np.array(self.Ps.copy())
        return Ps_array


    def _compute_reward(self) -> Tuple[int, float]:
        '''
        Compute reward for one time window.
        
        Returns
        -------
        state: int,
            current bin of the trace of the estimation error covariance matrix (quantized)
        reward: float,
            average trace of the estimation error covariance matrix across episode (time window)
        '''
        avg_err_var = 0
        processing_time = 0
        time_curr = self.window_curr * self.window_length
        for time_window in range(self.window_length):
            time_curr += 1
            
            # check if the base station has completed processing
            if self.is_processing:
                if (time_curr - self.start_processing) > processing_time:
                    self.is_processing = False

            # update delays
            self.stored_delays = [delay + 1 for delay in self.stored_delays]

            # store available data
            new_data_time_curr = False
            for sensor in self.sensors:
                sensor.time_step()
                new_data_from_sensor, new_data_index = self._collect_data(sensor, time_curr)
                if not(new_data_time_curr) and new_data_from_sensor:
                    new_data_time_curr = True

                if new_data_from_sensor:
                    self.msgs_to_process = max(self.STORAGE_DIM - new_data_index, self.msgs_to_process)
                        
            if self.previous_new_data:
                new_data_time_curr = True

            # update current error covariance
            if new_data_time_curr:
                if not self.is_processing:
                    self.P = self._open_loop(self.stored_cov[-1], self.stored_delays[-1])
                    self.previous_new_data = False
                    self.is_processing = True
                    processing_time = ceil(self.msgs_to_process * self.processing_factor)
                    self.start_processing = time_curr
                    self.msgs_to_process = 0

                else:
                    self.P = self._open_loop(self.P, 1)
                    self.previous_new_data = True

            else:
                self.P = self._open_loop(self.P, 1)

            # update average error variance
            err_var_curr = np.trace(self.P)
            avg_err_var += (err_var_curr - avg_err_var) / (time_window+1)
            self.Ps.append(err_var_curr)

        state = self._quantize(err_var_curr)
        reward = -avg_err_var
        
        return state, reward


    def _collect_data(self, sensor: Sensor, t: int) -> Tuple[bool, int]:
        '''
        Collects sensory data and updates state estimate and estimation error covariance matrix.
        
        Input
        -----
        sensor: Sensor,
            smart sensor in the network
        t: int,
            current time
            
        Returns
        -------
        new_data: bool,
            True if new data have been transmitted from sensor
        new_data_index: int,
            position of new measurement
        '''
        new_data = False
        new_data_index = None
        if sensor.has_new_data():
            delay, info_mat = sensor.new_data()
            if delay < self.stored_delays[0]:
                new_data = True
                new_data_stored = False

                # store new measurement and remove oldest one (one mesurement per each memory cell)
                for index, stored_delay in enumerate(self.stored_delays):

                    # if new measurement already stored, update error covariances associated with fresher measurements
                    if new_data_stored:
                        P_temp = self._open_loop(self.stored_cov[index-1], self.stored_delays[index-1]-stored_delay)
                        self.stored_cov[index] = self._update(P_temp, self.stored_info_mat[index])

                    else:
                        if delay >= stored_delay:

                            # update outdated error covariance associated with new measurement
                            if self.stored_delays[index-1] < np.inf:
                                P_temp = self._open_loop(self.stored_cov[index-1], self.stored_delays[index-1]-delay)

                            else:

                                # fill array at the beginning
                                P_temp = self._open_loop(self.stored_cov[index-1], t-delay)

                            self.stored_cov[index-1] = self._update(P_temp, info_mat)

                            # store new measurement
                            self.stored_delays[index-1] = delay
                            self.stored_info_mat[index-1] = info_mat.copy()
                            new_data_stored = True
                            new_data_index = index - 1

                            # update outdated error covariance associated with currently iterated measurement
                            P_temp = self._open_loop(self.stored_cov[index-1], delay-stored_delay)
                            self.stored_cov[index] = self._update(P_temp, self.stored_info_mat[index])

                        else:
                            if index > 0:

                                # overwrite old data with fresher ones
                                self.stored_delays[index-1] = stored_delay
                                self.stored_info_mat[index-1] = self.stored_info_mat[index].copy()
                                self.stored_cov[index-1] = self.stored_cov[index].copy()

                if not new_data_stored:
                    if self.stored_delays[-1] < np.inf:
                        P_temp = self._open_loop(self.stored_cov[-1], self.stored_delays[-1]-delay)

                    else:

                        # fill array at the beginning (first measurement)
                        P_temp = self._open_loop(self.stored_cov[-1], t-delay)

                    self.stored_cov[-1] = self._update(P_temp, info_mat)
                    self.stored_delays[-1] = delay
                    self.stored_info_mat[-1] = info_mat.copy()
                    new_data_index = self.STORAGE_DIM - 1

        return new_data, new_data_index


    def _open_loop(self, P0: np.ndarray, T: int) -> np.ndarray:
        '''
        Runs open loop step of Kalman filter.
        
        Input
        -----
        P0: numpy array,
            initial estimation error covariance matrix
        T: int,
            numer of open-loop steps
            
        Returns
        -------
        P_t: numpy array,
            estimation error covariance matrix after T open-loop steps
        '''
        if T == 0:
            P_T = P0
            
        else:
            Q_tot = self.Q.copy()
            for t in range(1, T):
                A_t = np.linalg.matrix_power(self.A, t)
                Q_tot += np.matmul(A_t, np.matmul(self.Q, np.transpose(A_t)))

            A_T = np.linalg.matrix_power(self.A, T)
            P_T = np.matmul(A_T, np.matmul(P0, np.transpose(A_T))) + Q_tot
        
        return P_T


    def _update(self, P0: np.ndarray, info_mat: np.ndarray) -> np.ndarray:
        '''
        Runs update with measurement of Kalman filter.
        
        Input
        -----
        P0: numpy array,
            initial estimation error covariance matrix
        info_mat: numpy array,
            information matrix associated with measurement
            
        Returns
        -------
        P_update: numpy array,
            estimation error covariance matrix after update with measurement
        '''
        if self.n > 1:
            P_update = np.linalg.inv(np.linalg.inv(P0) + info_mat)
        
        else:
            P_update = (P0**-1 + info_mat)**-1
            
        return P_update


    def _quantize(self, err_var: float) -> int:
        '''
        Find quantization bin corresponding to the trace of the estimation error covariance matrix.
        
        Input
        -----
        err_var: float,
            trace of the estimation error covariance matrix
            
        Returns
        -------
        bin: int,
            quantization bin corresponding to err_var
        '''
        for i in range(self.state_dim-1):
            if err_var < self.state_bins[i]:
                return i
                
        return self.state_dim - 1


    def get_uniform_quantization(self, where: str, save: bool=True, replace: bool=False) -> List[float]:
        '''
        Quantize domain of trace of estimation error covariance matrix.
        
        Input
        -----
        where: string,
            path to store bins
        save: bool,
            True if bins should be saved
        replace: bool,
            True if previously stored bins should be replaced
            
        Returns
        -------
        bins: list[float],
            quantization thresholds
        '''
        if not replace:
            try:
                with open(os.path.join(where, 'uniform_discretization.pickle'), 'rb') as file:
                    print('A uniform discretization has already been found!')
                    bins = pickle.load(file)
                    self.set_quantization(bins)
                    return bins
                                    
            except FileNotFoundError:
                pass

        bins = [6.6, 7.3, 9.0, 12.5]
        self.set_discretization(bins)
        if save:
            with open(os.path.join(where, 'uniform_discretization.pickle'), 'wb') as file:
                pickle.dump(bins, file)

        return bins


    def set_quantization(self, bins: List[float]):
        '''
        Set state space quantization for the environment.
        
        Input
        -----
        bins: list[float],
            thresholds of bins thaqt define the state space of the environment         
        '''
        self.state_bins = bins
        self.state_dim = len(bins) + 1