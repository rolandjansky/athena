/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETCONDITIONMT_H
#define TRIGHLTJETHYPO_DIJETCONDITIONMT_H

/********************************************************************
 *
 * NAME:     DijetConditionMT.h
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

class IConditionVisitor;

class DijetConditionMT: public IConditionMT{
 public:
  DijetConditionMT(
                   double massMin,
                   double massMax,
                   double detaMin,
                   double detaMax,
                   double dphiMin,
                   double dphiMax);

  ~DijetConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   IConditionVisitor*) const override;
  
  std::string toString() const noexcept override;

 private:

  bool passJetCuts(pHypoJet, pHypoJet) const;
  bool passDijetCuts(pHypoJet, pHypoJet) const;

  // cuts on sum of jets
  double m_massMin;
  double m_massMax;

  // cuts on the two jets
  double m_detaMin;
  double m_detaMax;
  
  double m_dphiMin;
  double m_dphiMax;
};

#endif
