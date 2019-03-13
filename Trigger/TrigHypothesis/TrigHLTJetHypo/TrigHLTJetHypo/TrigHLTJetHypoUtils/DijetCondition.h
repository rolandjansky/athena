/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETCONDITION_H
#define TRIGHLTJETHYPO_DIJETCONDITION_H

/********************************************************************
 *
 * NAME:     DijetCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class DijetCondition: public ICondition{
 public:
  DijetCondition(
                 double etThreshold0,
                 double etThreshold1,
                 double etaMin0,
                 double etaMax0,
                 double etaMin1,
                 double etaMax1,
                 double massMin,
                 double massMax,
                 double detaMin,
                 double detaMax,
                 double dphiMin,
                 double dphiMax
                 );

  ~DijetCondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  
  double orderingParameter() const noexcept override;
  
  std::string toString() const noexcept override;

 private:

  bool passJetCuts(pHypoJet, pHypoJet) const;
  bool passDijetCuts(pHypoJet, pHypoJet) const;


  // individual jet cut values
  double m_etThreshold0;
  double m_etThreshold1;

  double m_etaMin0;
  double m_etaMax0;

  double m_etaMin1;
  double m_etaMax1;

  // dijet cut values
  double m_massMin;
  double m_massMax;

  double m_detaMin;
  double m_detaMax;
  
  double m_dphiMin;
  double m_dphiMax;

};

#endif
