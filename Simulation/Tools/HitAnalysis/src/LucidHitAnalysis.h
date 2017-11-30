/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_HIT_ANALYSIS_H
#define LUCID_HIT_ANALYSIS_H

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


class LucidHitAnalysis : public AthAlgorithm {

 public:

   LucidHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~LucidHitAnalysis(){}

   virtual StatusCode initialize();
   virtual StatusCode execute();

 private:

   /** Some histograms**/
   TH1* m_h_hit_x;
   TH1* m_h_hit_y;
   TH1* m_h_hit_z;
   TH2* m_h_xy;
   TH2* m_h_zr;
   TH1* m_h_hit_post_x;
   TH1* m_h_hit_post_y;
   TH1* m_h_hit_post_z;
   TH1* m_h_hit_edep;
   TH1* m_h_hit_pdgid;
   TH1* m_h_hit_pretime;
   TH1* m_h_hit_posttime;
   TH1* m_h_genvolume;
   TH1* m_h_wavelength;

   std::vector<float>* m_hit_x;
   std::vector<float>* m_hit_y;
   std::vector<float>* m_hit_z;
   std::vector<float>* m_hit_post_x;
   std::vector<float>* m_hit_post_y;
   std::vector<float>* m_hit_post_z;
   std::vector<float>* m_hit_edep;
   std::vector<float>* m_hit_pdgid;
   std::vector<float>* m_hit_pretime;
   std::vector<float>* m_hit_posttime;
   std::vector<float>* m_gen_volume;
   std::vector<float>* m_wavelength;
   
   TTree * m_tree;
   std::string m_ntupleFileName; 
   std::string m_path; 
   ServiceHandle<ITHistSvc>  m_thistSvc;

};

#endif // LUCID_HIT_ANALYSIS_H
