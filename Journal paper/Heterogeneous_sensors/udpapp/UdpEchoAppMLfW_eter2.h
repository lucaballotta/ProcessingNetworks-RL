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

#ifndef __INET_UdpEchoAppMLfW_eter2_H
#define __INET_UdpEchoAppMLfW_eter2_H

//#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

namespace inet {

/**
 * UDP application. See NED for more info.
 */
class INET_API UdpEchoAppMLfW_eter2 : public ApplicationBase, public UdpSocket::ICallback
{
  protected:
    simtime_t timer = SIMTIME_ZERO;
    UdpSocket socket;
    int numEchoed;    // just for WATCH
    cMessage *event;
    simtime_t interval = 0.300;
    //elaborated 50 ms, non elaborated 5 ms , r = 50 if elaborated
    // policy {44,33,40,38,41,42}

    float QStateTable[5][108] = {};
    int previousAction = 1;
    int currentState = 0;
    int currentAction = 1;
    int counterStepForEpisode = 0;
    int counterEpisodes = 0;
    float G = 0;
    float alpha = 0.1;
    float epsilon = 0.2;
    float gamma = 0.9;
    float previousP = 0.;
    cMessage *selfMsg = nullptr;
    int a = 0;
    //                    0,  1,  2,   3,    4,    5,    6,     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,   17,    18,     19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44
    int actionMap[108] = {0, 10, 11, 1000, 1010, 1011, 1100, 1110, 1111, 2000, 2010, 2011, 2100, 2110, 2111, 2200, 2210, 2211, 100000, 100010, 100011, 101000, 101010, 101011, 101100, 101110, 101111, 102000, 102010, 102011, 102100, 102110, 102111, 102200, 102210, 102211, 110000, 110010, 110011, 111000, 111010, 111011, 111100, 111110, 111111, 112000, 112010, 112011, 112100, 112110, 112111, 112200, 112210, 112211, 200000, 200010, 200011, 201000, 201010, 201011, 201100, 201110, 201111, 202000, 202010, 202011, 202100, 202110, 202111, 202200, 202210, 202211, 210000, 210010, 210011, 211000, 211010, 211011, 211100, 211110, 211111, 212000, 212010, 212011, 212100, 212110, 212111, 212200, 212210, 212211, 220000, 220010, 220011, 221000, 221010, 221011, 221100, 221110, 221111, 222000, 222010, 222011, 222100, 222110, 222111, 222200, 222210, 222211};
    float statesDiscretization[4] = {3.1850383281708003, 3.36750066280365, 3.64279782772065, 4.25131583213805};//{4.937722921371501, 5.21683549880985, 5.6140193939209, 6.6209404468536};

  protected:
    virtual void resetTimer();
    virtual void makeDecision();
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;
    virtual float* updateOpenLoopPrediction(float p11,float p12,float p21,float p22,int openLoopStep);
    virtual float* updatePrediction(float * openLoopP, float r);

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
    virtual int  epsilon_greedy(int state);
    virtual int  getBestAction(int state);
    virtual int  getRandomAction(int state);

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

  public:
    virtual int improvePolicy();
    //virtual void handleMessage(cMessage *message) override;
};

} // namespace inet

#endif // ifndef __INET_UdpEchoAppMLfW_eter2_H

