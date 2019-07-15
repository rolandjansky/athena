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

  m_grouper  = m_config->getJetGrouper();

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

    if(m_conditions.empty()){return true;}
    
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

  bool pass = true;
  setupTimer.stop();
  exeTimer.start();
    
  for(auto& jetGroupVec : jetGroupsVec){
    ATH_MSG_DEBUG("No of groups" << jetGroupVec.size());

  
    for(auto& gjets : jetGroupVec){
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
    if (!childPass){return false;}
  }

  return true;
}
 

std::string CombinationsHelperTool::toString() const{
  std::stringstream ss;
  std::string msg =  nodeIDPrinter(name(), m_nodeID, m_parentNodeID) + "\n";
  msg += "No. of conditions " +std::to_string(m_conditions.size()) + '\n';
  for(const auto& cond : m_conditions){ msg += cond.toString() + "\n";}
  msg += "No. of chldren: " +std::to_string(m_children.size()) + "\n";
  msg += m_grouper -> toString() + '\n';
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
