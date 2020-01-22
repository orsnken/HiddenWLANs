#include "ns3.h"

#include "Domain.h"
#include "Framework.h"
#include "PhyParameters.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace ns3;
using namespace WirelessLan;

namespace {

int gPayloadSizeByte = 1470;

double gPositionX = 0.0;
double gPositionY = 0.0;

double gDistanceTerminalsMetre = 0.3;
double gDistanceDisturbersMetre = 15.0;

double gSimulationBeginSec = 0.0;
double gSimulationEndSec   = 5.0;

bool gEnableOutput = true;
bool gEnableOutputFile = false;

std::string gRemoteStationDataMode    = "ErpOfdmRate54Mbps";
std::string gRemoteStationControlMode = "ErpOfdmRate6Mbps";

ns3::WifiCeHelper          gWifiCeHelperDisturber;
ns3::WifiCeHelper          gWifiCeHelper;
ns3::YansWifiPhyHelper     gWifiPhyHelper;
ns3::YansWifiChannelHelper gWifiChannelHelper;

std::vector<Ptr<Domain>> gDomains;

}

std::string GenerateFilename() {
  std::string filename = "scratch/HiddenWLANs/output/";
  filename += "D";
  filename += std::to_string(static_cast<int>(gDistanceDisturbersMetre));
  filename += "_X" + std::to_string(static_cast<int>(gPositionX)) + "_Y" + std::to_string(static_cast<int>(gPositionY));
  filename += ".csv";
  return filename;
}

void Output(FlowMonitorHelper& flowMonitor, Ptr<FlowMonitor> fm) {
  fm->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowMonitor.GetClassifier());
  std::map<FlowId, FlowMonitor::FlowStats> stats = fm->GetFlowStats();
  std::cout << "--------------------------------------" << std::endl;
  std::vector<double> throughputs_kbps;
  uint32_t addr = 0xC0A86402; // "192.168.100.2"
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin(); iter != stats.end(); iter++) {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);
    double throughput_kbps = iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds() - iter->second.timeFirstTxPacket.GetSeconds()) / 1024;
    if (t.sourceAddress.Get() == addr) {
      // NS_LOG_UNCOND("Flow ID: " << iter->first << " src addr " << t.sourceAddress << " dest addr " << t.destinationAddress);
      // NS_LOG_UNCOND("TP:" << throughput_kbps << ", Tx:" << iter->second.txBytes << ", Rx:" << iter->second.rxBytes << ", LP:" << iter->second.lostPackets);
    }
    throughputs_kbps.push_back(throughput_kbps);
  }
  double total_th_kbps = 0.0;
  double fairness = 0.0;
  for (double th: throughputs_kbps) {
    fairness += th * th;
    total_th_kbps += th;
    NS_LOG_UNCOND(th / 1000.0 << "Mbps");
  }
  fairness = total_th_kbps * total_th_kbps / (static_cast<double>(throughputs_kbps.size()) * fairness);
  NS_LOG_UNCOND("TOTAL:" << total_th_kbps / 1000.0);
  NS_LOG_UNCOND("FAIRN:" << fairness);
}


void OutputFile(FlowMonitorHelper& flowMonitor, Ptr<FlowMonitor> fm) {
  std::string filename = GenerateFilename();
  std::ofstream ofs(filename, std::ios::app);
  if (!ofs) {
    NS_LOG_UNCOND("Can not open the file :" << filename << ".");
    return;
  }
  ofs << Framework::instance().GetSeed() << "," << Framework::instance().GetRunNumber() << ",";
    // output seed and run number.

  fm->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowMonitor.GetClassifier());
  std::map<FlowId, FlowMonitor::FlowStats> stats = fm->GetFlowStats();
  std::vector<double> throughputs_kbps;
  uint32_t addr = 0xC0A86402; // "192.168.100.2"
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin(); iter != stats.end(); iter++) {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);
    double throughput_kbps = iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds() - iter->second.timeFirstTxPacket.GetSeconds()) / 1024;
    if (t.sourceAddress.Get() == addr) {
      //test
    }
    throughputs_kbps.push_back(throughput_kbps);
  }
  
  double fairness = 0.0;
  double total_throughput_kbps = 0.0;
  for (double v: throughputs_kbps) {
    fairness += v * v;
    total_throughput_kbps += v;
    ofs << v / 1000 << ",";
  }
  fairness = total_throughput_kbps * total_throughput_kbps / (static_cast<double>(throughputs_kbps.size()) * fairness);
  // ofs << (total_throughput_kbps / static_cast<double>(throughputs_kbps.size())) << "," << fairness << std::endl;
  ofs << total_throughput_kbps / 1000 << "," << fairness << std::endl;
    // Output with Mbps
  ofs.close();
  NS_LOG_UNCOND("Output file done. > " << filename);
}

