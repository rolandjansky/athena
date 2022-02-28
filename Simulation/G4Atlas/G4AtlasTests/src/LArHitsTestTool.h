/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTESTS_LARHITSTESTTOOL
#define G4ATLASTESTS_LARHITSTESTTOOL

#include "SimTestToolBase.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class LArHitsTestTool : public SimTestToolBase {


public:

  LArHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode initialize() override; 

  virtual StatusCode processEvent() override;

 protected:
  std::string m_detname;
  double m_edep_cut;

  // globals
  TH1 *m_lar_eta, *m_lar_phi;
  TH2 *m_lar_zr, *m_lar_etaphi;
  TH2 *m_lar_edep_zr;

  // locals (EMB EMEC FCAL HEC)
  TH1 *m_eta, *m_phi;
  TH2 *m_zr, *m_etaphi;
  TH1 *m_time, *m_edep, *m_log_edep;
  TH2 *m_edep_zr;
  TH1 *m_eta_cut1, *m_phi_cut1;
  TH2 *m_zr_cut1, *m_etaphi_cut1;
  TH1 *m_time_cut1, *m_edep_cut1;
  TH2 *m_edep_zr_cut1;

  TH1 *m_etot;
  TH1 *m_edep_eta, *m_edep_phi;
  TH1 *m_edep_z, *m_edep_r;
  TH1 *m_etot_eta, *m_etot_phi;

 private:
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

};

#endif
