/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_HIT_ANALYSIS_H
#define ZDC_HIT_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TTree.h"

class TH1;
class TTree;

 
class ZDCHitAnalysis : public AthAlgorithm {

 public:

   ZDCHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~ZDCHitAnalysis(){}

   virtual StatusCode initialize();
   virtual StatusCode execute();

 private:

   /** Some variables**/
   TH1*  m_h_zdc_sidea_0;
   TH1*  m_h_zdc_sidea_1;
   TH1*  m_h_zdc_sidea_2;
   TH1*  m_h_zdc_sidea_3;
   TH1*  m_h_zdc_sidec_0;
   TH1*  m_h_zdc_sidec_1;
   TH1*  m_h_zdc_sidec_2;
   TH1*  m_h_zdc_sidec_3;

   std::vector<int>* m_zdc_strip_side;
   std::vector<int>* m_zdc_strip_mod;
   std::vector<double>* m_zdc_strip_energy;

   std::vector<int>* m_zdc_pix_side;
   std::vector<int>* m_zdc_pix_mod;
   std::vector<double>* m_zdc_pix_energy;

   TTree * m_tree;
   std::string m_ntupleFileName; 
   std::string m_path;
   ServiceHandle<ITHistSvc>  m_thistSvc;

};

#endif // ZDC_HIT_ANALYSIS_H
