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

class ITrigJetHypoInfoCollector;

class EtaEtConditionMT: public IConditionMT{
 public:
  EtaEtConditionMT(double etaMin,
                 double etaMax,
		   double threshold0);

  ~EtaEtConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return s_capacity;}
  std::string toString() const noexcept override;
  
 private:

  double m_etaMin;
  double m_etaMax;
  double m_threshold;

  bool isSatisfied(const pHypoJet&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const;
  
  const static  unsigned int s_capacity{1};
  
};

#endif
