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
    auto lumiPerBCID = MonitoredScalar::declare<float>("lumiPerBCID",0.0);
    auto lb = MonitoredScalar::declare<int>("lb",0);
    auto random = MonitoredScalar::declare<float>("random",0.0);    
    
    // Set the values of the monitored variables for the event
    lumiPerBCID = lbAverageInteractionsPerCrossing();
    lb = GetEventInfo(ctx)->lumiBlock();
    if (m_doRandom) {
        TRandom r;
        random = r.Rndm();
    }

    // Use the getGroup method to get your instance of the GenericMonitoringTool by name
    auto& tool = getGroup("ExampleMonitor");
    // Save. First argument is the tool, all others are the variables to be saved.
    Monitored::save(&tool,lumiPerBCID,lb,random);
    
    return StatusCode::SUCCESS;
}
