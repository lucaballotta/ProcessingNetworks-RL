import os.path
import numpy as np
from numpy.core.fromnumeric import size
import pickle
from tqdm import trange

# Environment: star network with sensors transmitting to master node

class ProcessingNetwork:


    def __init__(self, sensors, A, Q, window_length, window_num):

        self.sensors = sensors                              # list of sensors transmitting to master
        self.sens_num = len(self.sensors)                   # amounts of sensors
        self.state_bins = []                                # bins for state space quantization
        self.state_dim = 0                                  # state space dimension
        self.action_dim = self.sens_num + 1                 # amount of actions 
        self.actions = range(self.action_dim)               # action space
        self.proc_sens = 0                                  # amount of sensors currently processing
        self.A = A.copy()                                   # state matrix
        self.Q = Q.copy()                                   # stored_covariance of process noise
        self.n = size(A, 1)                                 # dynamical state dimension
        self.window_length = int(window_length)             # length of time window
        self.window_num = window_num                        # number windows such that K = window_length * window_num
        self.timer = 0                                      # tracks number of windows run so far
        self.STORAGE_DIM = 100                              # amount of measurements that can be  at the central station
        self.stored_delays = None                           # delays of stored measurements
        self.stored_info_mat = None                         # information matrices of stored measurements
        self.P0 = 10 * np.identity(self.n)                  # error covarance initial condition
        self.stored_cov = None                              # stored error covariances
        self.P = None                                       # current error covariance
        self.Ps = []                                        # error covariance history


    ### Apply environment transition
    def step(self, action):

        proc_sens_new = action
        
        # update processing sensors
        if proc_sens_new < self.proc_sens:

            # disable sensors of sens_type
            for to_disable in range(proc_sens_new, self.proc_sens):
                self.sensors[to_disable].disable()

        elif proc_sens_new > self.proc_sens:

            # enable sensors of sens_type
            for to_enable in range(self.proc_sens, proc_sens_new):
                self.sensors[to_enable].enable()
                
        for sensor in self.sensors:
            sensor.reset()

        self.proc_sens = proc_sens_new

        # compute reward for this episode
        state, reward = self._compute_reward()

        # check condition for episode termination
        self.timer += 1
        done = False
        if self.timer == self.window_num:
            done = True

        return state, reward, done


    ## Reset environment
    def reset(self, action_init=None):
        self.stored_delays = [np.inf for _ in range(self.STORAGE_DIM)]
        self.stored_info_mat = [np.zeros((self.n, self.n)) for _ in range(self.STORAGE_DIM)]
        self.stored_cov = [self.P0.copy() for _ in range(self.STORAGE_DIM)]
        self.P = self.P0.copy()
        if action_init is None:
            self.proc_sens = np.random.randint(0, self.action_dim)
            
        else:
            self.proc_sens = action_init

        for to_enable in range(self.proc_sens):
            self.sensors[to_enable].enable()
            self.sensors[to_enable].reset()

        for to_disable in range(self.proc_sens, self.sens_num):
            self.sensors[to_disable].disable()
            self.sensors[to_disable].reset()

        self.timer = 0
        self.Ps = []
        state_init = self._quantize(np.trace(self.P0))
        
        return state_init
    
    
    ### Get history of error variance
    def get_Ps(self):
        return np.array(self.Ps.copy())


    ### Compute reward for one window
    def _compute_reward(self):
        avg_err_var = 0
        for t in range(self.window_length):

            # update delays
            self.stored_delays = [delay + 1 for delay in self.stored_delays]

            # store available data
            sensor_has_new_data = False
            for sensor in self.sensors:
                sensor.time_step()
                new_data_from_sensor = self._collect_data(sensor, t)
                if not(sensor_has_new_data) and new_data_from_sensor:
                    sensor_has_new_data = True

            # update current error covariance
            if sensor_has_new_data:
                self.P = self._open_loop(self.stored_cov[-1], self.stored_delays[-1])

            else:
                self.P = self._open_loop(self.P, 1)

            # update average error variance
            err_var_curr = np.trace(self.P)
            avg_err_var += (err_var_curr - avg_err_var) / (t+1)
            self.Ps.append(err_var_curr)

        state = self._quantize(err_var_curr)
        reward = -avg_err_var
        
        return state, reward


    ### Collect sensory data and update Kalman predictor
    def _collect_data(self, sensor, t):
        new_data_collected = False

        if sensor.has_new_data():
            delay, info_mat = sensor.new_data()
            if delay < self.stored_delays[0]:
                new_data_collected = True
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

                            # update outdated error covariance associated with currently iterated measurement
                            P_temp = self._open_loop(self.stored_cov[index-1], delay-stored_delay)
                            self.stored_cov[index] = self._update(P_temp, self.stored_info_mat[index])

                        else:
                            if index > 0:

                                # overwrite old data with fresher ones
                                self.stored_delays[index-1] = stored_delay
                                self.stored_info_mat[index-1] = self.stored_info_mat[index].copy()
                                self.stored_cov[index-1] = self.stored_cov[index].copy()

                if not(new_data_stored):
                    if self.stored_delays[-1] < np.inf:
                        P_temp = self._open_loop(self.stored_cov[-1], self.stored_delays[-1]-delay)
                        
                    else:

                        # fill array at the beginning (first measurement)
                        P_temp = self._open_loop(self.stored_cov[-1], t-delay)

                    self.stored_cov[-1] = self._update(P_temp, info_mat)
                    self.stored_delays[-1] = delay
                    self.stored_info_mat[-1] = info_mat.copy()

        return new_data_collected
    
    
    ### Kalman predictor - open loop
    def _open_loop(self, P0, T):
        if T == 0:
            return P0

        Q_tot = self.Q.copy()
        for t in range(1, T):
            A_t = np.linalg.matrix_power(self.A, t)
            Q_tot += np.matmul(A_t, np.matmul(self.Q, np.transpose(A_t)))

        A_T = np.linalg.matrix_power(self.A, T)
        
        return np.matmul(A_T, np.matmul(P0, np.transpose(A_T))) + Q_tot


    ### Kalman predictor - update with measurement
    def _update(self, P0, info_mat):
        if self.n > 1:
            try:
                return np.linalg.inv(np.linalg.inv(P0) + info_mat)
            
            except:
                print(P0)
                print(info_mat)
                
        else:
            return (P0**-1 + info_mat)**-1


    ### State quantization
    def _quantize(self, err_var):
        for i in range(self.state_dim-1):
            if err_var < self.state_bins[i]:
                return i
                
        return self.state_dim-1


    ### Quantize domain of P_k
    def get_uniform_discretization(self, where, save=True, replace=False):
        if not replace:
            try:
                with open(os.path.join(where, 'uniform_discretization.pickle'), 'rb') as file:
                    print('A uniform discretization has already been found!')
                    bins = pickle.load(file)
                    self.set_discretization(bins)
                    return bins
                                    
            except FileNotFoundError:
                pass
            
        collected_states = []
        for i in trange(self.action_dim):
            self.reset()
            done = False
            while not done:
                _, _, done = self.step(i)
            collected_states += self.get_Ps().tolist()
            
        assert len(collected_states) == self.action_dim*self.window_num*self.window_length
        collected_states.sort()

        # define state quantization
        partition = len(collected_states)//8
        bins = [(collected_states[partition*1]+collected_states[partition*2])/2]
        bins.append((collected_states[partition*3]+collected_states[partition*5])/2)
        bins.append((collected_states[partition*6]+collected_states[partition*7])/2)
        bins.append((np.trace(self.P0)+collected_states[partition*7])/2)
        self.set_discretization(bins)
        if save:
            with open(os.path.join(where, 'uniform_discretization.pickle'), 'wb') as file:
                pickle.dump(bins, file)

        return bins


    ### Set state space quantization
    def set_discretization(self, a):
        self.state_bins = a
        self.state_dim = len(a) + 1