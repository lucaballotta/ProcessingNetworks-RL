from params_hom import *  # import parameters
from processingNetwork import ProcessingNetwork
from matplotlib import pyplot as plt
import os
import pickle
from utils import *


def testPolicy(net, policy):

    # q-table policy
    obs = net.reset()  # reset env
    epi_reward = 0  # set initial reward to 0
    done = False  # episode not done
    state_epi = []
    action_epi = []
    index = 0
    discount_factor = 1
    
    while not done:

        # Get action
        action = policy[obs]

        # Append current state
        state_epi.append(obs)

        # Append current action
        action_epi.append(action)

        # Apply environment transition
        new_obs, reward, done = net.step(action)

        # Compute return for this episode
        epi_reward += (discount_factor**index)*reward

        # Update state observation
        obs = new_obs

        index += 1
    
    state_epi.append(obs) 
    P_s = net.get_Ps()
    P_ma = []
    sum = 0
    for i in range(len(P_s)):
        sum += P_s[i]
        P_ma.append(sum/(i+1))
        
    return [epi_reward, state_epi, action_epi, P_s, P_ma]


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
   qlearning_policy = pickle.load(file)

### Run simulations
# Q-learning
[epi_reward, state_epi, action_epi, P_s, P_ma] = testPolicy(net, qlearning_policy)

# Static policies
all_proc = [(action_space_dim-1)]*state_space_dim
[epi_reward_all_proc, state_epi_all_proc, action_epi_all_proc, P_s_all_proc, P_ma_all_proc] = testPolicy(net, all_proc)
all_tx = [(N)]*state_space_dim
[epi_reward_all_tx, state_epi_all_tx, action_epi_all_tx, P_s_all_tx,P_ma_all_tx] = testPolicy(net, all_tx)

### Plot simulation results
# plt.style.use(['science','ieee'])
fig = plt.figure()

plt.plot([10*l for l in range(int(time_horizon))], P_s_all_proc,'r-', label = 'all-processing', linewidth = 1)
plt.plot([10*l for l in range(int(time_horizon))], P_ma_all_proc,'r--', label = 'MA all-processing', linewidth = 1)

plt.plot([10*l for l in range(int(time_horizon))], P_s_all_tx,'b-', label = 'all-raw', linewidth = 1)
plt.plot([10*l for l in range(int(time_horizon))], P_ma_all_tx,'b--', label = 'MA all-raw', linewidth = 1)

plt.plot([10*l for l in range(int(time_horizon))], P_s, 'y-', label = 'Q-learning (proposed)', linewidth = 1)
plt.plot([10*l for l in range(int(time_horizon))], P_ma,'y--', label = 'MA Q-learning (proposed)', linewidth = 1)

plt.axvline(x=int(0), color='k', linestyle='--', linewidth = 0.2)
for k in range(1,window_num+1):
    plt.axvline(x=10*int(window_length)*k, color='k', linestyle='--', linewidth = 0.2)

legend = plt.legend( shadow=True, fontsize='medium',frameon=True,loc = 'upper right')
plt.xlabel('Time[ms]')
plt.ylabel(r'Estimation error variance $\mathrm{Tr}(P_k)$')
# fig.savefig('comparison2.pdf',format='pdf')
plt.show()
plt.close()