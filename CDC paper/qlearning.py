import os
import pickle
from datetime import datetime
from processingNetwork import ProcessingNetwork
from tqdm import trange
import matplotlib.pyplot as plt
from utils import *
from params_hom import *


if __name__ == '__main__':

    # create directory for experiments
    dir_path = path()

    # create directory with current experiment
    curr_path = path(os.path.join(dir_path, datetime.now().strftime('%Y-%m-%d_%H-%M')))

    # create processing network
    net = ProcessingNetwork(sensors, A, Q, window_length, window_num)
    
    # compute or load discretization
    print('Discretizing state space...')
    bins = net.get_uniform_quantization(dir_path)
    print(f'Discretization: {bins}')
    state_space_dim = net.state_dim
    action_space_dim = net.action_dim

    # set learning hyperparameters
    print('Enter number of episodes:')
    episodes = int(float(input()))
    epsilon_ini = epsilon = 0.9
    epsilon_min = 0.1
    learning_rate = 0.01
    discount_factor = 1
    
    # create zero q-table and counter and set 'store' episodes to be stored
    store_every = episodes//5
    qtable = -40*load_qtable(dir_path, state_space_dim, action_space_dim, table='ones')
    qcounter = load_qtable(dir_path, state_space_dim, action_space_dim)
    qtable_story = [[[] for _ in range(action_space_dim)] for _ in range(state_space_dim)]
    
    # create dictionary for plot: episode is key, (trace(Ps), states, actions) is value
    dict_for_plot = {}

    # track rewards and Q-table history
    all_rewards = [[[] for _ in range(action_space_dim)] for _ in range(state_space_dim)]
    track_actions = [[[] for _ in range(action_space_dim)] for _ in range(state_space_dim)]
    counter_actions = 0

    # run Q-learning
    print('Running the Q-learning...')
    for episode in trange(episodes):
        obs = net.reset()
        epi_reward = 0
        done = False
        state_epi = []
        action_epi = []
        while not done:

            # get action
            if np.random.random() > epsilon:  # epsilon-greedy policy
                action = np.argmax(qtable[obs])

            else:
                action = np.random.randint(0, action_space_dim)

            state_epi.append(obs)
            action_epi.append(action)
            
            # run environment transition
            new_obs, reward, done = net.step(action)
            
            # update return
            epi_reward += discount_factor*reward

            # update rewards history
            counter_actions += 1
            for state_iter in range(state_space_dim):
                if obs == state_iter:
                    all_rewards[state_iter][action].append(reward)
                    track_actions[state_iter][action].append(counter_actions)

            # update Q-table
            if done:
                qtable[obs, action] = (1 - learning_rate) * qtable[obs, action] + learning_rate * reward
            else:
                qtable[obs, action] = (1 - learning_rate) * qtable[obs, action] + learning_rate * \
                                        (reward + discount_factor * max(qtable[new_obs]))

            # update Q-table history
            qtable_story[obs][action].append(qtable[obs, action])
            
            # update counter
            qcounter[obs, action] += 1
            
            # rewrite obs with new_obs (Markov step)
            obs = new_obs
            
        # update states
        state_epi.append(obs)

        # epsilon decay (every episode) following Monro-Robbins conditions
        epsilon = max(epsilon_ini/np.sqrt(episode+1), epsilon_min)

        # update dictionary for plots
        dict_for_plot[episode] = (net.get_Ps(), state_epi, action_epi)

        # store every 'store_every' episodes
        if not (episode+1) % store_every:
            store_qtable(curr_path, qtable, 'qtable_', episode)
            store_qtable(curr_path, qcounter, 'qcounter_', episode)
        
    # store Q-tables at the end of training
    store_qtable(curr_path, qtable, 'qtable_', episodes-1)
    store_qtable(curr_path, qcounter, 'qcounter_', episodes-1)
    qpolicy = [np.argmax(state) for state in qtable]
    print(f'Optimal policy according to terminal Qtable: {qpolicy}')
    with open(os.path.join(curr_path,'qtable_policy.pickle'),'wb') as file:
        pickle.dump(qpolicy,file)

    # save training data for plots
    with open(os.path.join(curr_path, 'plotting_dict.pickle'), 'wb') as file:
        pickle.dump(dict_for_plot, file)

    # plot rewards and history of Q-table
    labels = [str(l) for l in range(action_space_dim)]
    for state_iter in range(state_space_dim):
        for act in range(action_space_dim):
            plt.scatter(track_actions[state_iter][act], all_rewards[state_iter][act], label=labels[act])
        plt.title(f'Rewards_{state_iter}.pdf')
        plt.legend()
        plt.savefig(os.path.join(curr_path, f'Rewards_{state_iter}.pdf'))
        plt.close() 
   
    for state_iter in range(state_space_dim):
        for action_iter in range(action_space_dim):
            plt.plot(track_actions[state_iter][action_iter], qtable_story[state_iter][action_iter],\
                label=f'qtable_state{state_iter}_action{action_iter}')
        plt.title(f'Story state {state_iter}.pdf')
        plt.legend()
        plt.savefig(os.path.join(curr_path, f'qtable_state{state_iter}_.pdf'))
        plt.close()