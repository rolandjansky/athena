/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_HEAVYMASS2016CONDITION_H
#define TRIGHLTJETHYPO_HEAVYMASS2016CONDITION_H

/********************************************************************
 *
 * NAME:     TrigHLTJetHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  May 09, 2016
 *           
 *
 * Conditions object that searches for a particle P0 that decays to particles
 * P1 and P2  that decay to two jets each. Mass window cuts are applied
 * to P0, P1, P2.
 *
 *********************************************************************/

#include <string>
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"

namespace HypoJet{
  class IJet;
}

class HeavyMass2016Condition: public ICondition{
 public:
  HeavyMass2016Condition(double innerMassMin0, 
                         double innerMassMax0,
                         double innerMassMin1, 
                         double innerMassMax1, 
                         double outerMassMin, 
                         double outerMassMax);

  ~HeavyMass2016Condition() override {}

  bool isSatisfied(const HypoJetVector&) const override;

  double orderingParameter() const noexcept override;

  std::string toString() const noexcept override;

 private:

  double m_innerMassMin0;
  double m_innerMassMax0;
  double m_innerMassMin1;
  double m_innerMassMax1;
  double m_outerMassMin;
  double m_outerMassMax;
};

#endif
