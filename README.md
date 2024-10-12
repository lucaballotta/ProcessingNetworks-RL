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
- Conference paper: https://ieeexplore.ieee.org/document/9993151

  Freely available at https://arxiv.org/abs/2204.00703
  
  Citation:
  ```
  @INPROCEEDINGS{9993151,
    author={Ballotta, Luca and Peserico, Giovanni and Zanini, Francesco},
    booktitle={2022 IEEE 61st Conference on Decision and Control (CDC)}, 
    title={A Reinforcement Learning Approach to Sensing Design in Resource-Constrained Wireless Networked Control Systems}, 
    year={2022},
    volume={},
    number={},
    pages={3914-3919},
    doi={10.1109/CDC51059.2022.9993151}
  }
  ```
  
- Journal article (open access): https://ieeexplore.ieee.org/document/10225419
  
  Citation:
  ```
  @ARTICLE{10225419,
    author={Ballotta, Luca and Peserico, Giovanni and Zanini, Francesco and Dini, Paolo},
    journal={IEEE Transactions on Network Science and Engineering}, 
    title={To Compute or Not to Compute? Adaptive Smart Sensing in Resource-Constrained Edge Computing}, 
    year={2024},
    volume={11},
    number={1},
    pages={736-749},
    doi={10.1109/TNSE.2023.3306202}
  }
  ```
