/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ETACONDITIONSIGNED_H
#define TRIGHLTJETHYPO_ETACONDITIONSIGNED_H


/********************************************************************
 *
 * NAME:     EtaConditionSigned.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *********************************************************************/

#include <string>
#include "./ICondition.h"

namespace HypoJet{
  class IJet;
}

class ITrigJetHypoInfoCollector;

class EtaConditionSigned: public ICondition{
 public:
  EtaConditionSigned(double etaMin,
		       double etaMax);

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual unsigned int capacity() const override{return s_capacity;}
  std::string toString() const override;
  
 private:

  double m_min;
  double m_max;
  bool isSatisfied(const pHypoJet&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const;
  
  const static  unsigned int s_capacity{1};
  
};

#endif
