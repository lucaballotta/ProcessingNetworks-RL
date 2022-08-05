# ProcessingNetworks-RL
Code for simulation n. 1 in paper "To compute or not to compute? Adaptive smart sensing in resource-constrained edge computing" by L. Ballotta, G. Peserico, F. Zanini and P. Dini.

## How to run
To train a Q-learnig policy with new data:
1. copy files in "udpapp" folder and paste them in your omnet++ udpapp _$your_omnet_dir\samples\inet\src\inet\applications\udpapp_;
2. copy "MLForWirelessProject_eter2" folder and paste it in your omnet++ wireless example _$your_omnet_dir\samples\inet\examples\wireless_.

To open OMNeT++:
1. open folder _$your_omnet_dir\samples\inet\examples\wireless\MLForWirelessProject_eter2_;
2. run _omnetpp.ini_.

To choose whether to train a policy from scratch or to use a pre-trained policy,
you will want to set the following parameters in file _UdpEchoAppMLfW_eter2.cc_:

```
bool usepolicy_e = true;  //to use pre-trained policy
bool singleEpisode = true; // to run a single episode (to evaluate the policy)
int policy_e[5] = {89,89,88,71,70}; //to force a custom policy
```
