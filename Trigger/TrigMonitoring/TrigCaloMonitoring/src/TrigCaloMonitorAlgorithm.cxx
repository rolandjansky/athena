/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloMonitorAlgorithm.h"
#include "TRandom3.h"

TrigCaloMonitorAlgorithm::TrigCaloMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}


TrigCaloMonitorAlgorithm::~TrigCaloMonitorAlgorithm() {}


StatusCode TrigCaloMonitorAlgorithm::initialize() {
    return AthMonitorAlgorithm::initialize();
}


StatusCode TrigCaloMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
    auto lb = Monitored::Scalar<int>("lb",0);
    auto run = Monitored::Scalar<int>("run",0);
    auto test = Monitored::Scalar<int>("test",0);

    // Two variables (value and passed) needed for TEfficiency
    auto pT = Monitored::Scalar<float>("pT",0.0);
    auto pT_passed = Monitored::Scalar<float>("pT_passed",false);

    // Set the values of the monitored variables for the event		
    lumiPerBCID = lbAverageInteractionsPerCrossing();
    lb = GetEventInfo(ctx)->lumiBlock();
    run = GetEventInfo(ctx)->runNumber();
    test = 3;

    // Fill. First argument is the tool (GMT) name as defined in the py file, 
    // all others are the variables to be saved.
    fill("TrigCaloMonitor",lumiPerBCID,lb,test);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("TrigCaloMonitor");
    fill(tool,run);

    return StatusCode::SUCCESS;
}
