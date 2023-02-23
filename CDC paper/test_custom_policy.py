from params_hom import *
from processingNetwork import ProcessingNetwork
from matplotlib import pyplot as plt
import pickle
from utils import *

# Load pretraining policy
with open('qtable_policy.pickle','rb') as file:
   custom_policy = pickle.load(file)

print('Q-table policy:', custom_policy)

dir_path = path()
with open(os.path.join(dir_path, 'uniform_discretization.pickle'), 'rb') as file:
    bins = pickle.load(file)

# Instantiate processing network
net = ProcessingNetwork(sensors, A, Q, window_length, window_num)
net.set_quantization(bins)
state_space_dim = net.state_dim
action_space_dim = net.action_dim

# Run one episode (horizon K)
obs = net.reset()  # reset env
epi_reward = 0  # set initial reward to 0
done = False  # episode not done
state_epi = []
action_epi = []
index = 0
discount_factor = .99

while not done:

    # Get action
    action = custom_policy[obs]

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

# Visualize return
print(f'Return for this episode: {epi_reward}')
for i in range(10):
    print(f'Mean of the {i+1} window: {np.mean(-net.get_Ps()[(int(window_length)*i):(i+1)*int(window_length)])}')

# Visualize visited states
print(f'States at custom episode: {state_epi}')

# Visualize applied actions
print(f'Actions at custom episode: {action_epi}')

# Plot tr(P) along episode
plt.plot([l for l in range(int(time_horizon))], net.get_Ps())
for k in range(window_num):
    plt.axvline(x=int(window_length)*k, color='k', label='axvline - full height')

for kk in bins:
    plt.axhline(y=kk, color='r', linestyle='-')

plt.show()
plt.close()