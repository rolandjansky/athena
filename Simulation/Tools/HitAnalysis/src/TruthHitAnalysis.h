/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTH_HIT_ANALYSIS_H
#define TRUTH_HIT_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

class TH1;
class TH2;
class TTree;


class TruthHitAnalysis : public AthAlgorithm {

 public:

   TruthHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~TruthHitAnalysis(){}

   virtual StatusCode initialize();
   virtual StatusCode execute();

 private:

   /** Some variables**/ 
   TH1* m_h_n_vert;
   TH1* m_h_n_part;
   TH1* m_h_n_vert_prim; 
   TH1* m_h_n_part_prim;
   TH1* m_h_n_vert_sec;
   TH1* m_h_n_part_sec;
   TH1* m_h_vtx_x;
   TH1* m_h_vtx_y;
   TH1* m_h_vtx_z; 
   TH1* m_h_vtx_r;
   TH2* m_h_vtx_prim_xy;
   TH2* m_h_vtx_prim_zr;
   TH2* m_h_vtx_sec_xy;
   TH2* m_h_vtx_sec_zr;
   TH1* m_h_n_generations;
   TH1* m_h_truth_px;
   TH1* m_h_truth_py; 
   TH1* m_h_truth_pz;
   TH1* m_h_truth_pt;
   TH1* m_h_truth_eta;
   TH1* m_h_truth_phi; 
   TH1* m_h_barcode;
   TH1* m_h_part_status;
   TH1* m_h_part_pdgid;
   TH1* m_h_part_pdgid_sec;
   TH1* m_h_part_eta;
   TH1* m_h_part_phi;
   TH1* m_h_part_p;

   std::vector<float>* m_vtx_x;
   std::vector<float>* m_vtx_y;
   std::vector<float>* m_vtx_z;
   std::vector<float>* m_vtx_r;
   std::vector<float>* m_vtx_barcode;
   std::vector<float>* m_truth_px;
   std::vector<float>* m_truth_py;
   std::vector<float>* m_truth_pz;
   std::vector<float>* m_truth_pt;
   std::vector<float>* m_truth_eta;
   std::vector<float>* m_truth_phi;
   std::vector<float>* m_barcode;
   std::vector<float>* m_status;
   std::vector<float>* m_pdgid;

   TTree * m_tree;
   std::string m_ntupleFileName;    
   std::string m_path;
   ServiceHandle<ITHistSvc>  m_thistSvc;

};

#endif // TRUTH_HIT_ANALYSIS_H
