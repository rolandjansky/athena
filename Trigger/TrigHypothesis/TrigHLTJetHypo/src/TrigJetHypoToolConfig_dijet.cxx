/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instatniation of JetHypo components for  the DiMass scenario
 */
#include "TrigJetHypoToolConfig_dijet.h"

#include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"

#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolConfig_dijet::TrigJetHypoToolConfig_dijet(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetHypoToolConfig_dijet::~TrigJetHypoToolConfig_dijet(){
}

StatusCode TrigJetHypoToolConfig_dijet::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}




Conditions TrigJetHypoToolConfig_dijet::getConditions() const {
  auto conditions = conditionsFactoryDijetMT(m_massMins,
                                             m_massMaxs,
                                             m_dEtaMins,
                                             m_dEtaMaxs,
                                             m_dPhiMins,
                                             m_dPhiMaxs);
 
  std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
  
  return conditions;
}

 
std::unique_ptr<IJetGrouper> TrigJetHypoToolConfig_dijet::getJetGrouper() const {
  return std::make_unique<CombinationsGrouper>(2);
}

StatusCode TrigJetHypoToolConfig_dijet::checkVals() const {
  // check cionsistent sizes

  auto sz = m_massMins.size();
  if (sz != m_massMaxs.size() or
      sz != m_dEtaMins.size() or
      sz != m_dEtaMaxs.size() or
      sz != m_dPhiMins.size() or
      sz != m_dPhiMaxs.size()){
    ATH_MSG_ERROR(name()
                  << ": mismatch between number of thresholds "
                  << "and min, max fro mass, dEta, dPhi "
                  << m_massMins.size() << " "
                  << m_massMaxs.size() << " "
                  << m_dEtaMins.size() << " "
                  << m_dEtaMaxs.size() << " "
                  << m_dPhiMins.size() << " "
                  << m_dPhiMaxs.size() << " ");
    
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

std::vector<std::shared_ptr<ICleaner>> 
TrigJetHypoToolConfig_dijet::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