ApplicationContainer SetOnOffApplication(
  Ptr<Node> src,
  Ptr<Node> dest,
  int port,
  std::string protocol
) {
  if (protocol == "ns3::UdpSocketFactory") {
    Ipv4InterfaceAddress dest_addr = dest->GetObject<Ipv4>()->GetAddress(1, 0);
    InetSocketAddress remote_sock_addr(dest_addr.GetLocal(), port);

    OnOffHelper ftp(protocol, Address());
    ftp.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    ftp.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    ftp.SetAttribute("DataRate", StringValue("50Mbps"));
    ftp.SetAttribute("PacketSize", UintegerValue(gPayloadSizeByte));
    ftp.SetAttribute("Remote", AddressValue(remote_sock_addr));
    ApplicationContainer app = ftp.Install(src);
    PacketSinkHelper sink_helper(protocol, Address(remote_sock_addr));
    app.Add(sink_helper.Install(dest));
    return app;
  } else {
    Ipv4InterfaceAddress destAddr = dest->GetObject<Ipv4>()->GetAddress(1, 0);
    InetSocketAddress remoteSockAddr(destAddr.GetLocal(), port);

    BulkSendHelper ftp("ns3::TcpSocketFactory", Address());
    ftp.SetAttribute("Remote", AddressValue(remoteSockAddr));
    ApplicationContainer app = ftp.Install(src);

    InetSocketAddress remoteSockAddrT(destAddr.GetLocal(), port);
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", Address(remoteSockAddrT));
    app.Add(sinkHelper.Install(dest));
    return app;
  }
}


ApplicationContainer SetApplication(
  const Domain& domain,
  int port
) {
  ApplicationContainer app;
  for (int i = 0, n = domain.GetN() - 1; i < n; i++) {
    app.Add(SetOnOffApplication(domain.GetStaNode(i), domain.GetApNode(), port + i, "ns3::UdpSocketFactory"));
  }
  return app;
}

void InitMac() {
  gWifiCeHelperDisturber.SetStandard(kWifiPhyStandard);
  gWifiCeHelperDisturber.SetRemoteStationManager(
    "ns3::IdealWifiManager"
  );

  gWifiCeHelper.SetStandard(kWifiPhyStandard);
  gWifiCeHelper.SetRemoteStationManager(
    "ns3::ConstantRateWifiManager",
    "DataMode"   , StringValue(gRemoteStationDataMode),
    "ControlMode", StringValue(gRemoteStationControlMode)
  );

  gWifiChannelHelper.SetPropagationDelay(
    "ns3::ConstantSpeedPropagationDelayModel"
  );
  gWifiChannelHelper.AddPropagationLoss(
    "ns3::LogDistancePropagationLossModel",
    "Exponent"         , DoubleValue(kLogDistancePropagationLossExponent),
    "ReferenceDistance", DoubleValue(kLogDistancePropagationLossReferenceDistanceMetre),
    "ReferenceLoss"    , DoubleValue(kLogDistancePropagationLossReferenceLoss)
  );
  gWifiChannelHelper.AddPropagationLoss(
    "ns3::NakagamiPropagationLossModel",
    "Distance1", DoubleValue(kNakagamiPropagationLossDistance1Metre),
    "Distance2", DoubleValue(kNakagamiPropagationLossDistance2Metre),
    "m0", DoubleValue(kNakagamiPropagationLossM0),
    "m1", DoubleValue(kNakagamiPropagationLossM1),
    "m2", DoubleValue(kNakagamiPropagationLossM2)
  );

  gWifiPhyHelper = YansWifiPhyHelper::Default();
  gWifiPhyHelper.SetPcapDataLinkType(kPhyPcapDlt);
  gWifiPhyHelper.SetChannel(gWifiChannelHelper.Create());
  gWifiPhyHelper.Set("EnergyDetectionThreshold", DoubleValue(kPhyEnergyDetectionThresholdDbm));
  gWifiPhyHelper.Set("CcaMode1Threshold"       , DoubleValue(kPhyCcaMode1ThresholdDbm));
  gWifiPhyHelper.Set("TxPowerStart"            , DoubleValue(kPhyTxPowerStartDbm));
  gWifiPhyHelper.Set("TxPowerEnd"              , DoubleValue(kPhyTxPowerEndDbm));
  gWifiPhyHelper.Set("ChannelNumber", UintegerValue(1));
}

