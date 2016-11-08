/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TLACONDITION_H
#define TRIGHLTJETHYPO_TLACONDITION_H

/********************************************************************
 *
 * NAME:     TLACondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class TLACondition: public ICondition{
 public:
  TLACondition(const std::vector<double>& etaMins,
               const std::vector<double>& etaMaxs,
               const std::vector<double>& ystarMins,
               const std::vector<double>& ystarMaxs,
               const std::vector<double>& massMins,
               const std::vector<double>& massMaxs);

  TLACondition(double etaMin,
               double etaMax,
               double ystarMin,
               double ystarMax,
               double massMin,
               double massMax);
  
  ~TLACondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  
  double orderingParameter() const noexcept override;
  
  std::string toString() const noexcept override;

 private:
  double m_etaMin;
  double m_etaMax;
  double m_ystarMin;
  double m_ystarMax;
  double m_massMin;
  double m_massMax;
};

#endif
