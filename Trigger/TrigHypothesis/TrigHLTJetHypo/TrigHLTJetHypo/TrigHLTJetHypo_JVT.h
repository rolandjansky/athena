/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
//
// NAME:     Trighltjethypo_JVT.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
// AUTHOR:   Jona Bossio
//
// Selects events with Njets passing ET, eta and JVT requirements
//
//////////////////////////////////////////////////////////////////////

#ifndef TRIGHLTJETHYPO_JVT_H
#define TRIGHLTJETHYPO_JVT_H

#include "TrigHLTJetHypo/TrigHLTJetHypoBase.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_JVT : public TrigHLTJetHypoBase {

 public:

  // Constructor
  TrigHLTJetHypo_JVT(const std::string& name, ISvcLocator* pSvcLocator);

  HLT::ErrorCode hltInitialize();

  // Destructor
  ~TrigHLTJetHypo_JVT();

  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  Conditions getConditions() const override;
  std::shared_ptr<IJetGrouper> getJetGrouper() const override;
  bool checkVals()  const override;

 private:

  int m_NJets;
  double m_ETmin;
  double m_workingPoint;
  double m_scaleWP = 0.01;

};
#endif
