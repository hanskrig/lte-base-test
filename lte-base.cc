#include <cmath>
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/propagation-loss-model.h"


using namespace ns3;

int
main(int argc, char* argv[])
{
    uint8_t nUE = 2;
    uint32_t dist = 1000;
    float simTime = 5.0;

    CommandLine cmd(__FILE__);

    cmd.AddValue("nUE", "Number of UE", nUE);
    cmd.AddValue("dist", "dist between UE and eBN", dist);
    cmd.AddValue("simTime", "simulation time", simTime);

    cmd.Parse(argc, argv);

    
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

    //creating loss model
    lteHelper->SetAttribute("PathlossModel", StringValue("ns3::FriisSpectrumPropagationLossModel"));
    lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");  

    NodeContainer enbNodes;
    NodeContainer ueNodes;
    enbNodes.Create(1);
    ueNodes.Create(nUE);

    //mobility
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNodes);

    Ptr<ConstantPositionMobilityModel> enbMobilityModel = enbNodes.Get(0)->GetObject<ConstantPositionMobilityModel>();
    enbMobilityModel->SetPosition(Vector(0.0, 0.0, 10.0)); 
    
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(ueNodes);

    //UEs are on the circle (with radius=dist) around eNB
    for (int i = 0; i < nUE; i++)
    {  
        Ptr<ConstantPositionMobilityModel> mm = ueNodes.Get(i)->GetObject<ConstantPositionMobilityModel>();
        double angle = 2 * M_PI * i / nUE;
        mm->SetPosition(Vector(dist*(std::cos(angle)), dist*(std::sin(angle)) , 0.0));

    }

    // Create Devices and install them in the Nodes (eNB and UE)
    NetDeviceContainer enbDevs;
    NetDeviceContainer ueDevs;
    enbDevs = lteHelper->InstallEnbDevice(enbNodes);
    ueDevs = lteHelper->InstallUeDevice(ueNodes);

    // Attaching a UE to a eNB
    lteHelper->Attach(ueDevs, enbDevs.Get(0));

    // Activate an EPS bearer
    EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer(q);
    lteHelper->ActivateDataRadioBearer(ueDevs, bearer);
    lteHelper->EnableMacTraces();
    lteHelper->EnableRlcTraces();

    Simulator::Stop(Seconds(simTime));
    Simulator::Run();
    Simulator::Destroy();


    return 0;
}
