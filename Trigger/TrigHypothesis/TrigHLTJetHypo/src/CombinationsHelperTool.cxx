/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./CombinationsHelperTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"
#include "./ITrigJetHypoHelperVisitor.h"
#include "./nodeIDPrinter.h"

#include <sstream>

CombinationsHelperTool::CombinationsHelperTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent) :
  base_class(type, name, parent),
  m_totalTimer(std::make_unique<JetTrigTimer>()),
  m_setupTimer(std::make_unique<JetTrigTimer>()),
  m_extraTimer(std::make_unique<JetTrigTimer>()),
  m_extraTimer1(std::make_unique<JetTrigTimer>()){

}

bool CombinationsHelperTool::pass(HypoJetVector& jets) {
  /* seek first jet group that passes all children  */
  
  // create vector of vector of jets
  m_totalTimer -> start();
  m_setupTimer -> start();
  
  auto b = jets.begin();
  auto e = jets.end();
  auto grouper = CombinationsGrouper(m_size);
  auto jetGroups = grouper.group(b, e);

  ATH_MSG_DEBUG("No of groups" << jetGroups.size());
  m_pass = true;
  m_setupTimer -> stop();

  for(auto& jets : jetGroups){
    if (testGroup(jets)){
      m_pass = true;
      m_totalTimer->stop();
      return m_pass;
    }
  }
  
  m_pass = false;
  m_totalTimer->stop();
  return m_pass;
}



bool CombinationsHelperTool::testGroup(HypoJetVector& jets) const{
  m_extraTimer->start();
  for(auto child : m_children){
    m_extraTimer1->start();
    auto childPass =  child->pass(jets);
    m_extraTimer1->stop();
    if (!childPass){
      m_extraTimer->stop();
      return false;}
  }

  m_extraTimer->stop();
  return true;
}
 

std::string CombinationsHelperTool::toString() const{
  std::stringstream ss;
  ss << nodeIDPrinter(name(), m_nodeID, m_parentNodeID, m_pass,
                      m_totalTimer->readAndReset());
  ss  << "size " << m_size << " setup time: "
      <<m_setupTimer->readAndReset()
      <<" extra time " << m_extraTimer->readAndReset()
      <<" extra time1 " << m_extraTimer1->readAndReset()
      <<'\n';
  return ss.str();
}


std::string CombinationsHelperTool::toStringAndResetHistory() {
  auto result = toString();
  return result;
}


void CombinationsHelperTool::accept(ITrigJetHypoHelperVisitor& v){
  v.visit(this);
  for(const auto child: m_children){
    child->accept(v);
  }
}
