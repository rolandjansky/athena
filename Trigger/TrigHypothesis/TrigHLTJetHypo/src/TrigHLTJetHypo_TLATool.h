/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TLATOOL_H
#define TRIGHLTJETHYPO_TLATOOL_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_TLATool.h
 * PACKAGE:  Trigger/TrigHypothesisg/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypoTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_TLATool : virtual public ITrigHLTJetHypoTool,
  public TrigHLTJetHypoTool {

 public:

  TrigHLTJetHypo_TLATool(const std::string& type,
                         const std::string& name,
                         const IInterface* parent);
  ~TrigHLTJetHypo_TLATool();

  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  std::unique_ptr<IJetGrouper> getJetGrouper() const override;
  Conditions getConditions() const override;
  StatusCode checkVals()  const override;

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
