/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETMASSDETA_H
#define TRIGHLTJETHYPO_DIJETMASSDETA_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_DijetMassDeta.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoBase.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_DijetMassDEta : public TrigHLTJetHypoBase {

 public:

  TrigHLTJetHypo_DijetMassDEta(const std::string& name, 
                               ISvcLocator* pSvcLocator);

  ~TrigHLTJetHypo_DijetMassDEta();

  
  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  Conditions getConditions() const override;
  std::shared_ptr<IJetGrouper> getJetGrouper() const override;
  bool checkVals()  const override;

 private:

  std::vector<double> m_EtThresholds;
  std::vector<double> m_etaMins;
  std::vector<double> m_etaMaxs;
  std::vector<int> m_asymmetricEtas;
  std::vector<double> m_massMins;
  std::vector<double> m_massMaxs;
  std::vector<double> m_dEtaMins;
  std::vector<double> m_dEtaMaxs;

};
#endif