void InitNetwork() {
  WifiMacHelper mac_ap, mac_sta;
  mac_ap.SetType("ns3::ApWifiMac");
  mac_sta.SetType(
    "ns3::StaWifiMac",
    "ActiveProbing",   BooleanValue(false)
  );

  ApplicationContainer app;

  // True Triangle
  // for (int i = 1; i <= 3; i++) {
  //   Ptr<Domain> network_disturber = Create<Domain>("Network_Distuber_" + std::to_string(i), "192.168.20" + std::to_string(i) + ".0", "255.255.255.0", 1);
  //   network_disturber->Construct(
  //     gWifiCeHelperDisturber,
  //     gWifiPhyHelper,
  //     "ns3::ApWifiMac",
  //     mac_ap,
  //     "ns3::StaWifiMac",
  //     mac_sta
  //   );
  //   double r_disturbers_metre = gDistanceDisturbersMetre / 1.7320508;
  //   double th = 3.141592 * 2.0 / 3.0 * i;
  //   network_disturber->ConfigureMobility(
  //     Vector3D(r_disturbers_metre * sin(th), r_disturbers_metre * cos(th), 0.0), gDistanceTerminalsMetre
  //   );
  //   gDomains.push_back(network_disturber);
  //   app.Add(SetApplication(*network_disturber, 5000));
  // }

  // E-Triangle
  for (int i = 1; i <= 3; i++) {
    Ptr<Domain> network_disturber = Create<Domain>("Network_Distuber_" + std::to_string(i), "192.168.20" + std::to_string(i) + ".0", "255.255.255.0", 1);
    network_disturber->Construct(
      gWifiCeHelperDisturber,
      gWifiPhyHelper,
      "ns3::ApWifiMac",
      mac_ap,
      "ns3::StaWifiMac",
      mac_sta
    );
    double x = 0.0, y = 0.0;
    if (i == 1) {
      x = gDistanceDisturbersMetre / 4.0;
      y = -gDistanceDisturbersMetre * 1.7320508 / 4.0;
    } else if (i == 2) {
      x = -gDistanceDisturbersMetre / 4.0;
      y = -gDistanceDisturbersMetre * 1.7320508 / 4.0;
    } else if (i == 3) {
      x = 0.0;
      y = gDistanceDisturbersMetre * 1.7320508 / 4.0;
    }
    network_disturber->ConfigureMobility(Vector3D(x, y, 0.0), gDistanceTerminalsMetre);
    gDomains.push_back(network_disturber);
    app.Add(SetApplication(*network_disturber, 5000));
  }

  Ptr<Domain> network_target = Create<Domain>("Network_Target", "192.168.100.0", "255.255.255.0", 1);
  network_target->Construct(
    gWifiCeHelper,
    gWifiPhyHelper, 
    "ns3::ApWifiMac",
    mac_ap,
    "ns3::StaWifiMac",
    mac_sta
  );
  network_target->ConfigureMobility(
    Vector3D(gPositionX, gPositionY, 0.0), gDistanceTerminalsMetre
  );
  gDomains.push_back(network_target);
  app.Add(SetApplication(*network_target, 6000));

  app.Start(Seconds(gSimulationBeginSec + 0.1));
  app.Stop(Seconds(gSimulationEndSec - 0.1));
}

void Framework::InitArguments(int argc, char *argv[]) {
  CommandLine cmd;
  int seed = -1, run_number = -1;
  cmd.AddValue("Seed",
               "The seed for the simulation. Set to -1 if you want to initialize with a random stream.",
               seed);
  cmd.AddValue("RunNumber",
               "The run number for the simulation. Set to -1 if you want to initialize with a random stream.",
               run_number);
  cmd.AddValue("SimulationTime",
               "The simulation time in second.",
               gSimulationEndSec);
  cmd.AddValue("DistanceTerminals",
               "The distance in metre between AP and STA.",
               gDistanceTerminalsMetre);
  cmd.AddValue("DistanceDisturbers",
               "The distance in metre between wireless LANs.",
               gDistanceDisturbersMetre);
  cmd.AddValue("EnableOutput",
               "Enable output log.",
               gEnableOutput);
  cmd.AddValue("EnableOutputFile",
               "Enable output file.",
               gEnableOutputFile);
  cmd.AddValue("PositionX",
               "The value of x for the target wireless LAN.",
               gPositionX);
  cmd.AddValue("PositionY",
               "The value of y for the target wireless LAN.",
               gPositionY);
  cmd.AddValue("PayloadSize",
               "The payload size in byte.",
               gPayloadSizeByte);
  cmd.AddValue("RemoteStationDataMode",
               "The transmission rate for data mode.",
               gRemoteStationDataMode);
  cmd.AddValue("RemoteStationControlMode",
               "The transmission rate for control mode.",
               gRemoteStationControlMode);
  cmd.Parse(argc, argv);
  this->SetSeed(seed);
  this->SetRunNumber(run_number);
}

void Framework::InitSimulation() {
  std::cout << "[S" << this->GetSeed() << ",R" << this->GetRunNumber() << "]" << std::endl;

  Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue(999999));
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1470));

  InitMac();
  InitNetwork();
}

void Framework::Run() {
  // AnimationInterface anim("netanim/HiddenWLANs-anim-test.xml");

  FlowMonitorHelper flow_monitor;
  Ptr<FlowMonitor> fm = flow_monitor.InstallAll();

  Simulator::Stop(Seconds(gSimulationEndSec));
  Simulator::Run();
  Simulator::Destroy();

  if (gEnableOutput) {
    Output(flow_monitor, fm);
  }

  if (gEnableOutputFile) {
    OutputFile(flow_monitor, fm);
  }
}
