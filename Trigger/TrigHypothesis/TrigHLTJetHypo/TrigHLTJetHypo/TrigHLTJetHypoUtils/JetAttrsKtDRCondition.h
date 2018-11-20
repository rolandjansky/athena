/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETATTRSKTDRCONDITION_H
#define TRIGHLTJETHYPO_JETATTRSKTDRCONDITION_H

/********************************************************************
 *
 * NAME:     JetAttrsKtDRCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   A. Steinhebel
 * CREATED:  April 2018
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class JetAttrsKtDRCondition: public ICondition{
 public:
  JetAttrsKtDRCondition(
        double limitMin,
        double limitMax
                );

  ~JetAttrsKtDRCondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  
  double orderingParameter() const noexcept override;
  
  std::string toString() const noexcept override;

 private:

  typedef struct {bool validCondition; double ktdrVal;} Jet;
  bool passKtDRCut(const Jet&) const;

  double m_limitMin;
  double m_limitMax;
};

#endif

