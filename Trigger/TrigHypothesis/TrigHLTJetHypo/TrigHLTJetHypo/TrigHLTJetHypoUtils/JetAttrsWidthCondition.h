/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETATTRSWIDTHCONDITION_H
#define TRIGHLTJETHYPO_JETATTRSWIDTHCONDITION_H

/********************************************************************
 *
 * NAME:     JetAttrsWidthCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   A. Steinhebel
 * CREATED:  April 2018
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class JetAttrsWidthCondition: public ICondition{
 public:
  JetAttrsWidthCondition(
        double limitMin,
        double limitMax
                );

  ~JetAttrsWidthCondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  
  double orderingParameter() const noexcept override;
  
  std::string toString() const noexcept override;

 private:

  typedef struct {bool validCondition; double widthVal;} Jet;
  bool passWidthCut(const Jet&) const;

  double m_limitMin;
  double m_limitMax;
};

#endif

