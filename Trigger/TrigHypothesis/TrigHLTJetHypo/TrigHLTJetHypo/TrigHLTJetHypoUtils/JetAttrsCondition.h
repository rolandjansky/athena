/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETATTRSCONDITION_H
#define TRIGHLTJETHYPO_JETATTRSCONDITION_H

/********************************************************************
 *
 * NAME:     JetAttrsCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   A. Steinhebel
 * CREATED:  November 2018
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class JetAttrsCondition: public ICondition{
 public:
  JetAttrsCondition(
                 std::vector<std::string> jetVar,
                 std::vector<double> limitMin,
                 std::vector<double> limitMax
                 );

  ~JetAttrsCondition() override {}

  bool isSatisfied(const HypoJetVector&) const override;
  
  double orderingParameter() const noexcept override;
  
  std::string toString() const noexcept override;

 private:

  typedef struct {bool validCondition; double value;} Jet;
  bool passWidthCut(pHypoJet, int) const;
  bool passKtDRCut(pHypoJet, int) const;

  std::vector<std::string> m_jetVar;
  std::vector<double> m_limitMin;
  std::vector<double> m_limitMax;


};

#endif
