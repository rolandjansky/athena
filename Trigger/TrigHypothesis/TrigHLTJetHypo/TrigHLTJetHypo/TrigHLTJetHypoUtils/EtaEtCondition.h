/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ETAETCONDITION_H
#define TRIGHLTJETHYPO_ETAETCONDITION_H

/********************************************************************
 *
 * NAME:     EtaEtCondition.h
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
  EtaEtCondition(double etaMin,
                 double etaMax,
                 double threshold,
                 bool debug=false);
  virtual ~EtaEtCondition() override {}

  virtual bool isSatisfied(const HypoJetVector&) const override;
  bool isSatisfied(const pHypoJet&) const;

  virtual double orderingParameter() const noexcept override;

  virtual std::string toString() const noexcept override;
  virtual void resetHistory() noexcept override {}
 private:

  double m_etaMin;
  double m_etaMax;
  double m_threshold;
};

#endif
