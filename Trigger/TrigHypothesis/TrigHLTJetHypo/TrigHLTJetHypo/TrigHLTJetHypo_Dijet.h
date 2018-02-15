/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJET_H
#define TRIGHLTJETHYPO_DIJET_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_Dijet.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *
 * Selects events with dijet pairs.
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoBase.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_Dijet : public TrigHLTJetHypoBase {

 public:

  TrigHLTJetHypo_Dijet(const std::string& name, 
                               ISvcLocator* pSvcLocator);
  HLT::ErrorCode hltInitialize();

  ~TrigHLTJetHypo_Dijet();

  
  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  Conditions getConditions() const override;
  std::shared_ptr<IJetGrouper> getJetGrouper() const override;
  bool checkVals()  const override;

 private:

  std::vector<double> m_EtThresholds1;
  std::vector<double> m_EtThresholds2;
  std::vector<double> m_etaMins1;
  std::vector<double> m_etaMaxs1;
  std::vector<double> m_etaMins2;
  std::vector<double> m_etaMaxs2;
  std::vector<double> m_massMins;
  std::vector<double> m_massMaxs;
  std::vector<double> m_dEtaMins;
  std::vector<double> m_dEtaMaxs;
  std::vector<double> m_dPhiMins;
  std::vector<double> m_dPhiMaxs;

};
#endif
