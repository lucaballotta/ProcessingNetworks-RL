import os
import pickle
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

### Utility functions to store and plot data


def path(directory: str='experiments', verbosity: int=0):
    code_path = os.getcwd()
    storing_path = os.path.join(code_path, directory)
    if not os.path.isdir(storing_path):
        if verbosity > 0:
            print(f'Creating {directory} directory...')
        os.mkdir(storing_path)
    return storing_path


def store_qtable(where: str, table: np.ndarray, name: str, when: str, verbosity: int=0):
    if verbosity > 0:
        print(f'Storing {name}...')
    with open(os.path.join(where, name + f'{when}.pickle'), 'wb') as door:
        pickle.dump(table, door)


def load_qtable(gen_dir: str, states: int, actions: int, table: str='zeros'):
    if table == 'zeros':
        try:
            with open(os.path.join(gen_dir, 'zero_qtables.pickle'), 'rb') as door:
                dict_tables = pickle.load(door)
            return dict_tables[(states, actions)]
        except FileNotFoundError:
            dict_tables = {}
            with open(os.path.join(gen_dir, 'zero_qtables.pickle'), 'wb') as door:
                pickle.dump(dict_tables, door)
            print('I know, it will generate an error.. But be faithful and try again! I did not want'
                  ' to code a better logic!')
        except KeyError:
            zero_qtable = np.zeros((states, actions))
            dict_tables[(states, actions)] = zero_qtable
            with open(os.path.join(gen_dir, 'zero_qtables.pickle'), 'wb') as door:
                pickle.dump(dict_tables, door)
            return zero_qtable
    elif table == 'random':
        try:
            with open(os.path.join(gen_dir, 'random_qtables.pickle'), 'rb') as door:
                dict_tables = pickle.load(door)
            return dict_tables[(states, actions)]
        except FileNotFoundError:
            dict_tables = {}
            with open(os.path.join(gen_dir, 'random_qtables.pickle'), 'wb') as door:
                pickle.dump(dict_tables, door)
            print('I know, it will generate an error.. But be faithful and try again! I did not want'
                  ' to code a better logic!')
        except KeyError:
            random_qtable = np.random.uniform((states, actions))
            dict_tables[(states, actions)] = random_qtable
            with open(os.path.join(gen_dir, 'random_qtables.pickle'), 'wb') as door:
                pickle.dump(dict_tables, door)
            return random_qtable
    if table == 'ones':
        try:
            with open(os.path.join(gen_dir, 'ones_qtables.pickle'), 'rb') as door:
                dict_tables = pickle.load(door)
            return dict_tables[(states, actions)]
        except FileNotFoundError:
            dict_tables = {}
            with open(os.path.join(gen_dir, 'ones_qtables.pickle'), 'wb') as door:
                pickle.dump(dict_tables, door)
            print('I know, it will generate an error.. But be faithful and try again! I did not want'
                  ' to code a better logic!')
        except KeyError:
            ones_qtable = np.ones((states, actions))
            dict_tables[(states, actions)] = ones_qtable
            with open(os.path.join(gen_dir, 'ones_qtables.pickle'), 'wb') as door:
                pickle.dump(dict_tables, door)
            return ones_qtable
    else:
        try:
            with open(table + '.pickle', 'rb') as door:
                dict_tables = pickle.load(door)
            return dict_tables[(states, actions)]
        except FileNotFoundError:
            print('Wrong path or file or name!')


def plot_qtable(table: np.ndarray, name: str, states: int, actions: int, savetype: str='pdf'):
    ax = sns.heatmap(table, xticklabels=range(actions), yticklabels=range(states))
    ax.set_ylabel('State')
    ax.set_xlabel('Action')
    if savetype is not None:
        if savetype == 'pdf':
            plt.savefig(name + '.' + savetype)
        elif savetype == 'png':
            plt.savefig(name + '.' + savetype, dpi=300)
        plt.close()
    else:
        plt.show()