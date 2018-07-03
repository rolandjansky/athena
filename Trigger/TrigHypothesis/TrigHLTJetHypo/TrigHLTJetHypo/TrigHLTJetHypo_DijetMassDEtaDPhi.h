/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETMASSDETADPHI_H
#define TRIGHLTJETHYPO_DIJETMASSDETADPHI_H
/********************************************************************
 *
 * NAME:     Trighltjethypo_DijetMassDetaDPhi.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypo_DijetMassDEta.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigHLTJetHypo_DijetMassDEtaDPhi : public TrigHLTJetHypo_DijetMassDEta {

 public:

  TrigHLTJetHypo_DijetMassDEtaDPhi(const std::string& name, 
                                   ISvcLocator* pSvcLocator);

  ~TrigHLTJetHypo_DijetMassDEtaDPhi();

  
  Conditions getConditions() const override;
  bool checkVals()  const override;

 private:

  std::vector<double> m_dPhiMins;
  std::vector<double> m_dPhiMaxs;

};
#endif
