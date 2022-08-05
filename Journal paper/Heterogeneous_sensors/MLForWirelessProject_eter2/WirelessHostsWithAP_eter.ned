//
// This program is property of its copyright holder. All rights reserved.
//

package inet.examples.wireless.MLForWirelessProject_eter2;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth100M;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.node.inet.WirelessHost;
import inet.node.inet.AdhocHost;
import inet.node.wireless.AccessPoint;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
import inet.environment.common.PhysicalEnvironment;

network WirelessHostsWithAP_eter2
{
    @display("bgb=720,422");
    submodules:
        visualizer: <default("IntegratedCanvasVisualizer")> like IIntegratedVisualizer if hasVisualizer() {
            parameters:
                @display("p=442,398;is=vs");
        }
        configurator: Ipv4NetworkConfigurator {
            parameters:
                assignDisjunctSubnetAddresses = false;
                @display("p=32,385;is=vs");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            parameters:
                @display("p=208,384;is=vs");
        }
        physicalEnvironment: PhysicalEnvironment {
            parameters:
                @display("p=635,384;is=vs");
        }
        UWBsensor1: AdhocHost {
            parameters:
                @display("p=118,292");
        }
        UWBsensor2: AdhocHost {
            parameters:
                @display("p=295,292");
        }
        Camerasensor1: AdhocHost {
            parameters:
                @display("p=205,292");
        }
        Camerasensor2: AdhocHost {
            parameters:
                @display("p=205,105");
        }
        Lidarsensor1: AdhocHost {
            parameters:
                @display("p=170,200");
        }
        baseStation: AdhocHost {
            parameters:
                @display("p=5,16");
        }
        //router: Router {
        //    parameters:
        //        @display("p=100.94,52.015");
        //}

}
