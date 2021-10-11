/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETMASSCONDITION_H
#define TRIGHLTJETHYPO_DIJETMASSCONDITION_H

/********************************************************************
 *
 * NAME:     DijetMassCondition.h
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

class DijetMassCondition: public ICondition{
 public:
  DijetMassCondition(double massMin,
		       double massMax);

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  std::string toString() const override;

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
