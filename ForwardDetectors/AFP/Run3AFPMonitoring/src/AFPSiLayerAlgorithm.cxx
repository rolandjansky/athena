/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Run3AFPMonitoring/AFPSiLayerAlgorithm.h"

/* Old constructor:
AFPHitsMonitorAlgorithm::AFPHitsMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_doRandom(true)
{}
*/

AFPSiLayerAlgorithm::AFPSiLayerAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_doRandom(true)
{}


AFPSiLayerAlgorithm::~AFPSiLayerAlgorithm() {}


StatusCode AFPSiLayerAlgorithm::initialize() {
    using namespace Monitored;
    m_abGroups1 = buildToolMap<int>(m_tools,"AFPSiLayer",2);
    m_abGroups2 = buildToolMap<std::vector<int>>(m_tools,"AFPSiLayer",4,2);

    std::vector<std::string> layers = {"layer1","layer2"};
    std::vector<std::string> clusters = {"clusterX","clusterB"};
    m_cGroups1 = buildToolMap<int>(m_tools,"AFPSiLayer",layers);
    m_cGroups2 = buildToolMap<std::map<std::string,int>>(m_tools,"AFPSiLayer",layers,clusters);
    return AthMonitorAlgorithm::initialize();
}


StatusCode AFPSiLayerAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    // My histograms:
    auto h_hitMultiplicity = Monitored::Scalar<int>("h_hitMultiplicity", 0);
    auto h_timeOverThreshold = Monitored::Scalar<int>("h_timeOverThreshold", 0);
    //auto h_hitMap = Monitored::Scalar<int>("h_hitMap", 0);
    // End

    auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
    auto lb = Monitored::Scalar<int>("lb",0);
    auto run = Monitored::Scalar<int>("run",0);
    auto random = Monitored::Scalar<float>("random",0.0);
    auto testweight = Monitored::Scalar<float>("testweight",1.0);

    // Two variables (value and passed) needed for TEfficiency
    auto pT = Monitored::Scalar<float>("pT",0.0);
    auto pT_passed = Monitored::Scalar<bool>("pT_passed",false);

    // Set the values of the monitored variables for the event
    h_timeOverThreshold = lbAverageInteractionsPerCrossing(ctx);
    h_hitMultiplicity = lbAverageInteractionsPerCrossing(ctx);
    //    h_hitMap = lbAverageInteractionsPerCrossing(ctx);
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

    // Fill. First argument is the tool name, all others are the variables to be saved.
    fill("AFPSiLayer", h_timeOverThreshold, h_hitMultiplicity,lumiPerBCID,lb,random,pT,pT_passed,testweight);
    //fill("AFPSiLayerAlgorithm",h_hitMultiplicity,lb,random,pT,pT_passed,testweight);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("AFPSiLayer");
    fill(tool,run);

    // Fill with a vector; useful in some circumstances.
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> varVec = {lumiPerBCID,pT};
    fill("AFPSiLayer",varVec);
    fill(tool,varVec);

    // Filling using a pre-defined array of groups.
    auto a = Scalar<float>("a",0.0);
    auto b = Scalar<float>("b",1.0);
    auto c = Scalar<float>("c",2.0);
    for ( auto iEta : {0,1} ) {
        // 1) Valid but inefficient fill
        fill("AFPSiLayer_"+std::to_string(iEta),a,b,c);
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
