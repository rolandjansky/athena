/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRGHLTJETHYPO_JetAttrs_H
#define TRIGHLTJETHYPO_JetAttrs_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_JetAttrs.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   A. Steinhebel
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoBase.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetAttrsValueInterpreter.h"

class TrigHLTJetHypo_JetAttrs : public TrigHLTJetHypoBase {

 public:

  TrigHLTJetHypo_JetAttrs(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetHypo_JetAttrs();

  HLT::ErrorCode hltInitialize();

  std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  std::shared_ptr<IJetGrouper> getJetGrouper() const override;
  Conditions getConditions() const override;
  bool checkVals() const override;
  
//  std::vector<double> getStringBoundaries (const std::vector<std::string>& stv) const;

 private:
  // vectors with jet moment variables and the mins/maxes of their desired range
  std::vector<int> m_E;
  std::vector<std::string> m_jetVars;
  std::string m_momentstr;
  std::vector<std::string> m_has;
  std::vector<std::string> m_limitMins;
  std::vector<std::string> m_limitMaxs;
  std::map<std::string,std::unique_ptr<IJetAttrsValueInterpreter>> m_conversionMap;


  float GeV = 1000.;

};
#endif
