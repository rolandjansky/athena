/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETDPHICONDITION_H
#define TRIGHLTJETHYPO_DIJETDPHICONDITION_H

/********************************************************************
 *
 * NAME:     DijetDPhiCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  January, 2018
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class DijetDPhiCondition: public ICondition{
 public:
  DijetDPhiCondition(const std::vector<double>& dPhiMins,
                     const std::vector<double>& dPhiMaxs);

  
  ~DijetDPhiCondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  
  double orderingParameter() const noexcept override;
  
  std::string toString() const noexcept override;

 private:

  double m_dPhiMin;
  double m_dPhiMax;
};

#endif
