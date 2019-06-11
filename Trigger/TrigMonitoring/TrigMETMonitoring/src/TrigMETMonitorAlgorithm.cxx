/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMETMonitorAlgorithm.h"

TrigMETMonitorAlgorithm::TrigMETMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}


TrigMETMonitorAlgorithm::~TrigMETMonitorAlgorithm() {}


StatusCode TrigMETMonitorAlgorithm::initialize() {
    return AthMonitorAlgorithm::initialize();
}


StatusCode TrigMETMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    //NB! The variables and histograms defined here must match the ones in the py file exactly!
    auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
    auto lb = Monitored::Scalar<int>("lb",0);
    auto run = Monitored::Scalar<int>("run",0);

    // Two variables (value and passed) needed for TEfficiency
    auto pT = Monitored::Scalar<float>("pT",0.0);
    auto pT_passed = Monitored::Scalar<float>("pT_passed",false);

    //// Set the values of the monitored variables for the event		
    lumiPerBCID = lbAverageInteractionsPerCrossing();
    lb = GetEventInfo(ctx)->lumiBlock();
    run = GetEventInfo(ctx)->runNumber();
    
    // Fill. First argument is the tool (GMT) name as defined in the py file, 
    // all others are the variables to be saved.
    fill("TrigMETMonitor",lumiPerBCID,lb);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("TrigMETMonitor");
    fill(tool,run);

    return StatusCode::SUCCESS;
}
