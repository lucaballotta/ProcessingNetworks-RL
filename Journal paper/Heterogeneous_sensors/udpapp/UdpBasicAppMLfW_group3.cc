//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 Andras Varga
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

#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/applications/udpapp/UdpBasicAppMLfW_group3.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"

namespace inet {

Define_Module(UdpBasicAppMLfW_group3);

extern int numberOfElaboration_g3;
extern int numberOfNodeSentmessage_g3;
extern int numberOfSending_g3;
extern int numberOfSentPacket_inthisWindow_g3 ;
extern int numberOfSentElaboratedPacket_inthisWindow_g3;
extern bool resHaveToela_g3;
int numberOfNodeWhichHaveToElaborate_group3 = 2;
int numberOfNodeWhichHaveToSend_group3 = 2;
int numberOfNode_group3= 4;
simtime_t samplePacketTime_group3;
int counterRes_g3 = 0;


UdpBasicAppMLfW_group3::~UdpBasicAppMLfW_group3()
{
    cancelAndDelete(selfMsg);
}

void UdpBasicAppMLfW_group3::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        numSent = 0;
        numReceived = 0;
        numElaboratedSent = 0;
        WATCH(numSent);
        WATCH(numReceived);
        samplePacketTime_group3 = SIMTIME_ZERO;

        localPort = par("localPort");
        destPort = par("destPort");
        startTime = 1 + uniform(0,1)*inet::SimTime(50, inet::SIMTIME_MS);
        stopTime = par("stopTime");
        packetName = par("packetName");
        dontFragment = par("dontFragment");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        selfMsg = new cMessage("sendTimer");
    }
}

void UdpBasicAppMLfW_group3::finish()
{
    recordScalar("packets sent", numSent);
    recordScalar("packets received", numReceived);
    recordScalar("packets elaborated sent", numElaboratedSent);
    ApplicationBase::finish();
}

void UdpBasicAppMLfW_group3::setSocketOptions()
{
    int timeToLive = par("timeToLive");
    if (timeToLive != -1)
        socket.setTimeToLive(timeToLive);

    int dscp = par("dscp");
    if (dscp != -1)
        socket.setDscp(dscp);

    int tos = par("tos");
    if (tos != -1)
        socket.setTos(tos);

    const char *multicastInterface = par("multicastInterface");
    if (multicastInterface[0]) {
        IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        InterfaceEntry *ie = ift->findInterfaceByName(multicastInterface);
        if (!ie)
            throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
        socket.setMulticastOutputInterface(ie->getInterfaceId());
    }

    bool receiveBroadcast = par("receiveBroadcast");
    if (receiveBroadcast)
        socket.setBroadcast(true);

    bool joinLocalMulticastGroups = par("joinLocalMulticastGroups");
    if (joinLocalMulticastGroups) {
        MulticastGroupList mgl = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this)->collectMulticastGroups();
        socket.joinLocalMulticastGroups(mgl);
    }
    socket.setCallback(this);
}

L3Address UdpBasicAppMLfW_group3::chooseDestAddr()
{
    int k = intrand(destAddresses.size());
    if (destAddresses[k].isUnspecified() || destAddresses[k].isLinkLocal()) {
        L3AddressResolver().tryResolve(destAddressStr[k].c_str(), destAddresses[k]);
    }
    return destAddresses[k];
}

void UdpBasicAppMLfW_group3::sendPacket(int type)
{
    std::ostringstream str;
    str << packetName << "-" << numSent;
    Packet *packet = new Packet(str.str().c_str());
    //if(dontFragment)
    //    packet->addTag<FragmentationReq>()->setDontFragment(true);
    const auto& payload = makeShared<ApplicationPacket>();
    payload->setChunkLength(B(par("messageLength")));
    payload->setSequenceNumber(numSent);
    if(type==2)
    {
        //recordScalar("p", 1.);
        auto timeTag = payload->addTag<CreationTimeTag>();
        timeTag->setCreationTime(simTime()-SimTime(0.040));
        auto fragment = payload->addTag<FragmentationReq>();
        fragment->setDontFragment(true);
        packet->insertAtBack(payload);
        L3Address destAddr = chooseDestAddr();
        //auto timeS = packet->getTag<CreationTimeTag>();
        //auto timeS2 = timeS->getCreationTime(); // original time
        //recordScalar("creaTim", timeS2.dbl());
        emit(packetSentSignal, packet);
        auto currT = simTime();
        //recordScalar("time", currT.dbl());
        socket.sendTo(packet, destAddr, destPort);
        numSent++;
    }
    else if(type==1)
    {
        //recordScalar("sleep", 0.);
        auto timeTag = payload->addTag<CreationTimeTag>();
        timeTag->setCreationTime(simTime()-SimTime(0.020));
        auto fragment = payload->addTag<FragmentationReq>();
        fragment->setDontFragment(false);
        packet->insertAtBack(payload);
        L3Address destAddr = chooseDestAddr();
        //auto timeS = packet->getTag<CreationTimeTag>();
        //auto timeS2 = timeS->getCreationTime(); // original time
        //recordScalar("creaTim", timeS2.dbl());
        emit(packetSentSignal, packet);
        auto currT = simTime();
        //recordScalar("time", currT.dbl());
        socket.sendTo(packet, destAddr, destPort);
        numSent++;
    }
    else
    {
        //do nothing
        int nothing =1;
    }
}

