/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/NumJetVarTool.h"
#include "xAODJet/Jet.h"


NumJetVarTool::NumJetVarTool(const std::string & type, const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
 , m_failureOnMissingContainer(false)
{
  declareInterface<IEventHistoVarTool>(this);
  declareProperty("FailureOnMissingContainer", m_failureOnMissingContainer);

}

StatusCode NumJetVarTool::initialize() {

  ATH_MSG_INFO("Counting number of jets with PtCut: "<<m_ptCut<<", EtCut: "<<m_etCut<<", EtaMin: "<<m_etaMin<<", EtaMax: "<<m_etaMax);

  return StatusCode::SUCCESS;

}

float NumJetVarTool::value(const xAOD::EventInfo & /*eventinfo not used in this implementation*/, const xAOD::JetContainer & jets) const {
  
  if ( jets.empty() ) {
    if (m_failureOnMissingContainer){
      ATH_MSG_ERROR("Input JetContainer could not be found or is empty");
      return 0.;
    } else {
      ATH_MSG_WARNING("Input JetContainer could not be found or is empty");
      return 0.;
    }
  }

  float njets = 0.;
  for (const xAOD::Jet* jet : jets) {
    if (jet->pt() >= m_ptCut*1000. && jet->p4().Et() >= m_etCut*1000. && fabs(jet->eta()) >= m_etaMin && fabs(jet->eta()) <= m_etaMax ) {
      njets++;
    }
  }
  return njets;
}
