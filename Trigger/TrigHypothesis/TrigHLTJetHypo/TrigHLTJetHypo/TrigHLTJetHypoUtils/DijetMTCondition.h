/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETMTCONDITION_H
#define TRIGHLTJETHYPO_DIJETMTCONDITION_H

/********************************************************************
 *
 * NAME:     DijetMTCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * Cuts on paris of jets makling up a dijet
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  February 21, 2019
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class DijetMTCondition: public ICondition{
 public:
  DijetMTCondition(
                   double massMin,
                   double massMax,
                   double detaMin,
                   double detaMax,
                   double dphiMin,
                   double dphiMax
                 );

  ~DijetMTCondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  
  double orderingParameter() const noexcept override;
  
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
