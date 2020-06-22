/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/NumJetVarTool.h"
#include "xAODJet/Jet.h"


NumJetVarTool::NumJetVarTool(const std::string & type, const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
 ,m_jetContainerKey("AntiKt4LCTopoJets"), m_failureOnMissingContainer(true)
{
  declareInterface<IEventHistoVarTool>(this);
  declareProperty("JetContainerName",m_jetContainerKey);
  declareProperty("FailureOnMissingContainer", m_failureOnMissingContainer);

}

StatusCode NumJetVarTool::initialize() {

  ATH_CHECK( m_jetContainerKey.initialize() );
  if(m_pTcut > 0.) ATH_MSG_INFO("Counting number of jets with pT >= " << m_pTcut);

  return StatusCode::SUCCESS;

}

float NumJetVarTool::value(const xAOD::EventInfo & e) const {

  SG::ReadHandle<xAOD::JetContainer> jets(m_jetContainerKey);    
  if (! jets.isValid() ) {
    if (m_failureOnMissingContainer){
      ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerKey);
      return 0.;
    } else {
      ATH_MSG_WARNING("evtStore() does not contain jet Collection with name "<< m_jetContainerKey);
      return 0.;
    }
  }

  float njets = 0.;
  for (const xAOD::Jet* jet : *jets) {
    if (jet->pt() >= m_pTcut*1000.) {
      njets++;
    }
  }
  return njets;
}
