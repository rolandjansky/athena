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
  m_selector = PseudoJetSelector(m_etaMax, m_ptMin);
  ATH_MSG_INFO("etaMax: " << m_etaMax << " ptMin: " << m_ptMin << endreq);  

  return StatusCode::SUCCESS;
}

StatusCode PseudoJetSelectorEtaPt::select(const PseudoJetVector& in,
                                          PseudoJetVector& out) const {
  std::copy_if(in.begin(),
               in.end(),
               std::back_inserter(out),
               m_selector);
  
  return StatusCode::SUCCESS;}
  


