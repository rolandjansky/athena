/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJet_DijetMassDEtaTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: P Sherwood
//
// ********************************************************************

#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo_DijetMassDEtaTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"


TrigHLTJetHypo_DijetMassDEtaTool::TrigHLTJetHypo_DijetMassDEtaTool(const std::string& type,
                                                                   const std::string& name,
                                                                   const IInterface* parent):
  TrigHLTJetHypoTool(type, name, parent) {

  declareProperty("EtThresholds",   m_EtThresholds );
  declareProperty("dEta_mins", m_dEtaMins);
  declareProperty("dEta_maxs", m_dEtaMaxs);
  declareProperty("eta_mins", m_etaMins);
  declareProperty("eta_maxs", m_etaMaxs);
  declareProperty("mass_mins", m_massMins);
  declareProperty("mass_maxs", m_massMaxs);
  declareProperty("asymmetricEtas",   m_asymmetricEtas);
}

TrigHLTJetHypo_DijetMassDEtaTool::~TrigHLTJetHypo_DijetMassDEtaTool(){
}


 Conditions TrigHLTJetHypo_DijetMassDEtaTool::getConditions() const {
   
   // emulate old behaviour of TriggerMenu to set the min Et for the jets.
   // These limits are being set in the C++ code to discourage
   // firther changes. A new hypo strategy is under development to replce this
   // one.
   auto dmax = std::numeric_limits<double>::max();

   std::vector<double> etaMins {0., 0.};  // default from the run 1 hypo
   std::vector<double> etaMaxs {dmax, dmax}; // C++ default from Run 1
   std::vector<double> etThresholds = getEtThresholds(m_dEtaMins, 
                                                      m_EtThresholds);
   ATH_MSG_DEBUG("in setDijetMassDEtaConditions dEtamins:");
   for(auto em : m_dEtaMins){ATH_MSG_DEBUG(em);}
   ATH_MSG_DEBUG("in setDijetMassDEtaConditions m_EtThresholds:");
   for(auto et : m_EtThresholds){ATH_MSG_DEBUG(et);}
   ATH_MSG_DEBUG("in setDijetMassDEtaConditions etThresholds:");
   for(auto et : etThresholds){ATH_MSG_DEBUG(et);}


   auto conditions = conditionsFactoryDijetEtaMass(etaMins, etaMaxs,
                                                   etThresholds,
                                                   m_dEtaMins, m_dEtaMaxs,
                                                   m_massMins, m_massMaxs);
   
   std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
   return conditions;
 }


 
std::unique_ptr<IJetGrouper> TrigHLTJetHypo_DijetMassDEtaTool::getJetGrouper()  const{

  return std::make_unique<CombinationsGrouper>(2);
}


StatusCode TrigHLTJetHypo_DijetMassDEtaTool::checkVals() const {

  if (m_EtThresholds.size() != m_etaMins.size() or
      m_EtThresholds.size() != m_etaMaxs.size() or
      m_asymmetricEtas.size() != m_etaMaxs.size()){
      
    ATH_MSG_ERROR(name()
                  << ": mismatch between number of thresholds "
                  << "and eta min, max boundaries or asymmetric eta flags: "
                  << m_EtThresholds.size() << " "
                  << m_etaMins.size() << " "
                  << m_etaMaxs.size() << " "
                  << m_asymmetricEtas.size() << " "
                  );
    
    return StatusCode::FAILURE;
  }

  bool multOK = m_EtThresholds.size() > 1;
  bool ystarOK = (m_dEtaMins.size() < 2);
  bool massOK = (m_massMins.size() < 2);
  bool atLeastOne = m_dEtaMins.size() > 0 or m_massMins.size() > 0;
  
  if (not multOK){
    ATH_MSG_ERROR(name() << ": size error, expect >= 2 ");
    ATH_MSG_ERROR(name() << "Et thresholds " << m_EtThresholds);
  }
  
  if (not ystarOK){
    ATH_MSG_ERROR(name() << ": size error, expect 0 or 1");
    ATH_MSG_ERROR(name() << " dEta_mins " <<  m_dEtaMins.size());
    ATH_MSG_ERROR(name() << " dEta_maxs " <<  m_dEtaMaxs.size());
  }
  
  if (not massOK){
    ATH_MSG_ERROR(name() << ": size error, expect 0 or 1");
    ATH_MSG_ERROR(name() << " mass_mins " <<  m_massMins.size());
    ATH_MSG_ERROR(name() << " mass_maxs " <<  m_massMaxs.size());
  }
  
  
  if(not atLeastOne){
    ATH_MSG_ERROR(name() << " neither mass nor deta limits given");
  }
  
  return StatusCode(multOK and ystarOK and massOK and atLeastOne);
}


std::vector<std::shared_ptr<ICleaner>> 
TrigHLTJetHypo_DijetMassDEtaTool::getCleaners () const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}
