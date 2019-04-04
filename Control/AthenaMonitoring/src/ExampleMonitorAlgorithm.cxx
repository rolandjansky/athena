/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/ExampleMonitorAlgorithm.h"

ExampleMonitorAlgorithm::ExampleMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_doRandom(false)
{}


ExampleMonitorAlgorithm::~ExampleMonitorAlgorithm() {}


StatusCode ExampleMonitorAlgorithm::initialize() {
    return AthMonitorAlgorithm::initialize();
}


StatusCode ExampleMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
    auto lb = Monitored::Scalar<int>("lb",0);
    auto run = Monitored::Scalar<int>("run",0);
    auto random = Monitored::Scalar<float>("random",0.0);

    // Two variables (value and passed) needed for TEfficiency
    auto pT = Monitored::Scalar<float>("pT",0.0);
    auto pT_passed = Monitored::Scalar<float>("pT_passed",false);

    // Set the values of the monitored variables for the event
    lumiPerBCID = lbAverageInteractionsPerCrossing();
    lb = GetEventInfo(ctx)->lumiBlock();
    run = GetEventInfo(ctx)->runNumber();
    
    TRandom3 r(ctx.eventID().event_number());
    // Example of using flags
    if (m_doRandom) {
        random = r.Rndm();
    }

    // Fake efficiency calculator
    pT = r.Landau(15);
    pT_passed = pT>r.Poisson(15);

    // Fill. First argument is the tool name, all others are the variables to be saved.
    fill("ExampleMonitor",lumiPerBCID,lb,random,pT,pT_passed);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("ExampleMonitor");
    fill(tool,run);
    
    return StatusCode::SUCCESS;
}
