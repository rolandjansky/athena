/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRGHLTJETHYPO_ETAET_H
#define TRIGHLTJETHYPO_ETAET_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_EtaEt.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoBase.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_EtaEt : public TrigHLTJetHypoBase {

 public:

  TrigHLTJetHypo_EtaEt(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetHypo_EtaEt();

  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  std::shared_ptr<IJetGrouper> getJetGrouper() const override;
  Conditions getConditions() const override;
  bool checkVals()  const override;

 private:
  // vectors with Et thresholds, eta mins and eta maxs
  // (thresh, eta min, eta max) triplets will bbe converted to Conditon objs.
  std::vector<double> m_EtThresholds;
  std::vector<double> m_etaMins;
  std::vector<double> m_etaMaxs;
  std::vector<int> m_asymmetricEtas;

};
#endif
