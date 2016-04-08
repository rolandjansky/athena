/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ICONDITION_H
#define TRIGHLTJETHYPO_ICONDITION_H
/********************************************************************
 *
 * NAME:     ICondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <vector>
#include <string>


class ICondition {
 public:
  virtual ~ICondition(){}
  virtual bool 
    isSatisfied(const HypoJetVector&) const = 0;
  virtual std::string toString() const noexcept = 0;
  virtual double orderingParameter() const noexcept = 0;
};

#endif
