/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETMASSCONDITIONMT_H
#define TRIGHLTJETHYPO_DIJETMASSCONDITIONMT_H

/********************************************************************
 *
 * NAME:     DijetMassConditionMT.h
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

class DijetMassConditionMT: public IConditionMT{
 public:
  DijetMassConditionMT(double massMin,
		       double massMax);

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  std::string toString() const noexcept override;

  virtual unsigned int capacity() const override{return s_capacity;}

 private:

  bool passJetCuts(pHypoJet, pHypoJet) const;
  bool passDijetMassCuts(pHypoJet, pHypoJet) const;

  // cuts on dijet mass
  double m_min;
  double m_max;

  const static unsigned int s_capacity{2};
};

#endif
