/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   CaloCalibrationHitsTestTool.h
 * @author Chiara Debenedetti
 * @date    29/08/2011
 */

#ifndef G4ATLASTESTS_CALOCALIBRATIONHITSTESTTOOL_H
#define G4ATLASTESTS_CALOCALIBRATIONHITSTESTTOOL_H
 
#include "SimTestToolBase.h"

class CaloDetDescrManager;
 
class CaloCalibrationHitsTestTool : public SimTestToolBase {
 
 
 public:
 
  CaloCalibrationHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);
 
  StatusCode initialize(); 
 
  StatusCode processEvent();

 private:
  // keys
  std::string m_calibHitType;
  std::string m_hitcollkey;

  //--histos--//
  TH1 *m_eta, *m_phi, *m_eEM, *m_eNonEM, *m_eInv, *m_eEsc, *m_eTot;
  TH2 *m_rz, *m_etaphi;
  TH1 *m_eTot_partID, *m_eTot_eta, *m_eTot_phi;
  TH1 *m_partID_large, *m_partID_small;

  const CaloDetDescrManager* m_caloMgr{nullptr};
};
 
#endif
