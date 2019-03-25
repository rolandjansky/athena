/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJet_HT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: P Sherwood
//
// ********************************************************************

#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_HT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AllJetsGrouper.h"

#include <limits>

TrigHLTJetHypo_HT::TrigHLTJetHypo_HT(const std::string& name,
			       ISvcLocator* pSvcLocator):
TrigHLTJetHypoBase(name, pSvcLocator) {

  declareProperty("EtThresholds",   m_EtThresholds );
  declareProperty("eta_mins", m_etaMins);
  declareProperty("eta_maxs", m_etaMaxs);
  declareProperty("htMin", m_htMin = 0);
}

TrigHLTJetHypo_HT::~TrigHLTJetHypo_HT(){
}

std::vector<std::shared_ptr<ICleaner>>  TrigHLTJetHypo_HT::getCleaners()  const{

  // treat the kinematic cut applied to each jet as a cleaner.                                            
  // Wether these arre handled as a cleaner, or in the condition is                                       
  // aribtrary, but handling them with a cleaner makes debugging easier.                                  
  std::vector<std::shared_ptr<ICleaner>> v  {
    makeEtaEtCleaner(m_etaMins[0],
                     m_etaMaxs[0],
                     m_EtThresholds[0],
                     std::numeric_limits<double>::max())
      };
  return v;
}


Conditions TrigHLTJetHypo_HT::getConditions()  const{

   auto conditions = conditionsFactoryHT(m_htMin);
   std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
   return conditions;
 }

 
std::shared_ptr<IJetGrouper> TrigHLTJetHypo_HT::getJetGrouper() const {
  return std::make_shared<AllJetsGrouper>();
}

bool TrigHLTJetHypo_HT::checkVals() const {
  if (1 != m_etaMins.size() or
      1 != m_etaMaxs.size() or
      1 != m_EtThresholds.size() or
      0. > m_htMin){
    
    
    ATH_MSG_ERROR(name()
                  << ": size != 1 or htMin illegal: "
                  << "eta_mins" <<  m_etaMins.size() << " "
                  << "eta_maxs" <<  m_etaMaxs.size() << " "
                  << "et thresholds " << m_EtThresholds.size() << " "
                  << "m_htMin " << m_htMin);
    return false;
  }
  return true;
}

