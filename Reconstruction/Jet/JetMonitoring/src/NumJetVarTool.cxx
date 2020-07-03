/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/NumJetVarTool.h"
#include "xAODJet/Jet.h"


NumJetVarTool::NumJetVarTool(const std::string & type, const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
 , m_failureOnMissingContainer(true)
{
  declareInterface<IEventHistoVarTool>(this);
  declareProperty("FailureOnMissingContainer", m_failureOnMissingContainer);

}

StatusCode NumJetVarTool::initialize() {

  if(m_pTcut > 0.) ATH_MSG_INFO("Counting number of jets with " << m_pTcut);

  return StatusCode::SUCCESS;

}

float NumJetVarTool::value(const xAOD::EventInfo & e, const xAOD::JetContainer & jets) const {
  
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
    if (jet->pt() >= m_pTcut*1000.) {
      njets++;
    }
  }
  return njets;
}
