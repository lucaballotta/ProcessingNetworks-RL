import numpy as np

class Sensor:
    
    def __init__(self, raw_delay, raw_V, proc_delay, proc_V, enabled, C, raw_comm_del, proc_comm_del):
        self.raw_delay = raw_delay
        try:
            self.raw_V = raw_V.copy()
            raw_V_inv = np.linalg.inv(raw_V)
            self.raw_info_mat = np.matmul(np.transpose(C),np.matmul(raw_V_inv,C))
            
        except:
            self.raw_V = raw_V
            self.raw_info_mat = np.matmul(np.transpose(C),C) / raw_V
            
        self.proc_delay = proc_delay
        try:
            self.proc_V = proc_V.copy()
            proc_V_inv = np.linalg.inv(proc_V)
            self.proc_info_mat = np.matmul(np.transpose(C),np.matmul(proc_V_inv,C))
            
        except:
            self.proc_V = proc_V
            self.proc_info_mat = np.matmul(np.transpose(C),C) / proc_V
        
        self.enabled = enabled                                              # true if processing
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
        self.data_in_delivery = False                                       # flag for data currently in processing/tranmission
        self.data_delivered = False                                         # flag for completed data delivery to base station
        self.sampling_counter = 0
        self.first_meas = True                                              # to manage first transmission
        
        
    def copy(self):
        return Sensor(self.raw_delay, self.raw_V, self.proc_delay, self.proc_V, self.enabled, self.C.copy(), \
            self.raw_comm_delay, self.proc_comm_delay)
        
        
    ### Collect measurement, process and/or transmit
    def time_step(self):

        if self.sampling_counter == 0:
            
            if not self.first_meas:
                
                # transmit acquired measurement
                self.data_in_delivery = True
                self.data_in_delivery_delay_info = self.curr_delay_info
                self.time_to_delivery = self.comm_delay
                
            else:

                # no measurement available at start
                self.first_meas = not(self.first_meas)

            # acquire new measurement
            self.curr_delay_info = self.delay_info
            self.sampling_counter = self.sample_time - 1
            
        else:

            # continue ongoing acquisition
            self.sampling_counter -= 1

        if self.data_in_delivery:
            if self.time_to_delivery == 0:

                # measurement ready to be collected
                self.data_in_delivery = not(self.data_in_delivery)
                self.delivered_delay_info = self.data_in_delivery_delay_info
                self.data_delivered = True
                
            else:

                #continue ongoing transmission
                self.time_to_delivery -= 1


    def has_new_data(self):
        return self.data_delivered
        
        
    def new_data(self):

        # new data can be collected if delivered and not yet collected
        if self.data_delivered:
            self.data_delivered = False
            return self.delivered_delay_info

        raise BufferError('No new data to collect')


    def enable(self):
        if not(self.enabled):
            self.enabled = True
            self.sample_time = self.proc_delay
            self.comm_delay = self.proc_comm_delay
            self.delay_info = self.proc_delay + self.proc_comm_delay, self.proc_info_mat


    def disable(self):
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