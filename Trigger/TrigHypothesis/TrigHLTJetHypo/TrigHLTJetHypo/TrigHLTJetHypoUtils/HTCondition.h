/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_HTCONDITION_H
#define TRIGHLTJETHYPO_HTCONDITION_H

/********************************************************************
 *
 * NAME:     HTCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  September 14 2016
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include <string>
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"

namespace HypoJet{
  class IJet;
}

class HTCondition: public ICondition{
 public:
  HTCondition(double htMin);
  virtual ~HTCondition() override {}

  virtual bool isSatisfied(const HypoJetVector&) const override;

  virtual double orderingParameter() const noexcept override;

  virtual std::string toString() const noexcept override;
  virtual void resetHistory() noexcept override {}

 private:

  bool isSatisfied(const pHypoJet&) const;

  double m_htMin;
};

#endif
