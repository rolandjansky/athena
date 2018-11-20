/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FALSECONDITION_H
#define TRIGHLTJETHYPO_FALSECONDITION_H

/********************************************************************
 *
 * NAME:     FalseCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   A. Steinhebel
 * CREATED:  May 2018
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class FalseCondition: public ICondition{
 public:
  FalseCondition(double limitMin, 
         double limitMax);

  ~FalseCondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  
  double orderingParameter() const noexcept override;
  
  std::string toString() const noexcept override;

 private:
  double m_limitMin;
  double m_limitMax;

};

#endif

