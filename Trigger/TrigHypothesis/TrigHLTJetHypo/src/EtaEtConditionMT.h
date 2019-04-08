/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ETAETCONDITIONMT_H
#define TRIGHLTJETHYPO_ETAETCONDITIONMT_H

/********************************************************************
 *
 * NAME:     EtaEtConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *********************************************************************/

#include <string>
#include "./IConditionMT.h"

namespace HypoJet{
  class IJet;
}

class IConditionVisitor;

class EtaEtConditionMT: public IConditionMT{
 public:
  EtaEtConditionMT(double etaMin,
                 double etaMax,
                 double threshold);
  ~EtaEtConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   IConditionVisitor*) const override;

  std::string toString() const noexcept override;
 private:

  double m_etaMin;
  double m_etaMax;
  double m_threshold;

  bool isSatisfied(const pHypoJet&,
                   IConditionVisitor*) const;
};

#endif
