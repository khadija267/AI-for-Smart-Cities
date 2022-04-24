#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/netanim-module.h"

using namespace ns3;     //the namespace declaration

NS_LOG_COMPONENT_DEFINE ("WirelessTopology");

int 
main (int argc, char *argv[])
{

  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  uint32_t nCsma = 7;
  uint32_t backboneNodes = 10;

  
  CommandLine cmd;


  cmd.Parse (argc,argv);

  ///////////////////////////Creating the point-to-point part//////////////////////////////

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);


  //////////////////////////Creating the CSMA part////////////////////////////////////////
  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);
  
  
  ///////////////////////////Creating WIFI part////////////////////////////////////////
  
  NodeContainer backbone;
  backbone.Create (backboneNodes);

  WifiHelper wifi;
  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::AdhocWifiMac", "Ssid", SsidValue (ssid));
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate54Mbps"));
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  NetDeviceContainer backboneDevices;
  backboneDevices = wifi.Install (wifiPhy, mac, backbone);

  NetDeviceContainer backbonep2pDevice;
  NodeContainer backbonep2pNode = p2pNodes.Get (0);
  backbonep2pDevice = wifi.Install (wifiPhy, mac, backbonep2pNode);
  
  
  
  //////////////////////////Installing mobility models on the nodes///////////////////////////////
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (p2pNodes);
  mobility.Install (csmaNodes);
  
  MobilityHelper r_walk;
  r_walk.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-500, 500, -500, 500)),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=2]"));
  r_walk.Install (backbone);
  r_walk.Install (backbonep2pNode);
  
  ///////////////////////////////Installing protocol stack on the nodes///////////////////////////
  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (backbone);
  stack.Install (backbonep2pNode);

  ///////////////////////////Assigning IP addresses to the device interfaces////////////////////////
  Ipv4AddressHelper address;

  address.SetBase ("20.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("20.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("20.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInt;
  wifiInt = address.Assign (backboneDevices);
  address.Assign (backbonep2pDevice);  
  
  //////////////////////////Sending and receiving packets in the network/////////////////////////////
  
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (backbone.Get (backboneNodes - 1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  
  
  UdpEchoClientHelper echoClient (wifiInt.GetAddress (backboneNodes - 1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (100));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (csmaNodes.Get (nCsma));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  
  Simulator::Stop (Seconds (10.0));
  
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}