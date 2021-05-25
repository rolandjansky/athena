/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CLEANCONDITION_H
#define TRIGHLTJETHYPO_CLEANCONDITION_H

/********************************************************************
 *
 * NAME:     CleanCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   M. Valente
 *********************************************************************/

#include <string>
#include "./ICondition.h"
#include "AsgTools/AsgTool.h"
#include "xAODJet/JetContainer.h"
namespace HypoJet{
  class IJet;
}

class ITrigJetHypoInfoCollector;

class CleanCondition: public ICondition{
 public:
   CleanCondition(std::string cName);

   bool isSatisfied(const HypoJetVector &,
                    const std::unique_ptr<ITrigJetHypoInfoCollector> &) const override;

   virtual unsigned int capacity() const override { return s_capacity; }

   std::string toString() const override;
 private:
  
  std::string m_cName;
  bool isSatisfied(const pHypoJet&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const;
  
  const static  unsigned int s_capacity{1};

};

#endif
