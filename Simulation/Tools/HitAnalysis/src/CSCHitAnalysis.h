/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSC_HIT_ANALYSIS_H
#define CSC_HIT_ANALYSIS_H

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
 
class CSCHitAnalysis : public AthAlgorithm {

 public:

   CSCHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~CSCHitAnalysis(){}

   virtual StatusCode initialize();
   virtual StatusCode execute();

 private:

   /** Some variables**/
   TH1* h_hits_x;
   TH1* h_hits_y;
   TH1* h_hits_z;
   TH1* h_hits_r;
   TH2* h_xy;
   TH2* h_zr;
   TH1* h_hits_eta;
   TH1* h_hits_phi;
   TH1* h_hits_sx;
   TH1* h_hits_sy;
   TH1* h_hits_sz;
   TH1* h_hits_ex;
   TH1* h_hits_ey;
   TH1* h_hits_ez;
   TH1* h_hits_time;
   TH1* h_hits_edep;
   TH1* h_hits_kine;

   std::vector<float>* m_hits_x;
   std::vector<float>* m_hits_y;
   std::vector<float>* m_hits_z;
   std::vector<float>* m_hits_r;
   std::vector<float>* m_hits_eta;
   std::vector<float>* m_hits_phi;
   std::vector<float>* m_hits_start_x;
   std::vector<float>* m_hits_start_y;
   std::vector<float>* m_hits_start_z;
   std::vector<float>* m_hits_end_x;
   std::vector<float>* m_hits_end_y;
   std::vector<float>* m_hits_end_z;
   std::vector<float>* m_hits_time;
   std::vector<float>* m_hits_edep;
   std::vector<float>* m_hits_kine;
      
   TTree * m_tree;
   std::string m_ntupleFileName;
   std::string m_path; 
   ServiceHandle<ITHistSvc>  m_thistSvc;

};

#endif // CSC_HIT_ANALYSIS_H

