/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_HTCONDITIONMT_H
#define TRIGHLTJETHYPO_HTCONDITIONMT_H

/********************************************************************
 *
 * NAME:     HTConditionMT.h
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

class HTConditionMT: public IConditionMT{
 public:
  HTConditionMT(double htMin);
  ~HTConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   std::unique_ptr<IConditionVisitor>&) const override;

  std::string toString() const noexcept override;

 private:

  bool isSatisfied(const pHypoJet&, IConditionVisitor*) const;

  double m_htMin;
};

#endif
