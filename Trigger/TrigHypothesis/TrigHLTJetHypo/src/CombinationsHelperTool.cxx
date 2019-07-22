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
  
  auto opt_conditions = m_config->getConditions();
  if(!opt_conditions.has_value()){
    ATH_MSG_ERROR("Error setting conditions");
    return StatusCode::FAILURE;
  }  
  m_conditions = std::move(*opt_conditions);
  
  m_grouper  = std::move(m_config->getJetGrouper());
  m_matcher = std::move(m_config->getMatcher());

  return StatusCode::SUCCESS;
}


void
CombinationsHelperTool::collectData(const std::string& setuptime,
                                    const std::string& exetime,
                                    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
				    
                                    bool pass) const {
  if(!collector){return;}
  for(const auto& c: m_conditions){
    collector->collect("Condition", c->toString());
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

    if(m_conditions.empty()){return true;}
    
    std::vector<pHypoJet> v{j};
    for(const auto& c : m_conditions)
      {
        if (c->isSatisfied(v, m_collector))  // there is a satisfied condition
          {
            return true;
          }
      }
    
    return false;   // no condition  satisfied
  }
  const ConditionsMT& m_conditions;
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

  auto end_iter = jets.end();

  if(!m_conditions.empty()){
  
    HypoJetSelector selector(m_conditions, collector);
  
    // use conditions objects to select jets
    end_iter = std::partition(jets.begin(),
			      jets.end(),
			      selector);
  }
  
  auto begin = jets.begin();
  std::vector<HypoJetGroupVector> jetGroupsVec =
    m_grouper->group(begin, end_iter);

  setupTimer.stop();
  exeTimer.start();

  bool passed{false};
  
  for(auto& jetGroupVec : jetGroupsVec){
    ATH_MSG_DEBUG("No of groups" << jetGroupVec.size());

    auto passes = m_matcher->match(jetGroupVec.begin(),
				 jetGroupVec.end(),
				 jetCollector,
				 collector);

    if(!passes.has_value()){
      ATH_MSG_ERROR("Matcher cannot determine result. Config error?");
      return false;
    }

    passed = *passes;
    if(passed){
      exeTimer.stop();
      collectData(setupTimer.readAndReset(),
		  exeTimer.readAndReset(),
		  collector,
		  passed);
	
      return passed;
    }
  }
  
  exeTimer.stop();
  collectData(setupTimer.readAndReset(),
              exeTimer.readAndReset(),
              collector,
              passed);
  
  return passed;
}


std::string CombinationsHelperTool::toString() const{
  std::stringstream ss;
  std::string msg =  nodeIDPrinter(name(), m_nodeID, m_parentNodeID) + "\n";
  msg += "Conditions:\nNo. of conditions "
    + std::to_string(m_conditions.size()) + '\n';
  for(const auto& cond : m_conditions){ msg += cond->toString() + "\n";}
  msg += "Grouper:\n" + m_grouper -> toString() + '\n';
  msg += "Matcher:\n" + m_matcher -> toString() + '\n';
  return msg;
}

StatusCode
CombinationsHelperTool::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  return StatusCode::SUCCESS;

}
std::size_t CombinationsHelperTool::requiresNJets() const {
  return m_config->requiresNJets();
}
