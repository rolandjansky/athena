/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_CONDITIONBRIDGE_H
#define TRIGJETHYPO_CONDITIONBRIDGE_H
/********************************************************************
 *
 * NAME:     ConditionBridge.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "TrigJetHypo/TrigHLTJetHypoUtils/ICondition.h"
#include <string>
#include <memory>
#include <ostream>

class ConditionBridge{
  // Bridge object: convert polymorphic pointer to monomorthic class
  // can be passed to STL algs.
 public:
 ConditionBridge(std::shared_ptr<ICondition>& condition):
  m_pCondition(condition){}
  
  bool isSatisfied(const std::vector<const xAOD::IParticle*>& ips) const{
    return m_pCondition -> isSatisfied(ips);
  }
  
  bool isSatisfied(const xAOD::IParticle* ip) const {
    return m_pCondition -> isSatisfied(ip);
  }
  
  std::string toString() const noexcept{
    return m_pCondition -> toString();
  }

  double orderingParameter() const noexcept{
    return m_pCondition -> orderingParameter();
  }

 private:
  std::shared_ptr<ICondition> m_pCondition;
};

/*
std::ostream& operator<< (std::ostream& os, const ConditionBridge& c){
  return os << c.toString() <<'\n';
}
*/

// Define a class to hold etaMin, etaMax and threshold (Et in MeV)
// that jets will be matched against
#endif
