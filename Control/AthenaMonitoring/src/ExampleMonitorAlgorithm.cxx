/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ExampleMonitorAlgorithm.h"

ExampleMonitorAlgorithm::ExampleMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_doRandom(true)
{}


ExampleMonitorAlgorithm::~ExampleMonitorAlgorithm() {}


StatusCode ExampleMonitorAlgorithm::initialize() {
    using namespace Monitored;
    // initialize superclass
    ATH_CHECK( AthMonitorAlgorithm::initialize() );
    
    m_abGroups1 = buildToolMap<int>(m_tools,"ExampleMonitor",2);
    m_abGroups2 = buildToolMap<std::vector<int>>(m_tools,"ExampleMonitor",4,2);

    std::vector<std::string> layers = {"layer1","layer2"};
    std::vector<std::string> clusters = {"clusterX","clusterB"};
    m_cGroups1 = buildToolMap<int>(m_tools,"ExampleMonitor",layers);
    m_cGroups2 = buildToolMap<std::map<std::string,int>>(m_tools,"ExampleMonitor",layers,clusters);
    return StatusCode::SUCCESS;
}


StatusCode ExampleMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
    auto lb = Monitored::Scalar<int>("lb",0);
    auto run = Monitored::Scalar<int>("run",0);
    auto random = Monitored::Scalar<float>("random",0.0);
    auto testweight = Monitored::Scalar<float>("testweight",1.0);

    // Two variables (value and passed) needed for TEfficiency
    auto pT = Monitored::Scalar<float>("pT",0.0);
    auto pT_passed = Monitored::Scalar<bool>("pT_passed",false);

    // a string vector variable
    std::vector<std::string> strvec{"alpha", "beta"};
    auto mon_strvec = Monitored::Collection("strvec", strvec);
    // a plain string
    std::string mystring = "gamma";
    auto mon_str = Monitored::Scalar<std::string>("str", mystring);

    // Set the values of the monitored variables for the event
    lumiPerBCID = lbAverageInteractionsPerCrossing(ctx);
    lb = GetEventInfo(ctx)->lumiBlock();
    run = GetEventInfo(ctx)->runNumber();
    testweight = 2.0;
    
    TRandom3 r(ctx.eventID().event_number());
    // Example of using flags
    if (m_doRandom) {
        random = r.Rndm();
    }

    // Fake efficiency calculator
    pT = r.Landau(15);
    pT_passed = pT>r.Poisson(15);

    // Example of passing a collection
    std::vector<double> pT_vec{pT};
    while (pT_vec.back() > 5) {
        pT_vec.push_back(pT_vec.back() - 5);
    }
    auto mon_pT_vec = Monitored::Collection("pT_vec", pT_vec);

    // Fill. First argument is the tool name, all others are the variables to be saved.
    fill("ExampleMonitor",lumiPerBCID,lb,random,pT,pT_passed,mon_pT_vec,testweight,mon_str,mon_strvec);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("ExampleMonitor");
    fill(tool,run);

    // Fill with a vector; useful in some circumstances.
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> varVec = {lumiPerBCID,pT};
    fill("ExampleMonitor",varVec);
    fill(tool,varVec);

    // Filling using a pre-defined array of groups.
    auto a = Scalar<float>("a",0.0);
    auto b = Scalar<float>("b",1.0);
    auto c = Scalar<float>("c",2.0);
    for ( auto iEta : {0,1} ) {
        // 1) Valid but inefficient fill
        fill("ExampleMonitor_"+std::to_string(iEta),a,b,c);
        // 2) Faster way to fill a vector of histograms
        fill(m_tools[m_abGroups1[iEta]],a,b,c);
        for ( auto iPhi : {0,1} ) {
            // Same efficient method for 2D array
            fill(m_tools[m_abGroups2[iEta][iPhi]],a,b);
        }
    }

    // Filling using a pre-defined map of groups.
    for ( auto& layer : std::vector<std::string>({"layer1","layer2"}) ) {
        fill(m_tools[m_cGroups1.at(layer)],c);
        for ( auto& cluster : std::vector<std::string>({"clusterX","clusterB"}) ) {
            // Same efficient method for 2D map
            fill(m_tools[m_cGroups2.at(layer).at(cluster)],c);
        }
    }

    return StatusCode::SUCCESS;
}
