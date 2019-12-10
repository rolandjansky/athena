/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ETACONDITIONSIGNEDMT_H
#define TRIGHLTJETHYPO_ETACONDITIONSIGNEDMT_H


/********************************************************************
 *
 * NAME:     EtaConditionSignedMT.h
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

class EtaConditionSignedMT: public IConditionMT{
 public:
  EtaConditionSignedMT(double etaMin,
		       double etaMax);

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return s_capacity;}
  std::string toString() const noexcept override;
  
 private:

  double m_min;
  double m_max;
  bool isSatisfied(const pHypoJet&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const;
  
  const static  unsigned int s_capacity{1};
  
};

#endif
