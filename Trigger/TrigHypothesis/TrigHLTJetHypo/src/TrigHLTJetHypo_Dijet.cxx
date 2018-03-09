/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJet_Dijet.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: P Sherwood
//
// ********************************************************************

#include "TrigHLTJetHypo/TrigHLTJetHypo_Dijet.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"

#include <limits>

TrigHLTJetHypo_Dijet::TrigHLTJetHypo_Dijet(const std::string& name,
			       ISvcLocator* pSvcLocator):
TrigHLTJetHypoBase(name, pSvcLocator) {

  //Cuts on the individual jets in a pair

  declareProperty("aet_mins",  m_EtThresholds1);
  declareProperty("bet_mins",  m_EtThresholds2);

  declareProperty("aeta_mins", m_etaMins1);
  declareProperty("aeta_maxs", m_etaMaxs1);

  declareProperty("beta_mins", m_etaMins2);
  declareProperty("beta_maxs", m_etaMaxs2);

  // cuts on the dijet properties.

  declareProperty("m_mins", m_massMins);
  declareProperty("m_maxs", m_massMaxs);
  
  declareProperty("deta_mins", m_dEtaMins);
  declareProperty("deta_maxs", m_dEtaMaxs);

  declareProperty("dphi_mins", m_dPhiMins);
  declareProperty("dphi_maxs", m_dPhiMaxs);

}

TrigHLTJetHypo_Dijet::~TrigHLTJetHypo_Dijet(){
}

HLT::ErrorCode TrigHLTJetHypo_Dijet::hltInitialize()
{
  ATH_MSG_INFO("in initialize()");
  // positive inifinities are carrid over from the python config as -1.
  for(auto& d :  m_etaMaxs1) {
    if (d < 0){d = std::numeric_limits<double>::max();}
  }

  for(auto& d :  m_etaMaxs2) {
    if (d < 0){d = std::numeric_limits<double>::max();}
  }

  for(auto& d :  m_massMaxs) {
    if (d < 0){d = std::numeric_limits<double>::max();}
  }

  for(auto& d :  m_dEtaMaxs) {
    if (d < 0){d = std::numeric_limits<double>::max();}
  }

  for(auto& d :  m_dPhiMaxs) {
    if (d < 0){d = std::numeric_limits<double>::max();}
  }

  return TrigHLTJetHypoBase::hltInitialize();
}

 Conditions TrigHLTJetHypo_Dijet::getConditions() const {
   
   auto conditions = conditionsFactoryDijet(m_EtThresholds1,
                                            m_EtThresholds2,
                                            m_etaMins1,
                                            m_etaMaxs1,
                                            m_etaMins2,
                                            m_etaMaxs2,
                                            m_massMins,
                                            m_massMaxs,
                                            m_dEtaMins,
                                            m_dEtaMaxs,
                                            m_dPhiMins,
                                            m_dPhiMaxs);
   
   std::sort(conditions.begin(), conditions.end(), ConditionsSorter());

   ATH_MSG_DEBUG("getConditions() no of conditions "
                 << conditions.size()
                 << " no of thresholds "
                 << m_EtThresholds1.size());
   
   for(auto& c : conditions){ATH_MSG_DEBUG(c.toString());} 

   return conditions;
 }


 
std::shared_ptr<IJetGrouper> TrigHLTJetHypo_Dijet::getJetGrouper()  const{

  return std::make_shared<CombinationsGrouper>(2);
}


bool TrigHLTJetHypo_Dijet::checkVals() const {



  std::size_t n_dijets = m_EtThresholds1.size();
  if (n_dijets  != m_EtThresholds2.size() or
      n_dijets  != m_etaMins1.size() or
      n_dijets  != m_etaMaxs1.size() or
      n_dijets  != m_etaMins2.size() or
      n_dijets  != m_etaMaxs2.size() or
      n_dijets  != m_massMins.size() or
      n_dijets  != m_massMaxs.size() or
      n_dijets  != m_dEtaMins.size() or
      n_dijets  != m_dEtaMaxs.size() or
      n_dijets  != m_dPhiMins.size() or
      n_dijets  != m_dPhiMaxs.size()
      ){
    ATH_MSG_ERROR(name()
                  << "dijets configuration inconsistent for " <<n_dijets
                  << " dijets "
                  << m_etaMins1.size() << " "
                  << m_etaMaxs1.size() << " "
                  << m_etaMins2.size() << " "
                  << m_etaMaxs2.size() << " "
                  << m_massMins.size() << " "
                  << m_massMaxs.size() << " "
                  << m_dEtaMins.size() << " "
                  << m_dEtaMaxs.size() << " "
                  << m_dPhiMins.size() << " "
                  << m_dPhiMaxs.size()
                  );

    return false;
  }

  bool multOK = n_dijets > 0;
  
  if (not multOK){
    ATH_MSG_ERROR(name() << "no dijet cuts specified ");
    return false;
  }

  bool orderOK = true;
  for(std::size_t i = 0; i < n_dijets;  ++i){
    if (m_etaMins1[i] > m_etaMaxs1[i]) {orderOK = false;}
    if (m_etaMins2[i] > m_etaMaxs2[i]) {orderOK = false;}
    if (m_massMins[i] > m_massMaxs[i]) {orderOK = false;}
    if (m_dEtaMins[i] > m_dEtaMaxs[i]) {orderOK = false;}
    if (m_dPhiMins[i] > m_dPhiMaxs[i]) {orderOK = false;}
  }
  
  if(!orderOK){
    ATH_MSG_ERROR("a lower cut value exceeds an upper cut value ");
    return false;
  }
  
  return true;
}


std::vector<std::shared_ptr<ICleaner>> TrigHLTJetHypo_Dijet::getCleaners () const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}
