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
bool CombinationsHelperTool::pass(HypoJetVector& jets,
                                  ITrigJetHypoInfoCollector* collector) const {
  /* seek first jet group that passes all children  */
  
  // create vector of vector of jets

  JetTrigTimer exeTimer;
  JetTrigTimer setupTimer;

  setupTimer.start();
  
  auto b = jets.begin();
  auto e = jets.end();
  auto grouper = CombinationsGrouper(m_size);
  auto jetGroups = grouper.group(b, e);

  ATH_MSG_DEBUG("No of groups" << jetGroups.size());

  bool pass = true;
  setupTimer.stop();
  exeTimer.start();

  for(auto& jets : jetGroups){
    if (testGroup(jets, collector)){
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
  StatusCode sc;
  for(auto child : m_children){
    sc = sc & child->getDescription(c);
  }
  return sc;
}
