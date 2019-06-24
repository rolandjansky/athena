/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./CombinationsHelperTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./nodeIDPrinter.h"
#include "./JetTrigTimer.h"
#include "./DebugInfoCollector.h"

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


void
CombinationsHelperTool::collectData(const std::string& setuptime,
                                    const std::string& exetime,
                                    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
				    
                                    bool pass) const {
  if(!collector){return;}
  for(auto c: m_conditions){
    collector->collect("Condition", c.toString());
  }
  auto helperInfo = nodeIDPrinter(name(),
				  m_nodeID,
				  m_parentNodeID,
				  pass,
				  exetime + setuptime
				  );

  collector->collect(name(), helperInfo);
}


struct HypoJetSelector{
  // Selector jets according to OR of ocnditions objects.
  // This predicate is intended to be used with an STL algorithm
  HypoJetSelector(const ConditionsMT& conditions,
                  const std::unique_ptr<ITrigJetHypoInfoCollector>&collector ):
    m_conditions(conditions), m_collector(collector){
  }
  
  bool operator()(pHypoJet j){
    std::vector<pHypoJet> v{j};
    for(const auto& c : m_conditions)
      {
        if (c.isSatisfied(v, m_collector))  // there is a satisfied condition
          {
            return true;
          }
      }
    
    return false;   // no condition  satisfied
  }
  ConditionsMT m_conditions;
  const std::unique_ptr<ITrigJetHypoInfoCollector>& m_collector;
};
 
 
bool
CombinationsHelperTool::pass(HypoJetVector& jets,
			     xAODJetCollector& jetCollector,
			     const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  /* seek first jet group that passes all children  */
  
  // create vector of vector of jets
  
  JetTrigTimer exeTimer;
  JetTrigTimer setupTimer;
  
  setupTimer.start();
  
  
  HypoJetSelector selector(m_conditions, collector);
  
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
    if (testGroup(gjets, jetCollector, collector)){
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
				  xAODJetCollector& jetCollector,
				  const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  for(auto child : m_children){
    auto childPass =  child->pass(jets, jetCollector, collector);
    if (!childPass){
      return false;}
  }

  return true;
}
 

std::string CombinationsHelperTool::toString() const{
  std::stringstream ss;
  std::string msg =  nodeIDPrinter(name(), m_nodeID, m_parentNodeID) + "\n";
  for(const auto& cond : m_conditions){ msg += cond.toString() + "\n";}
  return msg;
}

StatusCode
CombinationsHelperTool::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  StatusCode sc;
  for(auto child : m_children){
    sc = sc & child->getDescription(c);
  }
  return sc;
}
