/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_IMATCHER_H
#define TRIGJETHYPO_IMATCHER_H
/********************************************************************
 *
 * NAME:     IMatcher.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "xAODJet/Jet.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include <string>
#include <memory>

using JetVector = std::vector<const xAOD::Jet*>;
using JetIter = JetVector::iterator;
using JetCIter = JetVector::const_iterator;



class IMatcher{
 public:
// Abstract interface used by a factory function
  virtual void match(JetIter b, JetIter e) = 0;
  virtual bool pass() const = 0;
  virtual std::pair<JetCIter, JetCIter> passed_iters() const = 0;
  virtual std::pair<JetCIter, JetCIter> failed_iters() const = 0;
  virtual const Conditions& getConditions() const noexcept = 0;
  virtual std::string toString() const noexcept = 0;
};

#endif
