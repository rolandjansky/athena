/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HITANALYSIS_CALOHITANALYSIS_H
#define HITANALYSIS_CALOHITANALYSIS_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

// Members
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

// STL includes
#include <string>

class TileID;
class TileDetDescrManager;
class TH1;
class TH2;
class TTree;


class CaloHitAnalysis : public AthAlgorithm {

 public:

   CaloHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~CaloHitAnalysis() {}

   virtual StatusCode initialize() override;
   virtual StatusCode execute() override;

 private:

   /** Simple variables by Ketevi */
   TH1* m_h_cell_eta{nullptr};
   TH1* m_h_cell_phi{nullptr};
   TH1* m_h_cell_e{nullptr};
   TH1* m_h_cell_radius{nullptr};
   TH2* m_h_xy{nullptr};
   TH2* m_h_zr{nullptr};
   TH2* m_h_etaphi{nullptr};
   TH2* m_h_time_e{nullptr};
   TH2* m_h_eta_e{nullptr};
   TH2* m_h_phi_e{nullptr};
   TH2* m_h_r_e{nullptr};
   TH1* m_h_calib_eta{nullptr};
   TH1* m_h_calib_phi{nullptr};
   TH2* m_h_calib_rz{nullptr};
   TH2* m_h_calib_etaphi{nullptr};
   TH1* m_h_calib_eEM{nullptr};
   TH1* m_h_calib_eNonEM{nullptr};
   TH1* m_h_calib_eInv{nullptr};
   TH1* m_h_calib_eEsc{nullptr};
   TH1* m_h_calib_eTot{nullptr};
   TH1* m_h_calib_eTotpartID{nullptr};

   const TileID * m_tileID{nullptr};
   const TileDetDescrManager * m_tileMgr{nullptr};

   std::vector<float>* m_cell_eta{nullptr};
   std::vector<float>* m_cell_phi{nullptr};
   std::vector<float>* m_cell_x{nullptr};
   std::vector<float>* m_cell_y{nullptr};
   std::vector<float>* m_cell_z{nullptr};
   std::vector<float>* m_cell_e{nullptr};
   std::vector<float>* m_cell_radius{nullptr};
   std::vector<float>* m_time{nullptr};
   std::vector<float>* m_calib_eta{nullptr};
   std::vector<float>* m_calib_phi{nullptr};
   std::vector<float>* m_calib_radius{nullptr};
   std::vector<float>* m_calib_z{nullptr};
   std::vector<float>* m_calib_eEM{nullptr};
   std::vector<float>* m_calib_eNonEM{nullptr};
   std::vector<float>* m_calib_eInv{nullptr};
   std::vector<float>* m_calib_eEsc{nullptr};
   std::vector<float>* m_calib_eTot{nullptr};
   std::vector<float>* m_calib_partID{nullptr};
   
   std::string m_expert;    
   std::string m_calib;    
   
   TTree* m_tree{nullptr};
   std::string m_ntupleFileName;
   std::string m_path;
   ServiceHandle<ITHistSvc> m_thistSvc;
   SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
       , "CaloDetDescrManager"
       , "CaloDetDescrManager"
       , "SG Key for CaloDetDescrManager in the Condition Store" };

};

#endif // CALO_HIT_ANALYSIS_H
