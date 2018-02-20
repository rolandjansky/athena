/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_RECORD_ANALYSIS_H
#define TRACK_RECORD_ANALYSIS_H

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

 
class TrackRecordAnalysis : public AthAlgorithm {

 public:

   TrackRecordAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~TrackRecordAnalysis(){}

   virtual StatusCode initialize();
   virtual StatusCode execute();

 private:
   
   /** Some variables**/
   TH1* m_h_hits_x;
   TH1* m_h_hits_y;
   TH1* m_h_hits_z;
   TH1* m_h_hits_r;
   TH2* m_h_xy;
   TH2* m_h_zr;
   TH1* m_h_hits_eta;
   TH1* m_h_hits_phi;
   TH1* m_h_hits_px;
   TH1* m_h_hits_py;
   TH1* m_h_hits_pz;
   TH1* m_h_hits_pt;
   TH1* m_h_time;
   TH1* m_h_edep;
   TH1* m_h_pdg;

   std::vector<float>* m_x;
   std::vector<float>* m_y;
   std::vector<float>* m_z;
   std::vector<float>* m_r;
   std::vector<float>* m_eta;
   std::vector<float>* m_phi;
   std::vector<float>* m_px;
   std::vector<float>* m_py;
   std::vector<float>* m_pz;
   std::vector<float>* m_pt;
   std::vector<float>* m_time;
   std::vector<float>* m_edep;
   std::vector<float>* m_pdg;

   std::string m_collection;
        
   TTree * m_tree;
   std::string m_ntupleFileName; 
   std::string m_path; 
   ServiceHandle<ITHistSvc>  m_thistSvc;

};

#endif // TRACK_RECORD_ANALYSIS_H
