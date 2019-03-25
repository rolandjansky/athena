/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ETAECONDITION_H
#define TRIGHLTJETHYPO_ETAECONDITION_H

/********************************************************************
 *
 * NAME:     EtaECondition.h
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

class EtaECondition: public ICondition{
 public:
  EtaECondition(double etaMin, double etaMax, double threshold);
  ~EtaECondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  bool isSatisfied(const pHypoJet&) const;

  double orderingParameter() const noexcept override;

  std::string toString() const noexcept override;

 private:

  double m_etaMin;
  double m_etaMax;
  double m_threshold;
};

#endif
