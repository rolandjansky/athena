/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaPerformance/MonitorElectronAlgorithm.h"

MonitorElectronAlgorithm::MonitorElectronAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
{
}

MonitorElectronAlgorithm::~MonitorElectronAlgorithm() {}


StatusCode MonitorElectronAlgorithm::initialize() {
    using namespace Monitored;

    ATH_CHECK( m_ParticleContainerKey.initialize() );
    return AthMonitorAlgorithm::initialize();
}


StatusCode MonitorElectronAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Only monitor good LAr Events :

    xAOD::EventInfo::EventFlagErrorState error_state = GetEventInfo(ctx)->errorState(xAOD::EventInfo::LAr);
    if (error_state==xAOD::EventInfo::Error) {
      ATH_MSG_DEBUG("LAr event data integrity error");
      return StatusCode::SUCCESS;
    }

    // get the Electron container

    SG::ReadHandle<xAOD::ElectronContainer> electrons(m_ParticleContainerKey, ctx);
    if (! electrons.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain electron Collection with name "<< m_ParticleContainerKey);
      return StatusCode::FAILURE;
    }

    // Event variables to be monitored

    auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
    auto lb = Monitored::Scalar<int>("lb",0);
    auto run = Monitored::Scalar<int>("run",0);

    // Particle variables to be monitored

    std::string name = m_ParticlePrefix+"N"+m_RecoName+m_WithTrigger;
    auto np = Monitored::Scalar<int>(name,0.0);
 
    auto pt = Monitored::Scalar<float>("pt",0.0); 

    // Set the values of the monitored variables for the event
    lumiPerBCID = lbAverageInteractionsPerCrossing(ctx);
    lb = GetEventInfo(ctx)->lumiBlock();
    run = GetEventInfo(ctx)->runNumber();

    // Fill. First argument is the tool name, all others are the variables to be histogramed
    fill("MonitorElectron",lumiPerBCID,lb,run);
 

    np = 0;
    for (const auto& e_iter : *electrons) {

      // Check that the electron meets our requirements
      bool isGood;
      if (! e_iter->passSelection(isGood,m_RecoName)) {
        ATH_MSG_WARNING("Misconfiguration: " << m_RecoName << " is not a valid working point");
        break; // no point in continuing
      }
      if(isGood) {np++;}
      else continue;
                           
      // do stuff with electrons
      pt = e_iter->pt()/Gaudi::Units::GeV; // in GeV
      fill("MonitorElectron",pt);

    }

    fill("MonitorElectron",np);

    return StatusCode::SUCCESS;
}
