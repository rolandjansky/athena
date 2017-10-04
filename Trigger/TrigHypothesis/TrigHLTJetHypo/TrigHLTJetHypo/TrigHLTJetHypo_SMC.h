/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_SMC_H
#define TRIGHLTJETHYPO_SMC_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_SMC.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   G. Marceca
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoBase.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_SMC : public TrigHLTJetHypoBase {

 public:

  TrigHLTJetHypo_SMC(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetHypo_SMC();

  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  std::shared_ptr<IJetGrouper> getJetGrouper() const override;
  Conditions getConditions() const override;
  bool checkVals()  const override;
  
  std::vector<double> getStringBoundaries (const std::vector<std::string>& stv) const;

 private:
  // vectors with Et thresholds, eta mins, eta maxs, mass min and mass max
  // (thresh, eta min, eta max, mass min, mass max) triplets will bbe converted to Conditon objs.
  std::vector<double> m_EtThresholds;
  std::vector<double> m_etaMins;
  std::vector<double> m_etaMaxs;
  std::vector<std::string> m_JetMassMin;
  std::vector<std::string> m_JetMassMax;
  std::vector<int> m_asymmetricEtas;

  static constexpr float s_GeV = 1000.;

};
#endif
