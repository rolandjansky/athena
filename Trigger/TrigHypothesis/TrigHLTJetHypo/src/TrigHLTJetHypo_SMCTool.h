/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRGHLTJETHYPO_SMCTOOL_H
#define TRIGHLTJETHYPO_SMCTOOL_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_SMC.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   G. Marceca
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypoTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_SMCTool : public TrigHLTJetHypoTool {

 public:

  TrigHLTJetHypo_SMCTool(const std::string& type, 
                         const std::string& name, 
                         const IInterface* parent);
  ~TrigHLTJetHypo_SMCTool();

  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  std::unique_ptr<IJetGrouper> getJetGrouper() const override;
  Conditions getConditions() const override;
  StatusCode checkVals()  const override;
  
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
