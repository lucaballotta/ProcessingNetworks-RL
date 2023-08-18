# ProcessingNetworks-RL
Code for simulations in CDC'22 paper "A Reinforcement Learning Approach to Sensing Design in Resource-Constrained Wireless Networked Control Systems" by L. Ballotta, G. Peserico, and F. Zanini (folder "CDC paper") and in IEEE TNSE article "To Compute or not to Compute? Adaptive Smart Sensing in Resource-Constrained Edge Computing" by L. Ballotta, G. Peserico, F. Zanini, and P. Dini (folder "Journal paper").

## How to run
CDC paper: 
- to train a Q-learning sensing policy,
set desired parameters in _params_hom_ and run _qlearning_;
- to test and plot behavior of pre-trained and custom policies,
run _test_custom_policies_ and _compare_policies_.

Journal paper:
- Homogeneous sensors: same as CDC paper.
- Heterogeneous sensors:
  - To train a Q-learnig policy with new data:
    1. copy files in "udpapp" folder and paste them in your omnet++ udpapp _$your_omnet_dir\samples\inet\src\inet\applications\udpapp_;
    2. copy "MLForWirelessProject_eter2" folder and paste it in your omnet++ wireless example _$your_omnet_dir\samples\inet\examples\wireless_.

  - To open OMNeT++:
    1. open folder _$your_omnet_dir\samples\inet\examples\wireless\MLForWirelessProject_eter2_;
    2. run _omnetpp.ini_.

  - To choose whether to train a policy from scratch or to use a pre-trained policy,
  you will want to set the following parameters in file _UdpEchoAppMLfW_eter2.cc_:
  ``` 
  bool usepolicy_e = true;  //to use pre-trained policy
  bool singleEpisode = true; // to run a single episode (to evaluate the policy)
  int policy_e[5] = {89,89,88,71,70}; //to force a custom policy
  ```
  To train a Q-learnig policy with new data,
  run _qlearning_.

Training data are automatically saved in the folder "experiments".

## Link to papers
- CDC paper: https://arxiv.org/abs/2204.00703__  
  Please cite as: L. Ballotta, G. Peserico, F. Zanini,
  "A Reinforcement Learning Approach to Sensing Design in Resource-Constrained Wireless Networked Control Systems,"
  Proceedings of the IEEE Conference on Decision and Control (CDC), 2022.
- Journal article: https://arxiv.org/abs/2209.02166__  
  Please cite as: L. Ballotta, G. Peserico, F. Zanini, P. Dini,
  "To Compute or not to Compute? Adaptive Smart Sensing in Resource-Constrained Edge Computing,"
  in IEEE Transactions on Network Science and Engineering, 2023.
