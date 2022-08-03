import os
import pickle
from processingNetwork import ProcessingNetwork
import matplotlib.pyplot as plt
import numpy as np
import pickle
from params_hom import *
from utils import *


### Create network
dir_path = path()
with open(os.path.join(dir_path, 'uniform_discretization.pickle'), 'rb') as file:
    bins = pickle.load(file)

net = ProcessingNetwork(sensors, A, Q, window_length, window_num)
net.set_discretization(bins)
state_space_dim = net.state_dim
action_space_dim = net.action_dim

### Load pre-trained policy
with open('qtable_policy.pickle','rb') as file:
    pi_qlearning = pickle.load(file)

print('Q-learning policy:',pi_qlearning)

err_init = np.trace(net.P0)
trP_pi_qlearning = [err_init]
trP_pi_custom = [[]] * action_space_dim

### Run simulations
# Q-learning
obs = net.reset()
state_epi = [obs]
action_epi = []
for window in range(window_num):
    action = pi_qlearning[obs]
    action_epi.append(action)
    obs, _, _ = net.step(action)
    state_epi.append(obs)

print(f'States with q-learning policy: {state_epi}')
print(f'Actions with q-learning policy: {action_epi}')

trP_pi_qlearning.extend(net.get_Ps())
MA_qlearning = []
for i in range(len(trP_pi_qlearning)):
    MA_qlearning.append(sum(trP_pi_qlearning[:i+1])/(i+1))
    
trP_avg_pi_qlearning = np.mean(trP_pi_qlearning)

# Static policies
trP_avg_pi_custom = [0] * action_space_dim
MA_pi_custom = [] 
for action in range(action_space_dim):    
    net.reset()
    for window in range(window_num):
        net.step(action)

    trP_pi_action = [err_init]; trP_pi_action.extend(net.get_Ps())
    trP_pi_custom[action] = trP_pi_action
    trP_avg_pi_custom[action] = np.mean(trP_pi_custom[action])

for action in range(action_space_dim): 
    Ma_list = [trP_pi_custom[action][0]] 
    for i in range(1,len(trP_pi_custom[action])):
        val = (i*Ma_list[i-1] + trP_pi_custom[action][i])/(i+1)
        Ma_list.append(val)
    MA_pi_custom.append(Ma_list)

### Plot simulation results
print('Cost with Q-learning policy:', trP_avg_pi_qlearning)
for action in range(action_space_dim):
    print(f'Cost with static policy All-{action}:', trP_avg_pi_custom[action])
    
static_to_plot = [0, 4] # static policies to be plotted
# plt.style.use(['science','ieee'])
plt.xlabel("$k$")
plt.ylabel("$trace(P_k)$")
plt.grid(b=True, linestyle='-')
plt.plot(trP_pi_qlearning, label='Q-learning', linestyle='-',linewidth=0.5)
plt.plot(MA_qlearning, label='MA Q-learning', linestyle='-.',linewidth=0.5)
for action in static_to_plot:
    plt.plot(trP_pi_custom[action], label = f'All-{action}', linestyle='-',linewidth=0.5)
    plt.plot(MA_pi_custom[action], label = f'MA All-{action}', linestyle='-.',linewidth=0.5)

for k in range(window_num):
    plt.axvline(x=int(window_length)*k, color='tab:gray',linewidth=0.5)

plt.legend(shadow=True, fontsize='small',frameon=True,loc = 'upper right',framealpha=0.8)
# plt.savefig(f'Comparison.pdf')
plt.show()
plt.close()