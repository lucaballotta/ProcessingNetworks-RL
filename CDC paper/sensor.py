import numpy as np

from typing import Tuple

class Sensor:
    
    def __init__(self, raw_delay: int, raw_V: np.ndarray, proc_delay: int, proc_V: np.ndarray, enabled: bool, C: np.ndarray, raw_comm_del: int, proc_comm_del: int):
        self.raw_delay = raw_delay
        self.proc_delay = proc_delay
        
        # compute information matrices for Kalman filter
        try:
            self.raw_V = raw_V.copy()
            raw_V_inv = np.linalg.inv(raw_V)
            self.raw_info_mat = np.matmul(np.transpose(C),np.matmul(raw_V_inv,C))
            
        except:
            self.raw_V = raw_V
            self.raw_info_mat = np.matmul(np.transpose(C),C) / raw_V
            
        try:
            self.proc_V = proc_V.copy()
            proc_V_inv = np.linalg.inv(proc_V)
            self.proc_info_mat = np.matmul(np.transpose(C),np.matmul(proc_V_inv,C))
            
        except:
            self.proc_V = proc_V
            self.proc_info_mat = np.matmul(np.transpose(C),C) / proc_V
        
        self.enabled = enabled                                              # true if sensor starts in processing mode
        self.C = C.copy()                                                   # state-to-output transformation
        self.raw_comm_delay = raw_comm_del
        self.proc_comm_delay = proc_comm_del

        if self.enabled:
            self.sample_time = self.proc_delay                              
            self.comm_delay = self.proc_comm_delay
            self.delay_info = self.proc_delay + self.proc_comm_delay, self.proc_info_mat
            
        else:
            self.sample_time = self.raw_delay
            self.comm_delay = self.raw_comm_delay
            self.delay_info = self.raw_delay + self.raw_comm_delay, self.raw_info_mat

        self.curr_delay_info = self.delay_info                              # delay and information matrix for acquired data
        self.data_in_delivery_delay_info = self.delay_info                  # delay and information matrix for transmitted data
        self.delivered_delay_info = self.delay_info                         # delay and information matrix for delivered data
        self.time_to_delivery = 0                                           # time-to-go to delivery of freshest measurement
        self.data_in_delivery = False                                       # flag for data currently in processing/transmission
        self.data_delivered = False                                         # flag for completed data delivery to base station
        self.sampling_counter = 0
        self.first_meas = True                                              # to manage first transmission
        
        
    def copy(self):
        return Sensor(
            self.raw_delay, self.raw_V, self.proc_delay, self.proc_V, self.enabled, 
            self.C.copy(), self.raw_comm_delay, self.proc_comm_delay)
        
        
    def time_step(self):
        '''
        Runs one time step of simulation.
        If the sensor can acquire new data,
        simulates acquisition of a new sample and starts counter for delayed transmission (computation and communication).
        '''
        if self.sampling_counter == 0:
            if not self.first_meas:
                
                # transmit latest measurement
                self.data_in_delivery = True
                self.data_in_delivery_delay_info = self.curr_delay_info
                self.time_to_delivery = self.comm_delay
                
            else:

                # sample first measurement (no data are transmitted)
                self.first_meas = not(self.first_meas)

            # acquire new measurement
            self.curr_delay_info = self.delay_info
            self.sampling_counter = self.sample_time - 1
            
        else:

            # continue data acquisition/processing
            self.sampling_counter -= 1

        if self.data_in_delivery:
            
            # a measurement is currently being transmitted
            if self.time_to_delivery == 0:

                # the latest transmitted measurement is received
                self.data_in_delivery = False
                self.data_delivered = True
                self.delivered_delay_info = self.data_in_delivery_delay_info
                
            else:

                # continue ongoing transmission
                self.time_to_delivery -= 1


    def has_new_data(self) -> bool:
        '''
        Returns True if a new measurement has been received.
        '''
        return self.data_delivered
        
        
    def new_data(self) -> Tuple[int, np.ndarray]:
        '''
        Returns latest data transmitted.
        Raises BufferError if there are no new data to collect.
        
        Returns
        -------
        delivered_delay_info: tuple[int, np.array],
                    tuple with delay (computation plus communication) and information matrix of received measurement
         
        '''
        # new data can be collected if delivered and not yet collected
        if self.data_delivered:
            self.data_delivered = False
            return self.delivered_delay_info

        raise BufferError('No new data to collect')


    def set_proc(self):
        '''
        Sets sensor in processing mode.
        '''
        if not(self.enabled):
            self.enabled = True
            self.sample_time = self.proc_delay
            self.comm_delay = self.proc_comm_delay
            self.delay_info = self.proc_delay + self.proc_comm_delay, self.proc_info_mat


    def set_raw(self):
        '''
        Sets sensor in raw mode.
        '''
        if self.enabled:
            self.enabled = False
            self.sample_time = self.raw_delay
            self.comm_delay = self.raw_comm_delay
            self.delay_info = self.raw_delay + self.raw_comm_delay, self.raw_info_mat


    def reset(self):
        self.first_meas = True          # reset sampling
        self.sampling_counter = 0       # data being acquired are destroyed
        self.data_in_delivery = False   # data being transmitted are destroyed
        self.data_delivered = False     # data available for collection are destroyed