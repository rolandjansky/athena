/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETDETAMASSCONDITION_H
#define TRIGHLTJETHYPO_DIJETDETAMASSCONDITION_H

/********************************************************************
 *
 * NAME:     DijetDEtaMassCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class DijetDEtaMassCondition: public ICondition{
 public:
  DijetDEtaMassCondition(const std::vector<double>& etaMins,
                         const std::vector<double>& etaMaxs,
                         const std::vector<double>& ystarMins,
                         const std::vector<double>& ystarMaxs,
                         const std::vector<double>& massMins,
                         const std::vector<double>& massMaxs);

  
  ~DijetDEtaMassCondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  
  double orderingParameter() const noexcept override;
  
  std::string toString() const noexcept override;

 private:
  double m_etaMin;
  double m_etaMax;
  double m_dEtaMin;
  double m_dEtaMax;
  double m_massMin;
  double m_massMax;
};

#endif
