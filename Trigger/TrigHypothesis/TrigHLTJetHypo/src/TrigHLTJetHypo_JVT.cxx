/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
//
// NAME:     TrigHLTJet_JVT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   Jona Bossio
//
///////////////////////////////////////////////////////////////////////

#include "TrigHLTJetHypo/TrigHLTJetHypo_JVT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"

#include <limits>


// Constructor
TrigHLTJetHypo_JVT::TrigHLTJetHypo_JVT(const std::string& name, ISvcLocator* pSvcLocator):
TrigHLTJetHypoBase(name, pSvcLocator) {
  declareProperty("etmin", m_ETmin);
  declareProperty("wp",    m_workingPoint);
  declareProperty("njet",  m_NJets);
} 


// Destructor
TrigHLTJetHypo_JVT::~TrigHLTJetHypo_JVT(){}


// hltInitialize()
HLT::ErrorCode TrigHLTJetHypo_JVT::hltInitialize(){
  ATH_MSG_DEBUG("in initialize()");

  // Scale working point
  m_workingPoint *= m_scaleWP;

  return TrigHLTJetHypoBase::hltInitialize();
}


// getConditions()
Conditions TrigHLTJetHypo_JVT::getConditions() const{
  auto conditions = conditionsFactoryJVT(m_NJets,m_ETmin,m_workingPoint);
  std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
  return conditions;
}


// getJetGrouper()
std::shared_ptr<IJetGrouper> TrigHLTJetHypo_JVT::getJetGrouper() const {
  return std::make_shared<SingleJetGrouper>();
}


// checkVals()
bool TrigHLTJetHypo_JVT::checkVals() const{
  return true;
}


// getCleaners()
std::vector<std::shared_ptr<ICleaner>> TrigHLTJetHypo_JVT::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}
