/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ETAETTOOL_H
#define TRIGHLTJETHYPO_ETAETTOOL_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_EtaEtTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypoTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_EtaEtTool: virtual public ITrigHLTJetHypoTool,
  public TrigHLTJetHypoTool {
 public:

  TrigHLTJetHypo_EtaEtTool(const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);
  ~TrigHLTJetHypo_EtaEtTool();

  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  std::unique_ptr<IJetGrouper> getJetGrouper() const override;
  Conditions getConditions() const override;

 private:
  // vectors with Et thresholds, eta mins and eta maxs
  // (thresh, eta min, eta max) triplets will be converted to Conditon objs.
  std::vector<double> m_EtThresholds;
  std::vector<double> m_etaMins;
  std::vector<double> m_etaMaxs;
  std::vector<int> m_asymmetricEtas;

  StatusCode checkVals()  const override;

};
#endif
