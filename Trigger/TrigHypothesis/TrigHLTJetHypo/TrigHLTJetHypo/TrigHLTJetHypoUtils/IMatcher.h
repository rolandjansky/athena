/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IMATCHER_H
#define TRIGHLTJETHYPO_IMATCHER_H
/********************************************************************
 *
 * NAME:     IMatcher.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>


class IMatcher{
 public:
// Abstract interface used by a factory function
  virtual ~IMatcher(){}
  virtual void match(const HypoJetIter&, const HypoJetIter&) = 0;
  virtual bool pass() const = 0;
  virtual const HypoJetVector& passedJets() const noexcept = 0;
  virtual const HypoJetVector& failedJets() const noexcept = 0;
  virtual const Conditions& getConditions() const noexcept = 0;
  virtual std::string toString() const noexcept = 0;
};

#endif


