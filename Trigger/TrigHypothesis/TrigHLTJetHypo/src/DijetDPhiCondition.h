/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETDPHICONDITION_H
#define TRIGHLTJETHYPO_DIJETDPHICONDITION_H


/********************************************************************
 *
 * NAME:     DijetDPhiCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * Cuts on pairs of jets makling up a dijet
 *
 * AUTHOR:   P. Sherwood
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./ICondition.h"
#include <vector>
#include <string>

class ITrigJetHypoInfoCollector;

class DijetDPhiCondition: public ICondition{
 public:
  DijetDPhiCondition(double detaMin,
		       double detaMax);

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  std::string toString() const override;

  virtual unsigned int capacity() const override{return s_capacity;}
  
 private:

  bool passJetCuts(pHypoJet, pHypoJet) const;
  bool passDijetDPhiCuts(pHypoJet, pHypoJet) const;

  // cuts on dphi of jets
  double m_min;
  double m_max;

  const static unsigned int s_capacity{2};

};

#endif
