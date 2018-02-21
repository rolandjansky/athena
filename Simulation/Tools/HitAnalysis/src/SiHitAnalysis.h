/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SI_HIT_ANALYSIS_H
#define SI_HIT_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"

class TH1;
class TH2;
class TTree;


class SiHitAnalysis : public AthAlgorithm {

 public:

   SiHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~SiHitAnalysis(){}

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
   TH1* m_h_hits_time;
   TH1* m_h_hits_eloss;
   TH1* m_h_hits_step;
   TH1* m_h_hits_barcode;
   TH2* m_h_time_eloss;
   TH2* m_h_z_eloss;
   TH2* m_h_r_eloss;
   
   std::vector<float>* m_hits_x;
   std::vector<float>* m_hits_y;
   std::vector<float>* m_hits_z;
   std::vector<float>* m_hits_r;
   std::vector<float>* m_hits_time;
   std::vector<float>* m_hits_eloss;
   std::vector<float>* m_hits_step;
   std::vector<float>* m_hits_barcode;

   std::string m_collection;
   std::string m_expert;  
   
   TTree* m_tree;
   std::string m_ntupleFileName; 
   std::string m_path;
   ServiceHandle<ITHistSvc>  m_thistSvc;

};

#endif // SI_HIT_ANALYSIS_H
