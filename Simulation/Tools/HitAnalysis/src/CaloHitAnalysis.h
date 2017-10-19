/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALO_HIT_ANALYSIS_H
#define CALO_HIT_ANALYSIS_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

// Members
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// STL includes
#include <string>
//#include <vector>

class TileID;
class TileDetDescrManager;
class TH1;
class TH2;
class TTree;


class CaloHitAnalysis : public AthAlgorithm {

 public:

   CaloHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~CaloHitAnalysis() {}

   virtual StatusCode initialize();
   virtual StatusCode execute();

 private:

   /** Simple variables by Ketevi */
   TH1* m_h_cell_eta;
   TH1* m_h_cell_phi;
   TH1* m_h_cell_e;
   TH1* m_h_cell_radius;
   TH2* m_h_xy;
   TH2* m_h_zr;
   TH2* m_h_etaphi;
   TH2* m_h_time_e;
   TH2* m_h_eta_e;
   TH2* m_h_phi_e;
   TH2* m_h_r_e;
   TH1* m_h_calib_eta;
   TH1* m_h_calib_phi;
   TH2* m_h_calib_rz;
   TH2* m_h_calib_etaphi;
   TH1* m_h_calib_eEM;
   TH1* m_h_calib_eNonEM;
   TH1* m_h_calib_eInv;
   TH1* m_h_calib_eEsc;
   TH1* m_h_calib_eTot;
   TH1* m_h_calib_eTotpartID;

   const TileID * m_tileID;
   const TileDetDescrManager * m_tileMgr;

   std::vector<float>* m_cell_eta;
   std::vector<float>* m_cell_phi;
   std::vector<float>* m_cell_x;
   std::vector<float>* m_cell_y;
   std::vector<float>* m_cell_z;
   std::vector<float>* m_cell_e;
   std::vector<float>* m_cell_radius;
   std::vector<float>* m_time;
   std::vector<float>* m_calib_eta;
   std::vector<float>* m_calib_phi;
   std::vector<float>* m_calib_radius;
   std::vector<float>* m_calib_z;
   std::vector<float>* m_calib_eEM;
   std::vector<float>* m_calib_eNonEM;
   std::vector<float>* m_calib_eInv;
   std::vector<float>* m_calib_eEsc;
   std::vector<float>* m_calib_eTot;
   std::vector<float>* m_calib_partID;
   
   std::string m_expert;    
   std::string m_calib;    
   
   TTree* m_tree;
   std::string m_ntupleFileName;
   std::string m_path;
   ServiceHandle<ITHistSvc> m_thistSvc;
   
};

#endif // CALO_HIT_ANALYSIS_H
