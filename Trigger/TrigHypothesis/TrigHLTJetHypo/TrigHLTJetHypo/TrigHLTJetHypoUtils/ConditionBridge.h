/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CONDITIONBRIDGE_H
#define TRIGHLTJETHYPO_CONDITIONBRIDGE_H
/********************************************************************
 *
 * NAME:     ConditionBridge.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>
#include <memory>
#include <ostream>

class ConditionBridge{
  // Bridge object: convert polymorphic pointer to monomorphic class
  // can be passed to STL algs.
 public:
 ConditionBridge(std::shared_ptr<ICondition>& condition):
  m_pCondition(condition){}
  
  bool isSatisfied(const HypoJetVector& ips) const{
    return m_pCondition -> isSatisfied(ips);
  }

  bool operator()(const HypoJetVector& ips) const{
    return isSatisfied(ips);
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

#endif
