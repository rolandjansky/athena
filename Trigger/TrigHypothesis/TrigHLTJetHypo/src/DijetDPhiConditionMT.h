/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETDPHICONDITIONMT_H
#define TRIGHLTJETHYPO_DIJETDPHICONDITIONMT_H


/********************************************************************
 *
 * NAME:     DijetDPhiConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * Cuts on pairs of jets makling up a dijet
 *
 * AUTHOR:   P. Sherwood
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./IConditionMT.h"
#include <vector>
#include <string>

class ITrigJetHypoInfoCollector;

class DijetDPhiConditionMT: public IConditionMT{
 public:
  DijetDPhiConditionMT(double detaMin,
		       double detaMax);

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  std::string toString() const noexcept override;

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
