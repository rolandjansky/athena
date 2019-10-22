/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETDETACONDITIONMT_H
#define TRIGHLTJETHYPO_DIJETDETACONDITIONMT_H

/********************************************************************
 *
 * NAME:     DijetDEtaConditionMT.h
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

class DijetDEtaConditionMT: public IConditionMT{
 public:
  DijetDEtaConditionMT(double detaMin,
		       double detaMax);

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  std::string toString() const noexcept override;

  virtual unsigned int capacity() const override{return s_capacity;}

 private:

  bool passJetCuts(pHypoJet, pHypoJet) const;
  bool passDijetDEtaCuts(pHypoJet, pHypoJet) const;


  // cuts on the two jet dEta
  double m_min;
  double m_max;

  const static unsigned int s_capacity{2};

};

#endif
