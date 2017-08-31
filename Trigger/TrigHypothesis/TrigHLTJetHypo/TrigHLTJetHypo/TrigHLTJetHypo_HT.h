/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_HT_H
#define TRIGHLTJETHYPO_HT_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_HT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoBase.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_HT : public TrigHLTJetHypoBase {

 public:

  TrigHLTJetHypo_HT(const std::string& name,  ISvcLocator* pSvcLocator);

  ~TrigHLTJetHypo_HT();

  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  std::shared_ptr<IJetGrouper> getJetGrouper() const override;
  Conditions getConditions() const override;
  bool checkVals()  const override;
  
 private:

  std::vector<double> m_EtThresholds;
  std::vector<double> m_etaMins;
  std::vector<double> m_etaMaxs;

  double m_htMin;

};
#endif
