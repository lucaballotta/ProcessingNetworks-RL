# ProcessingNetworks-RL
Code for simulations in paper "To compute or not to compute? Adaptive smart sensing in resource-constrained edge computing" by L. Ballotta, G. Peserico, F. Zanini and P. Dini.

## How to run

To train a Q-learnig policy with new data:
1. copy files in "udpapp" folder and paste them in your omnet++ udpapp ($your_omnet_dir\samples\inet\src\inet\applications\udpapp);
2. copy "MLForWirelessProject_eter2" folder and paste it in your omnet++ wireless example ($your_omnet_dir\samples\inet\examples\wireless)

To open omnet++,
open folder $your_omnet_dir\samples\inet\examples\wireless\MLForWirelessProject_eter2
you can run omnetpp.ini

to choose if train the system or use only the policy, in file UdpEchoAppMLfW_eter2.cc there are the following parameters:

'''
bool usepolicy_e = true;  //to use pre-trained policy
bool singleEpisode = true; // to run a single episode (to evaluate the policy)
int policy_e[5] = {89,89,88,71,70}; //to force a custom policy
'''