void UdpBasicAppMLfW_group3::processStart()
{
    socket.setOutputGate(gate("socketOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
    setSocketOptions();

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    while ((token = tokenizer.nextToken()) != nullptr) {
        destAddressStr.push_back(token);
        L3Address result;
        L3AddressResolver().tryResolve(token, result);
        if (result.isUnspecified())
            EV_ERROR << "cannot resolve destination address: " << token << endl;
        destAddresses.push_back(result);
    }

    if (!destAddresses.empty()) {
        selfMsg->setKind(SEND);
        processSend();

    }
    else {
        if (stopTime >= SIMTIME_ZERO) {
            selfMsg->setKind(STOP);
            scheduleAt(stopTime, selfMsg);
            samplePacketTime_group3 = simTime();
        }
    }
}

void UdpBasicAppMLfW_group3::processSend()
{
    simtime_t d = simTime() + inet::SimTime(20, inet::SIMTIME_MS) + uniform(0,1)*inet::SimTime(2, inet::SIMTIME_MS);
    if(resHaveToela_g3)
    {
        if(!resetDone)
        {
            resetHaveToSend();
            resetHaveToElaborate();
            counterRes_g3++;
            resetDone = true;
            if(counterRes_g3==numberOfNode)
            {
                resHaveToela_g3 = false;
                counterRes_g3 = 0;
            }
        }
        simtime_t d2 = simTime() + SimTime(5, inet::SIMTIME_MS);
        selfMsg->setKind(SEND);
        scheduleAt(d2, selfMsg);
    }
    else
    {
        resetDone = false;
        if((numberOfSending_g3<numberOfNodeWhichHaveToSend_group3)&(!(haveToSend)))
        {
            haveToSend = true;
            numberOfSending_g3++;
            if((numberOfElaboration_g3<numberOfNodeWhichHaveToElaborate_group3)&(!(haveToelaborate)))
            {
                haveToelaborate = true;
                numberOfElaboration_g3++;
            }
        }
        //recordScalar("haveToelaborate", haveToelaborate);
        if(haveToSend)
        {
            if(haveToelaborate)
            {
                numElaboratedSent++;
                numberOfSentElaboratedPacket_inthisWindow_g3 += 1;
                numberOfElaboratedPacket_inthisWindow += 1;
                //d += inet::SimTime(50, inet::SIMTIME_MS);
                sendPacket(2);
            }
            else
                sendPacket(1);
        }
        else
            sendPacket(0);

        numberOfNodeSentmessage_g3++;
        if (stopTime < SIMTIME_ZERO || d < stopTime) {
            selfMsg->setKind(SEND);
            scheduleAt(d, selfMsg);
            samplePacketTime_group3 = simTime();
            numberOfSentPacket_inthisWindow_g3  += 1;
        }
        else {
            selfMsg->setKind(STOP);
            scheduleAt(stopTime, selfMsg);
            samplePacketTime_group3 = simTime();
        }
    }
}

void UdpBasicAppMLfW_group3::processStop()
{
    socket.close();
}

void UdpBasicAppMLfW_group3::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        ASSERT(msg == selfMsg);
        switch (selfMsg->getKind()) {
            case START:
                processStart();
                break;

            case SEND:
                processSend();
                break;

            case STOP:
                processStop();
                break;

            default:
                throw cRuntimeError("Invalid kind %d in self message", (int)selfMsg->getKind());
        }
    }
    else
        socket.processMessage(msg);
}

void UdpBasicAppMLfW_group3::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    // process incoming packet
    processPacket(packet);
}

void UdpBasicAppMLfW_group3::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void UdpBasicAppMLfW_group3::socketClosed(UdpSocket *socket)
{
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void UdpBasicAppMLfW_group3::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();

    char buf[100];
    sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void UdpBasicAppMLfW_group3::processPacket(Packet *pk)
{
    emit(packetReceivedSignal, pk);
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(pk) << endl;
    delete pk;
    numReceived++;
}

void UdpBasicAppMLfW_group3::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t start = std::max(startTime, simTime());
    if ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime)) {
        selfMsg->setKind(START);
        scheduleAt(start, selfMsg);
    }
}

void UdpBasicAppMLfW_group3::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMsg);
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void UdpBasicAppMLfW_group3::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMsg);
    socket.destroy();         //TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
}

void UdpBasicAppMLfW_group3::resetHaveToElaborate()
{
    haveToelaborate = false;
}

void UdpBasicAppMLfW_group3::resetHaveToSend()
{
    haveToSend = false;
}
} // namespace inet

