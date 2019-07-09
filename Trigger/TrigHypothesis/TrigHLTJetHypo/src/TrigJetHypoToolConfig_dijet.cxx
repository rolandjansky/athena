/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instatniation of JetHypo components for  the DiMass scenario
 */
#include "TrigJetHypoToolConfig_dijet.h"

#include "GaudiKernel/StatusCode.h"

#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"
#include "ArgStrToDouble.h"
#include "./groupsMatcherFactoryMT.h"

#include "DecisionHandling/TrigCompositeUtils.h"

#include <algorithm>

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

  ArgStrToDouble a2d;
  
  std::transform(m_massMins_str.begin(),
                 m_massMins_str.end(),
                 std::back_inserter(m_massMins),
                 a2d
                 );

    
  std::transform(m_massMaxs_str.begin(),
                 m_massMaxs_str.end(),
                 std::back_inserter(m_massMaxs),
                 a2d
                 );

    
  std::transform(m_dEtaMins_str.begin(),
                 m_dEtaMins_str.end(),
                 std::back_inserter(m_dEtaMins),
                 a2d
                 );
  
  std::transform(m_dEtaMaxs_str.begin(),
                 m_dEtaMaxs_str.end(),
                 std::back_inserter(m_dEtaMaxs),
                 a2d
                 );

  std::transform(m_dPhiMins_str.begin(),
                 m_dPhiMins_str.end(),
                 std::back_inserter(m_dPhiMins),
                 a2d
                 );
  
  std::transform(m_dPhiMaxs_str.begin(),
                 m_dPhiMaxs_str.end(),
                 std::back_inserter(m_dPhiMaxs),
                 a2d
                 );

  return StatusCode::SUCCESS;
}




ConditionsMT TrigJetHypoToolConfig_dijet::getConditions() const {
  auto conditions = conditionsFactoryDijetMT(m_massMins,
                                             m_massMaxs,
                                             m_dEtaMins,
                                             m_dEtaMaxs,
                                             m_dPhiMins,
                                             m_dPhiMaxs);
 
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

std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_dijet::getMatcher () const {
  return groupsMatcherFactoryMT_MaxBipartite(getConditions());
}

