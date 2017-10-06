/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TLA_H
#define TRIGHLTJETHYPO_TLA_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_TLA.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoBase.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_TLA : public TrigHLTJetHypoBase {

 public:

  TrigHLTJetHypo_TLA(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetHypo_TLA();

  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  std::shared_ptr<IJetGrouper> getJetGrouper() const override;
  Conditions getConditions() const override;
  bool checkVals()  const override;

 private:

  // vector of indices find ofssets into the jet vector,
  // and other Condition variables used for TLA style hypos.
  std::vector<double> m_ystarMins;
  std::vector<double> m_ystarMaxs;
  std::vector<double> m_massMins;
  std::vector<double> m_massMaxs;
  std::vector<double> m_etaMins;
  std::vector<double> m_etaMaxs;
  std::vector<unsigned int> m_jetvec_indices; //indexed jets

};
#endif
