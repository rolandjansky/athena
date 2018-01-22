/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_HIT_ANALYSIS_H
#define TRT_HIT_ANALYSIS_H

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


class TRTHitAnalysis : public AthAlgorithm {

 public:

   TRTHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~TRTHitAnalysis(){}

   virtual StatusCode initialize();
   virtual StatusCode execute();

 private:

   /** Some variables**/
   TH1* m_h_TRT_x;
   TH1* m_h_TRT_y;
   TH1* m_h_TRT_z;
   TH1* m_h_TRT_r;
   TH2* m_h_TRT_xy;
   TH2* m_h_TRT_zr;
   TH1* m_h_TRT_time_photons;
   TH1* m_h_TRT_time_nonphotons;
   TH1* m_h_TRT_edep_photons;
   TH1* m_h_TRT_edep_nonphotons;
   TH1* m_h_TRT_kine_photons;
   TH1* m_h_TRT_kine_nonphotons;
   TH1* m_h_TRT_barcode;

   std::vector<float>* m_TRT_x;
   std::vector<float>* m_TRT_y;
   std::vector<float>* m_TRT_z;
   std::vector<float>* m_TRT_r;
   std::vector<float>* m_TRT_time_photons;
   std::vector<float>* m_TRT_time_nonphotons;
   std::vector<float>* m_TRT_edep_photons;
   std::vector<float>* m_TRT_edep_nonphotons;
   std::vector<float>* m_TRT_kine_photons;
   std::vector<float>* m_TRT_kine_nonphotons;
   std::vector<float>* m_TRT_barcode;
   
   TTree * m_tree;
   std::string m_path;
   std::string m_ntupleFileName; 
   ServiceHandle<ITHistSvc> m_thistSvc;

};

#endif // TRT_HIT_ANALYSIS_H
