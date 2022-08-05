//
// Copyright (C) 2011 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
#include <math.h>
#include <chrono>
#include <thread>
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/networklayer/common/HopLimitTag_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/icmpv6/Icmpv6.h"
#include "inet/networklayer/icmpv6/Ipv6NeighbourDiscovery.h"
#include "inet/networklayer/ipv6/Ipv6Header.h"
#include "inet/networklayer/ipv6/Ipv6InterfaceData.h"
#include "inet/networklayer/ipv6/Ipv6RoutingTable.h"
#include "UdpEchoAppMLfW_eter2.h"
#include "UdpEchoAppMLfW_eter2.h"

namespace inet {

Define_Module(UdpEchoAppMLfW_eter2);


//global variable
//extern
int numberOfNodeWhichHaveToElaborate_eter;
extern int numberOfNodeWhichHaveToElaborate_group1;
extern int numberOfNodeWhichHaveToElaborate_group2;
extern int numberOfNodeWhichHaveToElaborate_group3;
//extern

int numberOfNodeWhichHaveToSend_eter;
extern int numberOfNodeWhichHaveToSend_group1;
extern int numberOfNodeWhichHaveToSend_group2;
extern int numberOfNodeWhichHaveToSend_group3;
int numberOfNodeSentmessage_g1 = 0;
int numberOfNodeSentmessage_g2 = 0;
int numberOfNodeSentmessage_g3 = 0;
int numberOfNodeSentmessage_group1 = 0;
int numberOfNodeSentmessage_group2 = 0;
int numberOfNodeSentmessage_group3 = 0;
// da capire se servono
int numberOfElaboration_g1 = 0;
int numberOfElaboration_g2 = 0;
int numberOfElaboration_g3 = 0;
int numberOfSending_g1 = 0;
int numberOfSending_g2 = 0;
int numberOfSending_g3 = 0;
int numberOfReceivedPacket_inthisWindow_eter = 0;
int numberOfReceivedElaboratedBytes_inthisWindow_eter = 0;
int numberOfSentPacket_inthisWindow_g1 = 0; //by edge nodes
int numberOfSentElaboratedPacket_inthisWindow_g1 = 0; //by edge nodes
int numberOfSentPacket_inthisWindow_g2 = 0; //by edge nodes
int numberOfSentElaboratedPacket_inthisWindow_g2 = 0; //by edge nodes
int numberOfSentPacket_inthisWindow_g3 = 0; //by edge nodes
int numberOfSentElaboratedPacket_inthisWindow_g3 = 0; //by edge nodes
/////////////////////////////////////
int numberOfWindowsForThisEpisode_eter = 0;
int numberOfWindowsForEpisode_eter = 10;
int curr_numberOfEpisode_eter = 0;
int numberOfEpisode_eter = 108;
float delta_e = 0.001;
int numberOfNode2_e = 7;
simtime_t previousSampleTime_e;
simtime_t tauAr_e[16] = {};
float sigma_e = 100;
float sigma_e2 = 4.5;
float currentP11_e = 3.75;
float currentP12_e = 0.;
float currentP21_e = 0.;
float currentP22_e = 3.75;
float prediction11_e[16]= {currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e};
float prediction12_e[16]= {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
float prediction21_e[16]= {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
float prediction22_e[16]= {currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e,currentP11_e};
float elaboratedAr_e[16]= {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
float meanCurrP_e = 0.;
float mean_prediction11_e= 0;
float  mean_prediction22_e = 0;
float previous_meanTrace_e = 20;
int counterPred_e = 0;
int countT_e = 0;
bool resHaveToela_g1 = false;
bool resHaveToela_g2 = false;
bool resHaveToela_g3 = false;
float previousP11W_e = 10.;
float previousP22W_e = 10.;
int countState0_e = 0;
int countState1_e = 0;
int countState2_e = 0;
int countState3_e = 0;
int countState4_e = 0;
int countState5_e = 0;
bool isElaborating_e = false;
int messagesToElaborate_e = 1;
simtime_t previousSampleElaboratingTime_e;
float timeToElaborate_e = 0;
float costTime_e = 3.;
int positionMessages_e[16]= {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
//269,225, 229, 197,  191}//197,225, 229, 196,  191//197,225, 196, 191,  190
int policy_e[5] = {89,89,88,71,70};//{71,99,70,65,64}; //89,89,88,71,70
//{197,225, 196, 191,  190};
bool usepolicy_e = true;
bool singleEpisode = true;
float r_ela_uwb = 11;
float r_ela_cam = 80;
float r_ela_lid = 82.;
float r_uwb = 10;
float r_cam = 15;
float r_lid = 50.;
int currentAction_ind=0;


//initialization
void UdpEchoAppMLfW_eter2::initialize(int stage)
{
    ApplicationBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        // init statistics
        numEchoed = 0;
        WATCH(numEchoed);
        //initialization of the Q Action value table to 1. such that each action is explore
        for(int i=0;i<5;i++)
        {
            for(int j=0;j<108;j++)
            {
                QStateTable[i][j] = 0.;
            }
        }
        for(int i=0;i<16;i++)
        {
            tauAr_e[i] = SIMTIME_ZERO;
        }
        previousSampleTime_e = SIMTIME_ZERO;
        previousSampleElaboratingTime_e = SIMTIME_ZERO;
        if(usepolicy_e)
        {
            currentAction = policy_e[0];
        }
        currentState = 0;
        recordScalar("state", currentState);
        recordScalar("action", currentAction);
        WATCH(G);
        WATCH(previousAction);
        WATCH(currentState);
        WATCH(QStateTable);
        WATCH(currentAction);
        WATCH(counterStepForEpisode);
        WATCH(timer);
        WATCH(interval);
        WATCH(counterEpisodes);
        WATCH(G);
        WATCH(alpha);
        WATCH(gamma);
        selfMsg = new cMessage("sendTimer");
    }
    resetTimer();
}

// input: /
// return random action
int UdpEchoAppMLfW_eter2::getRandomAction(int state)
{
    //action -1,0,1
    /*
    if(state==0)
        return ((rand()%2)+1);
    else if(state==8)
        return (rand()%2);
    else
        return (rand()%3);
    */
    //action correspond to the number of nodes which have to elaborate
    //return (rand()%9);
    //action correspond to the number of nodes which have to elaborate and the number of node which have to sleep
    return (rand()%108);
}

// input: int state for which the best action is wanted
// return int maxPos, which is the index of the best action for the state according the Q table value
int UdpEchoAppMLfW_eter2::getBestAction(int state)
{
    //action -1,0,1
    /*
    int initial = 0;
    int final = 3;
    if (state==0)
        initial = 1;
    else if(state==8)
        final = 2;
    float maxValue = QStateTable[state][initial];
    int maxPos = initial;
    for(int i=initial+1;i<final;i++)
    {
        if(QStateTable[state][i]>maxValue)
        {
            maxValue = QStateTable[state][i];
            maxPos = i;
        }
    }*/
    //action correspond to the number of nodes which have to elaborate
    /*
    float maxValue = QStateTable[state][0];
    int maxPos = 0;
    for(int i=1;i<9;i++)
    {
        if(QStateTable[state][i]>maxValue)
        {
            maxValue = QStateTable[state][i];
            maxPos = i;
        }
    }*/
    //action correspond to the number of nodes which have to elaborate and the number of node which have to sleep
    float maxValue = QStateTable[state][0];
    int maxPos = 0;
    for(int i=1;i<108;i++)
    {
        if(QStateTable[state][i]>maxValue)
        {
            maxValue = QStateTable[state][i];
            maxPos = i;
        }
    }
    return maxPos;
}

// input: int state for which the action is wanted to be chosen
// return int action, which is the index of the best action for the state according the Q table value with probabilit of 90%, or a random action otherwise
int UdpEchoAppMLfW_eter2::epsilon_greedy(int state)
{
    int action;
    if((rand()%1000)< (int)(epsilon*1000))
    {
        action =  getRandomAction(state);
    }
    // choice of the current best action for the state
    else
    {
        action = getBestAction(state);
    }
    return action;
}

// input: /
// return int currentAction, which is theaction selected using  q-learning algorithm
int UdpEchoAppMLfW_eter2::improvePolicy()
{
    auto currT = simTime();
    if(currT>1.1)
    {
        int nextState;
        float currentTrace = currentP11_e + currentP22_e;
        //recordScalar("currT",currentTrace);
        // bins [2.70213174819945, 3.030921459198, 3.9860602617264, 4.25996160507205]
        bool nextStateFound = false;
        for(int m=3;m>=0;m--)
        {
            if(currentTrace>statesDiscretization[m])
            {
                nextState = 3-m;
                nextStateFound = true;
                break;
            }
        }
        if(!nextStateFound)
            nextState = 4;
        /*
        if(currentTrace>5.0260555744171)
        {
            nextState = 0;
            //countState0_e++;
        }
        else if(currentTrace>4.79360795021055)
        {
            nextState = 1;
            //countState1_e++;
        }
        else if(currentTrace>3.6396869421005498)
        {
            nextState = 2;
            //countState2_e++;
        }
        else if(currentTrace>3.4507528543471997)
        {
            nextState = 3;
            //countState3_e++;
        }
        else if(currentTrace>3.0682737827301)
        {
            nextState = 4;
            //countState4_e++;
        }
        else
        {
            nextState = 5;
            //countState5_e++;
        }*/
        float reward = 0.;
        if(!usepolicy_e)
        {
            //reward = (-meanCurrP_e/((float)(countT_e)) + (previousP11W_e+previousP22W_e))/(previousP11W_e+previousP22W_e);// -1.;
            if(countT_e>0)
            {
                reward = -meanCurrP_e/((float)(countT_e));//(-meanCurrP_e/((float)(countT_e)) + previous_meanTrace_e)/(previous_meanTrace_e);
                if(reward>0)
                {
                    recordScalar("WARNING:err",1);
                    reward = -20;
                }
                else if(reward<-100)
                {
                    recordScalar("WARNING3:err",3);
                    reward = -40;
                }
            }
            else
            {
                recordScalar("WARNING2:err",2);
                reward = -20;
            }
            numberOfWindowsForThisEpisode_eter++;
            //recordScalar("reward", reward);
            if(numberOfWindowsForThisEpisode_eter<numberOfWindowsForEpisode_eter)
            {
                //recordScalar("reward", reward);
                /*recordScalar("countT_e", countT_e);
                recordScalar("previousP11W_e", previousP11W_e);
                recordScalar("previousP22W_e", previousP22W_e);*/
                //reward = -mean_prediction11_e-mean_prediction22_e;
                // calculate A_next according QTable
                int bestAction = getBestAction(nextState);
                //int nextAction = epsilon_greedy(currentState);
                //update of QTable
                QStateTable[currentState][currentAction] += alpha * (reward + gamma*QStateTable[nextState][bestAction] - QStateTable[currentState][currentAction]);
                // A = A_next, S = S-next
                currentState = nextState;
                currentAction = epsilon_greedy(currentState);
                //float trac = currentP11_e + currentP22_e;
                //recordScalar("trac", trac);
                //currentAction = nextAction;
                // update discount
                G += reward;
            }
            else
            {
                QStateTable[currentState][currentAction] += alpha * (reward - QStateTable[currentState][currentAction]);
                numberOfWindowsForThisEpisode_eter = 0;
                curr_numberOfEpisode_eter++;
                //epsilon = epsilon;
                //recordScalar("curr_numberOfEpisode_eter", curr_numberOfEpisode_eter);
                //recordScalar("epsilon", epsilon);
                currentState = 0;
                currentAction = getRandomAction(currentState);
                for(int i=0;i<16;i++)
                {
                    tauAr_e[i] = SIMTIME_ZERO;
                    prediction11_e[i]= 10.;
                    prediction12_e[i]= 0.;
                    prediction21_e[i]= 0.;
                    prediction22_e[i]= 10.;
                    elaboratedAr_e[i]= 1.;
                }
                currentP11_e = 10.;
                currentP22_e = 10.;
            }
        }
        else
        {
            float toSave = -meanCurrP_e;//((float)(countT_e));
            //recordScalar("rew",toSave);
            currentState = nextState;
            currentAction = policy_e[currentState];
            //recordScalar("astate",currentState);
            //recordScalar("action",currentAction);
            auto currT = simTime();
            reward = -meanCurrP_e/((float)(countT_e));//(-meanCurrP_e/((float)(countT_e)) + previous_meanTrace_e)/(previous_meanTrace_e);
            //recordScalar("trac", reward);
            //recordScalar("actiontime", currT.dbl());
            //for discretization
            numberOfWindowsForThisEpisode_eter++;
            if(numberOfWindowsForThisEpisode_eter<numberOfWindowsForEpisode_eter)
            {
                currentState = nextState;
                currentAction = policy_e[currentState];
                float toSave = -meanCurrP_e;
                //recordScalar("rew",toSave);
                float trac = currentP11_e + currentP22_e;
                recordScalar("trac", trac);
                //recordScalar("state", currentState);
                //recordScalar("action", currentAction);

            }
            else
            {
                float trac = currentP11_e + currentP22_e;
                recordScalar("trac", trac);
                numberOfWindowsForThisEpisode_eter = 0;
                currentAction_ind++;
                recordScalar("ind",currentAction_ind);
                currentState = rand()%5;
                if(singleEpisode)
                    finish();
                if(currentAction_ind==numberOfEpisode_eter)
                {
                    finish();
                }
                for(int j=0;j<5;j++)
                {
                    policy_e[j] = currentAction_ind;
                }
                currentAction = policy_e[currentState];
                recordScalar("ind_currAction",currentAction);
                for(int i=0;i<16;i++)
                {
                    tauAr_e[i] = SIMTIME_ZERO;
                    prediction11_e[i]= 10.;
                    prediction12_e[i]= 0.;
                    prediction21_e[i]= 0.;
                    prediction22_e[i]= 10.;
                    elaboratedAr_e[i]= 1.;
                }
                currentP11_e = 10.;
                currentP22_e = 10.;
            }
        }
        float temp_prev_meanTrace = previous_meanTrace_e;
        previous_meanTrace_e = meanCurrP_e/((float)(countT_e));
        if(previous_meanTrace_e<0)
        {
            previous_meanTrace_e = temp_prev_meanTrace;
        }

        // do action A
        mean_prediction11_e = 0;
        mean_prediction22_e = 0;
        counterPred_e = 0;
        resHaveToela_g1 = true;
        resHaveToela_g2 = true;
        resHaveToela_g3 = true;
    }
    int mappedNumber = actionMap[currentAction];
    int group1_action = mappedNumber%100;
    int group2_action = (int)((mappedNumber%10000 - group1_action)/100);
    int group3_action = (int)((mappedNumber-group2_action - group1_action)/10000);
    numberOfNodeWhichHaveToSend_group1 = floor(group1_action/10);
    numberOfNodeWhichHaveToElaborate_group1 = group1_action%10;
    numberOfNodeWhichHaveToSend_group2 = floor(group2_action/10);
    numberOfNodeWhichHaveToElaborate_group2 = group2_action%10;
    numberOfNodeWhichHaveToSend_group3 = floor(group3_action/10);
    numberOfNodeWhichHaveToElaborate_group3 = group3_action%10;
    return currentAction;
}

// input: /
// return /, it calls improvepolicy_e() to execute q-learning algorithm and then it resets the statistics
void UdpEchoAppMLfW_eter2::makeDecision()
{
    //make the RL algorithm and choose the number of nodes which have to elaborate data
    //numberOfNodeWhichHaveToElaborate_eter += (improvepolicy_e()-1);
    improvePolicy();
    //Once made the choise, reset window variable
    numberOfNodeSentmessage_g1 = 0;
    numberOfElaboration_g1 = 0;
    numberOfNodeSentmessage_g2 = 0;
    numberOfElaboration_g2 = 0;
    numberOfNodeSentmessage_g3 = 0;
    numberOfElaboration_g3 = 0;
    numberOfSending_g1 = 0;
    numberOfSending_g2 = 0;
    numberOfSending_g3 = 0;
    numberOfReceivedPacket_inthisWindow_eter = 0;
    numberOfReceivedElaboratedBytes_inthisWindow_eter = 0;
    numberOfSentPacket_inthisWindow_g1 = 0;
    numberOfSentElaboratedPacket_inthisWindow_g1 = 0;
    numberOfSentPacket_inthisWindow_g2 = 0;
    numberOfSentElaboratedPacket_inthisWindow_g2 = 0;
    numberOfSentPacket_inthisWindow_g3 = 0;
    numberOfSentElaboratedPacket_inthisWindow_g3 = 0;
    meanCurrP_e = 0;
    auto currT = simTime();
    countT_e = 0;
    previousP11W_e = currentP11_e;
    previousP22W_e = currentP22_e;
    //recordScalar("time", currT.dbl());

}

// input: cMessage *msg
// return /, it handle received messages. Internal Omnet++ function, it was not modified.
void UdpEchoAppMLfW_eter2::handleMessageWhenUp(cMessage *msg)
{
    if (simTime() - timer >= interval)
    {
        makeDecision();
        resetTimer();
    }
    if(msg->isSelfMessage())
    {
        float *openLoopTemp = updateOpenLoopPrediction(currentP11_e,currentP12_e,currentP21_e,currentP22_e,1);
        currentP11_e = openLoopTemp[0];
        currentP12_e = openLoopTemp[1];
        currentP21_e = openLoopTemp[2];
        currentP22_e = openLoopTemp[3];
        if(isnan(currentP11_e))
            currentP11_e = 0.;
        if(isnan(currentP22_e))
            currentP22_e = 0.;
        if(isinf(currentP11_e))
        {
            if(currentP11_e==-std::numeric_limits<float>::infinity())
                currentP11_e = -1000000.;
            else
                currentP11_e = 1000000.;
        }
        if(isinf(currentP22_e))
        {
            if(currentP22_e==-std::numeric_limits<float>::infinity())
                currentP22_e = -1000000.;
            else
                currentP22_e = 1000000.;
        }
        if(isnan(currentP12_e))
            currentP12_e = 0.;
        if(isnan(currentP21_e))
            currentP21_e = 0.;
        if(isinf(currentP12_e))
        {
            if(currentP12_e==-std::numeric_limits<float>::infinity())
                currentP12_e = -1000000.;
            else
                currentP12_e = 1000000.;
        }
        if(isinf(currentP21_e))
        {
            if(currentP21_e==-std::numeric_limits<float>::infinity())
                currentP21_e = -1000000.;
            else
                currentP21_e = 1000000.;
        }
        meanCurrP_e += (currentP11_e +currentP22_e);
        countT_e++;
        simtime_t d = simTime() + inet::SimTime(delta_e*1000, inet::SIMTIME_MS);
        scheduleAt(d, selfMsg);
        if(usepolicy_e)
        {
            if(singleEpisode)
            {
                float trac = currentP11_e + currentP22_e;
                recordScalar("trac", trac);
            }
        }
        /*
        recordScalar("p21", currentP21_e);
        recordScalar("p22", currentP22_e);
        recordScalar("meanCurrP_e", meanCurrP_e);*/
    }
    else{
        socket.processMessage(msg);
    }

}

// input: UdpSocket *socket, Packet *pk
// return /, this function manages the receptions of messsages and the relative updating of the prediction variance. Moreover, whenever the predefined duration of the time windows is achieved,
//           the method to use the q-learning algorithm and to reset the statistics is called.
void UdpEchoAppMLfW_eter2::socketDataArrived(UdpSocket *socket, Packet *pk)
{
    // check if the time window is expire
    if (simTime() - timer >= interval)
    {
        makeDecision();
        resetTimer();
    }
    if(isElaborating_e)
    {
        if((simTime()-previousSampleElaboratingTime_e).dbl()*1000>=timeToElaborate_e)
            isElaborating_e = false;
    }
    //check of tags of the received messages: creationTime and FragmentationReq (used to signal if the message is elaborated)
    auto creationTime = SIMTIME_ZERO;
    bool fragm = true;
    try{
        auto data2 = pk->peekData(); // get all data from the packet
        auto regions = data2->getAllTags<CreationTimeTag>(); // get all tag regions
        for (auto& region : regions) { // for each region do
            creationTime = region.getTag()->getCreationTime();
        }
        auto regions2 = data2->getAllTags<FragmentationReq>(); // get all tag regions
        for (auto& region : regions2) { // for each region do
            fragm = region.getTag()->getDontFragment();
        }

    }
    catch(...)
    {}
    L3Address remoteAddress = pk->getTag<L3AddressInd>()->getSrcAddress();
    int srcPort = pk->getTag<L4PortInd>()->getSrcPort();
    int isElaborated = 0;
    auto currentTime = simTime();
    auto tau = currentTime-creationTime;
    //recordScalar("currentTime", currentTime.dbl());
    //recordScalar("creationTime", creationTime.dbl());
    //recordScalar("tau", tau.dbl());
    if(fragm)//tau.dbl()>0.04)
    {
        isElaborated = 1;
        numberOfReceivedElaboratedBytes_inthisWindow_eter++;
    }
    //update of delay
    for(int i=0;i<16;i++)
    {
        tauAr_e[i] += simTime()-previousSampleTime_e;
    }
    //check if the creation time of the received message is smaller then one of the delay stored (which are sorted)
    for(int i=15;i>=1;i--)
    {
        //if it is, store it in correct position and shift of the other delays (to keep the array sort). Manage the variance array and the elaborated array to keep correspondence between delay and them
        if(tau<=tauAr_e[i])
        {
            for(int j=0; j<i;j++)
            {
                tauAr_e[j] = tauAr_e[j+1];
                prediction11_e[j] = prediction11_e[j+1];
                prediction12_e[j] = prediction12_e[j+1];
                prediction21_e[j] = prediction21_e[j+1];
                prediction22_e[j] = prediction22_e[j+1];
                elaboratedAr_e[j] = elaboratedAr_e[j+1];
            }
            tauAr_e[i] = tau;
            //calculation of the variance prediction associated to the new messa
            float* openLoop;
            float r = 1;
            if(isElaborated==1)
            {
                double double_tau = tau.dbl();
                if(double_tau<0.060) //ultrawideband
                {
                    r = r_ela_uwb;
                    //recordScalar("r_ela_uwb",1);
                }
                else if(double_tau<0.120) //camera
                {
                    r = r_ela_cam;
                    //recordScalar("r_ela_cam",1);
                }
                else //lidar
                {
                    r = r_ela_lid;
                    //recordScalar("r_ela_lid",1);
                }
            }
            else
            {
                double double_tau = tau.dbl();
                if(double_tau<0.030) //ultrawideband
                {
                    r = r_uwb;
                    //recordScalar("r_uwb",1);
                }
                else if(double_tau<0.080) //camera
                {
                    r = r_cam;
                    //recordScalar("r_cam",1);
                }
                else //lidar
                {
                    r = r_lid;
                    //recordScalar("r_lid",1);
                }
            }
            elaboratedAr_e[i] = r;
            for(int n=0;n<messagesToElaborate_e;n++)
            {
                if(i>=positionMessages_e[n])
                {
                    if((positionMessages_e[n]-1)>0)
                        positionMessages_e[n] = positionMessages_e[n]-1;
                }
            }
            positionMessages_e[messagesToElaborate_e] = i;
            messagesToElaborate_e++;
            if(!(isElaborating_e))
            {
                isElaborating_e = false;
                previousSampleElaboratingTime_e = simTime();
                timeToElaborate_e = 0;//costTime_e*(messagesToElaborate_e+1);//costTime_e*log10(5*messagesToElaborate_e+1);
                for(int s =i;s<16;s++)
                {
                    int curr_r = elaboratedAr_e[s];
                    int stepLoop = (int)(floor((abs(tauAr_e[s].dbl()-tauAr_e[s-1].dbl()))/delta_e));
                    float* openLoopFinal = updateOpenLoopPrediction(prediction11_e[s-1],prediction12_e[s-1],prediction21_e[s-1],prediction22_e[s-1],stepLoop);
                    float* newPrediction = updatePrediction(openLoopFinal,curr_r);
                    /*if((prediction11_e[s]<newPrediction[0])&(prediction22_e[s]<newPrediction[3]))
                    {
                        prediction11_e[s] = newPrediction[0];
                        prediction12_e[s] = newPrediction[1];
                        prediction21_e[s] = newPrediction[2];
                        prediction22_e[s] = newPrediction[3];
                    }
                    else
                        break;
                        */
                    prediction11_e[s] = newPrediction[0];
                    prediction12_e[s] = newPrediction[1];
                    prediction21_e[s] = newPrediction[2];
                    prediction22_e[s] = newPrediction[3];
                }
                auto tau2 = tauAr_e[15];
                int sL =  (int)(floor((abs(tau2.dbl()))/delta_e));
                float *openLoopTemp2;
                if(sL>0)
                {
                    openLoopTemp2 = updateOpenLoopPrediction(prediction11_e[15],prediction12_e[15],prediction21_e[15],prediction22_e[15],1);
                    for(int mn=1;mn<sL;mn++)
                        openLoopTemp2 = updateOpenLoopPrediction(openLoopTemp2[0],openLoopTemp2[1],openLoopTemp2[2],openLoopTemp2[3],1);
                }
                else
                {
                    float temp2[4] = {prediction11_e[15],prediction12_e[15],prediction21_e[15],prediction22_e[15]};
                    openLoopTemp2 = temp2;
                }
                /*if((currentP11_e+currentP12_e+currentP21_e+currentP22_e)<(openLoopTemp2[0]+openLoopTemp2[1]+openLoopTemp2[2]+openLoopTemp2[3]))
                {
                    recordScalar("ERRORRRRR", 1);
                    recordScalar("currentP11_e", currentP11_e);
                    recordScalar("currentP22_e", currentP22_e);
                    recordScalar("openLoopTemp2[0]", openLoopTemp2[0]);
                    recordScalar("openLoopTemp2[3]", openLoopTemp2[3]);
                    recordScalar("tau", sL);
                    recordScalar("i", i);
                }*/
                currentP11_e = openLoopTemp2[0];
                currentP12_e = openLoopTemp2[1];
                currentP21_e = openLoopTemp2[2];
                currentP22_e = openLoopTemp2[3];
                /*
                for(int s=0;s<messagesToElaborate_e;s++)
                {
                    float temp[4] = {prediction11_e[positionMessages_e[s]],prediction12_e[positionMessages_e[s]],prediction21_e[positionMessages_e[s]],prediction22_e[positionMessages_e[s]]};
                    openLoop = &temp[0];
                    int stepLoop = (int)(floor((abs(tauAr_e[positionMessages_e[s]].dbl()-tauAr_e[positionMessages_e[s]-1].dbl()))/delta_e));
                    if(stepLoop>=1)
                    {
                        openLoop = updateOpenLoopPrediction(prediction11_e[positionMessages_e[s]-1],prediction12_e[positionMessages_e[s]-1],prediction21_e[positionMessages_e[s]-1],prediction22_e[positionMessages_e[s]-1],stepLoop);
                    }
                    float* newPrediction = updatePrediction(openLoop,r);
                    prediction11_e[positionMessages_e[s]] = newPrediction[0];
                    prediction12_e[positionMessages_e[s]] = newPrediction[1];
                    prediction21_e[positionMessages_e[s]] = newPrediction[2];
                    prediction22_e[positionMessages_e[s]] = newPrediction[3];
                    int k;
                    if(positionMessages_e[s]<15)
                    {
                        if(!(prediction11_e[positionMessages_e[s]+1]==10.))
                        {
                            for(k=positionMessages_e[s]+1;k<16;k++)
                            {
                                r = elaboratedAr_e[k-1];
                                stepLoop = (int)(floor((abs(tauAr_e[k].dbl()-tauAr_e[k-1].dbl()))/delta_e));
                                float* openLoopFinal = updateOpenLoopPrediction(prediction11_e[k-1],prediction12_e[k-1],prediction21_e[k-1],prediction22_e[k-1],stepLoop);
                                float* newPrediction = updatePrediction(openLoopFinal,r);
                                prediction11_e[k] = newPrediction[0];
                                prediction12_e[k] = newPrediction[1];
                                prediction21_e[k] = newPrediction[2];
                                prediction22_e[k] = newPrediction[3];
                            }
                        }
                    }
                    auto tau2 = tauAr_e[15];
                    /*recordScalar("previousVaBeforedelta_eTO",0);
                    recordScalar("p11Pop", prediction11_e[15]);
                    recordScalar("p12Ppp", prediction12_e[15]);
                    recordScalar("p21Pop", prediction21_e[15]);
                    recordScalar("p22Pop", prediction22_e[15]);
                    int sL =  (int)(floor((abs(tau2.dbl()))/delta_e));
                    /*recordScalar("tauAr_e", tauAr_e[15].dbl());
                    recordScalar("tau2", tau2.dbl());
                    recordScalar("stepLoop2", sL);
                    float *openLoopTemp2;
                    if(sL>0)
                        openLoopTemp2 = updateOpenLoopPrediction(prediction11_e[15],prediction12_e[15],prediction21_e[15],prediction22_e[15],sL);
                    else
                    {
                        float temp2[4] = {prediction11_e[15],prediction12_e[15],prediction21_e[15],prediction22_e[15]};
                        openLoopTemp2 = temp2;
                    }
                    /*recordScalar("p11Pop", openLoopTemp2[0]);
                    recordScalar("p12Ppp", openLoopTemp2[1]);
                    recordScalar("p21Pop", openLoopTemp2[2]);
                    recordScalar("p22Pop", openLoopTemp2[3]);
                    currentP11_e = openLoopTemp2[0];
                    currentP12_e = openLoopTemp2[1];
                    currentP21_e = openLoopTemp2[2];
                    currentP22_e = openLoopTemp2[3];
                }
                */
                messagesToElaborate_e = 0;
            }
            break;
        }
    }
    pk->clearTags();
    pk->trim();
    // statistics
    numberOfReceivedPacket_inthisWindow_eter += 1;//int(B(pk->getDataLength()).get());
    numEchoed++;
    emit(packetReceivedSignal, pk);
    delete pk;
    // send back a feedback message
    std::ostringstream str;
    str << "echo" << "-" << numEchoed;
    previousSampleTime_e = simTime();
    Packet *packet2 = new Packet(str.str().c_str());
    const auto& payload = makeShared<ApplicationPacket>();
    payload->setChunkLength(B(1));
    payload->setSequenceNumber(numEchoed);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet2->insertAtBack(payload);
    emit(packetSentSignal, packet2);
    socket->sendTo(packet2, remoteAddress, srcPort);

}

// Internal Omnet++ function, it was not modified.
void UdpEchoAppMLfW_eter2::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

// Internal Omnet++ function, it was not modified.
void UdpEchoAppMLfW_eter2::socketClosed(UdpSocket *socket)
{
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

// Internal Omnet++ function, it was not modified.
void UdpEchoAppMLfW_eter2::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();

    char buf[40];
    sprintf(buf, "echoed: %d pks", numEchoed);
    getDisplayString().setTagArg("t", 0, buf);
}

// input: /
// return /, this terminate the simulation, storing the policy_e and the values of the q action value table
void UdpEchoAppMLfW_eter2::finish()
{
    /*
    for(int i=0;i<9;i++)
    {
        //EV << "For state "<< i << "the policy_e suggest the action "<< policy_e[i];
        char buffer [20] = {0};
        sprintf (buffer, "policy_e%d", i);
        int initial = 0;
        int final = 3;
        if(i==0)
            initial=1;
        else if(i==8)
            final=2;
        float maxVal = QStateTable[i][initial];
        int maxPos = initial;
        for(int j = initial;j<final;j++)
        {
            if(QStateTable[i][j]>maxVal)
            {
                maxVal = QStateTable[i][j];
                maxPos = j;
            }
            char charTemp[20] = {0};
            sprintf (charTemp, "qtable|%d|%d", i,j);
            recordScalar(charTemp, QStateTable[i][j]);

        }
        recordScalar(buffer, double(maxPos));

    }*/
    for(int i=0;i<5;i++)
    {
        //EV << "For state "<< i << "the policy_e suggest the action "<< policy_e[i];
        char buffer [20] = {0};
        sprintf (buffer, "policy_e%d", i);
        float maxVal = QStateTable[i][0];
        int maxPos = 0;
        for(int j = 0;j<108;j++)
        {
            if(QStateTable[i][j]>maxVal)
            {
                maxVal = QStateTable[i][j];
                maxPos = j;
            }
            char charTemp[20] = {0};
            sprintf (charTemp, "qtable|%d|%d", i,j);
            recordScalar(charTemp, QStateTable[i][j]);

        }
        recordScalar(buffer, double(maxPos));

    }
    //recordScalar("countState0_e",countState0_e);
    //recordScalar("countState1_e",countState1_e);
    //recordScalar("countState2_e",countState2_e);
    //recordScalar("countState3_e",countState3_e);
    //recordScalar("countState4_e",countState4_e);
    //recordScalar("countState5_e",countState5_e);
    ApplicationBase::finish();
    exit(0);
}

// Internal Omnet++ function, it was not modified.
void UdpEchoAppMLfW_eter2::handleStartOperation(LifecycleOperation *operation)
{
    socket.setOutputGate(gate("socketOut"));
    int localPort = par("localPort");
    socket.bind(localPort);
    MulticastGroupList mgl = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this)->collectMulticastGroups();
    socket.joinLocalMulticastGroups(mgl);
    socket.setCallback(this);
    simtime_t d = simTime() + inet::SimTime(1, inet::SIMTIME_S);
    scheduleAt(d, selfMsg);
}

// Internal Omnet++ function, it was not modified.
void UdpEchoAppMLfW_eter2::handleStopOperation(LifecycleOperation *operation)
{
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

// Internal Omnet++ function, it was not modified.
void UdpEchoAppMLfW_eter2::handleCrashOperation(LifecycleOperation *operation)
{
    if (operation->getRootModule() != getContainingNode(this))     // closes socket when the application crashed only
        socket.destroy();         //TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
    socket.setCallback(nullptr);
}

// input: /
// return /, reset the timer
void UdpEchoAppMLfW_eter2::resetTimer()
{
    timer = simTime();
}

// input: float  p1, p12, p21, p22 = components of the variance prediction matrix, starting point for the openloop
//         int openLoopStep = step of openLoop to do
//         float r = variance of noise error associated to the message
// return float* toRet, this function to the openLoop of openLoopStep steps, starting from the given variance error matrix  and considering the associated variance of the noise
float* UdpEchoAppMLfW_eter2::updateOpenLoopPrediction(float p11,float p12,float p21,float p22,int openLoopStep)
{
    float a11 = 0;
    float a12 = 0;
    float a21 = 0;
    float a22 = 0;
    for(int i=0;i<openLoopStep;i++)
    {
        a11 += (pow(((float)(1+2*i))/2.,2)*pow(delta_e,4))*pow(sigma_e,2);
        a12 += (((float)(1+2*i))/2.*pow(delta_e,3))*pow(sigma_e,2);
        a21 += (((float)(1+2*i))/2.*pow(delta_e,3))*pow(sigma_e,2);
        a22 += (pow(delta_e,2))*pow(sigma_e,2);
    }
    a11 += p11 + (2.*((float)(openLoopStep*delta_e))*p21) + ((float)(pow((float)(openLoopStep*delta_e),2.)*p22));
    a12 += p12 + ((float)(openLoopStep*delta_e)*p22);
    a21 += p21 + ((float)(openLoopStep*delta_e)*p22);
    a22 += p22;
    float *toRet = new float[4];
    if(isnan(a11))
        a11 = 0.0001;
    if(isnan(a12))
       a12 = 0.0001;
    if(isnan(a21))
        a21 = 0.0001;
    if(isnan(a22))
        a22 = 0.0001;
    if(isinf(a11))
    {
        if(a11==-std::numeric_limits<float>::infinity())
            a11 = -1000000.;
        else
            a11 = 1000000.;
    }
    if(isinf(a12))
    {
        if(a12==-std::numeric_limits<float>::infinity())
            a12 = -1000000.;
        else
            a12 = 1000000.;
    }
    if(isinf(a21))
    {
        if(a21==-std::numeric_limits<float>::infinity())
            a21 = -1000000.;
        else
            a21 = 1000000.;
    }
    if(isinf(a22))
    {
        if(a22==-std::numeric_limits<float>::infinity())
            a22 = -1000000.;
        else
            a22 = 1000000.;
    }
    toRet[0] = a11;
    toRet[1] = a12;
    toRet[2] = a21;
    toRet[3] = a22;
    return toRet;
}

// input: float* openLoop, matrix to update, which was previously obtained doing an openloop
//         float r = variance of noise error associated to the message
// return float* toRet, this function update the variance error matrix, starting from the given variance error matrix  and considering the associated variance of the noise and returning the updated values
float* UdpEchoAppMLfW_eter2::updatePrediction(float *openLoopP, float r)
{
    float det = openLoopP[0] + pow(sigma_e2,2)/r;
    float a11 = openLoopP[0]* pow(pow(sigma_e2,2)/(r*det),2) + pow(sigma_e2,2)/r * pow(openLoopP[0]/det,2);
    float a12 = openLoopP[1]*pow(pow(sigma_e2,2)/(r*det),2) +  openLoopP[0]*openLoopP[1]*pow(sigma_e2,2)/(r*pow(det,2)) ;
    float a21 = a12;
    // (-P[0][1]**2*(sigma_e**2/r)-P[0][1]**2*(P[0][0]+sigma_e**2/r)+P[1][1]*(P[0][0]+sigma_e**2/r)**2)/(P[0][0]+(sigma_e**2/r))**2 +  (P[0][1]**2*(sigma_e**2/r))/(P[0][0]+sigma_e**2/r)**2
    float a22 = (-pow(openLoopP[1],2)*pow(sigma_e2,2)/r -pow(openLoopP[1],2)*(openLoopP[0]+pow(sigma_e2,2)/r) + openLoopP[3]*pow((openLoopP[0]+pow(sigma_e2,2)/r),2))/pow((openLoopP[0]+pow(sigma_e2,2)/r),2) + (pow(openLoopP[1],2)*pow(sigma_e2,2)/r)/pow((openLoopP[0]+pow(sigma_e2,2)/r),2);
    float *toRet = new float[4];
    if(isnan(a11))
        a11 = 0.0001;
    if(isnan(a12))
        a12 = 0.0001;
    if(isnan(a21))
        a21 = 0.0001;
    if(isnan(a22))
        a22 = 0.0001;
    if(isinf(a11))
    {
        if(a11==-std::numeric_limits<float>::infinity())
            a11 = -1000000.;
        else
            a11 = 1000000.;
    }
    if(isinf(a12))
    {
        if(a12==-std::numeric_limits<float>::infinity())
            a12 = -1000000.;
        else
            a12 = 1000000.;
    }
    if(isinf(a21))
    {
        if(a21==-std::numeric_limits<float>::infinity())
            a21 = -1000000.;
        else
            a21 = 1000000.;
    }
    if(isinf(a22))
    {
        if(a22==-std::numeric_limits<float>::infinity())
            a22 = -1000000.;
        else
            a22 = 1000000.;
    }
    toRet[0] = a11;
    toRet[1] = a12;
    toRet[2] = a21;
    toRet[3] = a22;
    return toRet;
}


} // namespace inet

