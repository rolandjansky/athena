/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./CombinationsHelperTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./nodeIDPrinter.h"
#include "./JetTrigTimer.h"

#include <sstream>

CombinationsHelperTool::CombinationsHelperTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent) :
  base_class(type, name, parent){

}

StatusCode CombinationsHelperTool::initialize() {

  m_conditions = m_config->getConditions();
  m_grouper  = std::move(m_config->getJetGrouper());

  return StatusCode::SUCCESS;
}


void CombinationsHelperTool::collectData(const std::string& setuptime,
                                         const std::string& exetime,
                                         ITrigJetHypoInfoCollector* collector,
                                         bool pass) const {
  if(!collector){return;}
  auto helperInfo = nodeIDPrinter(name(),
                                  m_nodeID,
                                  m_parentNodeID,
                                  pass,
                                  exetime + setuptime
                                  );
  
  collector->collect(name(), helperInfo);
}


struct HypoJetSelector{
  HypoJetSelector(const ConditionsMT& c):m_conditions(c){}
  bool operator()(pHypoJet j){
    std::vector<pHypoJet> v{j};
    for(const auto& c : m_conditions)
      {
        if (!c.isSatisfied(v, nullptr))
          {
            return false;
          }
      }
    
    return true;
  }
  ConditionsMT m_conditions;
};

 
bool CombinationsHelperTool::pass(HypoJetVector& jets,
                                  ITrigJetHypoInfoCollector* collector) const {
  /* seek first jet group that passes all children  */
  
  // create vector of vector of jets

  JetTrigTimer exeTimer;
  JetTrigTimer setupTimer;

  setupTimer.start();

  
  HypoJetSelector selector(m_conditions);

  // use conditions objects to select jets
  auto end_iter = std::partition(jets.begin(),
                                 jets.end(),
                                 selector);
  
  // auto grouper = CombinationsGrouper(m_size);
  auto begin = jets.begin();
  auto jetGroups = m_grouper->group(begin, end_iter);

  ATH_MSG_DEBUG("No of groups" << jetGroups.size());

  bool pass = true;
  setupTimer.stop();
  exeTimer.start();

  for(auto& gjets : jetGroups){
    if (testGroup(gjets, collector)){
      pass = true;
      exeTimer.stop();
      collectData(setupTimer.readAndReset(),
                  exeTimer.readAndReset(),
                  collector,
                  pass);
      
      return pass;
    }
  }
  
  pass = false;
  exeTimer.stop();
  collectData(setupTimer.readAndReset(),
              exeTimer.readAndReset(),
              collector,
              pass);
  
  return pass;
}



bool
CombinationsHelperTool::testGroup(HypoJetVector& jets,
                                  ITrigJetHypoInfoCollector* collector) const {
  for(auto child : m_children){
    auto childPass =  child->pass(jets, collector);
    if (!childPass){
      return false;}
  }

  return true;
}
 

std::string CombinationsHelperTool::toString() const{
  std::stringstream ss;
  return nodeIDPrinter(name(), m_nodeID, m_parentNodeID);
}

StatusCode
CombinationsHelperTool::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  for(auto child : m_children){
    child->getDescription(c);
  }
  return StatusCode::SUCCESS;
}
