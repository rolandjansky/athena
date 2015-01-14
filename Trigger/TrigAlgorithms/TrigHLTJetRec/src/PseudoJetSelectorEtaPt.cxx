/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolFactory.h"
#include "TrigHLTJetRec/PseudoJetSelectorEtaPt.h"

PseudoJetSelectorEtaPt::PseudoJetSelectorEtaPt(const std::string& s,
                                               const std::string& name,
                                               const IInterface* i):
  AthAlgTool(s, name, i){
  declareInterface<IPseudoJetSelector>(this);
  declareProperty("etaMax", m_etaMax);
  declareProperty("ptMin", m_ptMin);
}

StatusCode PseudoJetSelectorEtaPt::initialize() {
  m_remover = PseudoJetRemover(m_etaMax, m_ptMin);
  ATH_MSG_INFO("etaMax: " << m_etaMax << " ptMin: " << m_ptMin << endreq);  

  return StatusCode::SUCCESS;
}

StatusCode PseudoJetSelectorEtaPt::select(PseudoJetVector& pseudojets) const {
  pseudojets.erase(std::remove_if(pseudojets.begin(),
                                  pseudojets.end(),
                                  m_remover),
                   pseudojets.end());
  
  return StatusCode::SUCCESS;}
  


