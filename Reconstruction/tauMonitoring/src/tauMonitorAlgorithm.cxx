/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "tauMonitoring/tauMonitorAlgorithm.h"

tauMonitorAlgorithm::tauMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_doRandom(true)
{
}


tauMonitorAlgorithm::~tauMonitorAlgorithm() {}


StatusCode tauMonitorAlgorithm::initialize() {
    using namespace Monitored;

    ATH_CHECK( m_TauContainerKey.initialize() );

    return AthMonitorAlgorithm::initialize();
}


StatusCode tauMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;


    auto Eflow = Monitored::Scalar<float>("Eflow",-1.);
    auto eta = Monitored::Scalar<float>("eta",-4.0);
    auto phi = Monitored::Scalar<float>("phi",-1.0);


    SG::ReadHandle<xAOD::TauJetContainer> taus(m_TauContainerKey, ctx);
    if (! taus.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain tau Collection with name "<< m_TauContainerKey);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Number of taus: " << taus->size());

    bool test = false;

    for (const auto& tau : *taus) {
      // do stuff with taus
      
      eta = tau->eta();
      phi = tau->phi();
      ATH_MSG_DEBUG(m_etaMin << m_etaMax << eta);
      if ( (fabs(eta) < m_etaMin) || (fabs(eta) > m_etaMax)) continue;

      test=tau->detail(xAOD::TauJetParameters::mEflowApprox, Eflow);
      if (test) fill(m_kinGroupName, Eflow, eta, phi );


    }
    

    return StatusCode::SUCCESS;
}
