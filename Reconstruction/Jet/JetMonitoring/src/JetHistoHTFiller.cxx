/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "JetMonitoring/JetHistoHTFiller.h"
#include "JetMonitoring/JetMonitoringAlg.h"


JetHistoHTFiller::JetHistoHTFiller( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
  , m_failureOnMissingContainer(false)
{
  declareInterface<IJetHistoFiller>(this);
  declareProperty("FailureOnMissingContainer", m_failureOnMissingContainer);
  
}


StatusCode JetHistoHTFiller::initialize() {
  ATH_MSG_INFO(" Initializing " << name() << " with " << m_minPt << " and "<< m_maxEta);
  return StatusCode::SUCCESS;
}

StatusCode JetHistoHTFiller::finalize() {
  ATH_MSG_INFO ("Finalizing " << name());
  return StatusCode::SUCCESS;
}

StatusCode JetHistoHTFiller::processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ) const {

  if ( jets.empty() ) {
    if (m_failureOnMissingContainer){
      ATH_MSG_ERROR("Input JetContainer could not be found or is empty");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("Input JetContainer could not be found or is empty");
      return StatusCode::SUCCESS;
    }
  }

  auto HT = Monitored::Scalar<float> ("jetHT", 0.0);      
  for (const auto jetItr : jets) {
    float pt = jetItr->pt()/1000.;
    if ( pt > m_minPt ) {
      if (fabs(jetItr->eta()) < m_maxEta) HT += pt;
    }
    else { break; } // jets ordered by pT anyway
  }
  
  parentAlg.fill(m_group, HT);

  return StatusCode::SUCCESS;
}
