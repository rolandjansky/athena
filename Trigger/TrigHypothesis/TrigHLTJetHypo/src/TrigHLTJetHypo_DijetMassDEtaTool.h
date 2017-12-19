/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETMASSDETATOOL_H
#define TRIGHLTJETHYPO_DIJETMASSDETATOOL_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_DijetMassDetaTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypoTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_DijetMassDEtaTool : virtual public ITrigHLTJetHypoTool,
  public TrigHLTJetHypoTool {

 public:

  TrigHLTJetHypo_DijetMassDEtaTool(const std::string& type, 
                                   const std::string& name,
                                   const IInterface* parent);

  ~TrigHLTJetHypo_DijetMassDEtaTool();

  
  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  Conditions getConditions() const override;
  std::unique_ptr<IJetGrouper> getJetGrouper() const override;

 private:

  std::vector<double> m_EtThresholds;
  std::vector<double> m_etaMins;
  std::vector<double> m_etaMaxs;
  std::vector<int> m_asymmetricEtas;
  std::vector<double> m_massMins;
  std::vector<double> m_massMaxs;
  std::vector<double> m_dEtaMins;
  std::vector<double> m_dEtaMaxs;

  StatusCode checkVals()  const override;
};
#endif
