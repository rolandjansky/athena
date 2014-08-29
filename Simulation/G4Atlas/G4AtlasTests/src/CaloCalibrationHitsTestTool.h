/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//==============================================================//
// Chiara Debenedetti, 29/08/2011, CaloCalibrationHitsTestTool.h //
//==============================================================//

#ifndef G4AT_CALOCALIBRATIONHITSTESTTOOL
#define G4AT_CALOCALIBRATIONHITSTESTTOOL
 
#include "SimTestToolBase.h"

 
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
};
 
#endif
