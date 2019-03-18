/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JVTCONDITION_H
#define TRIGHLTJETHYPO_JVTCONDITION_H

/********************************************************************
 *
 * NAME:     JVTCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   Jona Bossio
 * CREATED:  February, 2019
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class JVTCondition: public ICondition{
 public:
  JVTCondition(double etThreshold, double workingPoint);

  ~JVTCondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  
  double orderingParameter() const noexcept override;
  
  std::string toString() const noexcept override;

 private:

  bool passCuts(pHypoJet) const;

  double m_etThreshold;
  double m_workingPoint;

};

#endif
