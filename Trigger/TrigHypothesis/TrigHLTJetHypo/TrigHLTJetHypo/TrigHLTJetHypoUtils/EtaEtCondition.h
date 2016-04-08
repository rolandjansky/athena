/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ETAETCONDITION_H
#define TRIGHLTJETHYPO_ETAETCONDITION_H

/********************************************************************
 *
 * NAME:     TrigHLTJetHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include <string>
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"

namespace HypoJet{
  class IJet;
}

class EtaEtCondition: public ICondition{
 public:
  EtaEtCondition(double etaMin, double etaMax, double threshold);
  ~EtaEtCondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;

  double orderingParameter() const noexcept override;

  std::string toString() const noexcept override;

 private:

  bool isSatisfied(const pHypoJet&) const;

  double m_etaMin;
  double m_etaMax;
  double m_threshold;
};

#endif
