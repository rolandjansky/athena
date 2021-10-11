/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETDETACONDITION_H
#define TRIGHLTJETHYPO_DIJETDETACONDITION_H

/********************************************************************
 *
 * NAME:     DijetDEtaCondition.h
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

class DijetDEtaCondition: public ICondition{
 public:
  DijetDEtaCondition(double detaMin,
		       double detaMax);

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  std::string toString() const override;

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
