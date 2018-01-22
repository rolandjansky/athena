/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_HIT_ANALYSIS_H
#define ALFA_HIT_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TTree.h"

class TH1;
class TTree;


class ALFAHitAnalysis : public AthAlgorithm {

 public:

   ALFAHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~ALFAHitAnalysis(){}

   virtual StatusCode initialize();
   virtual StatusCode execute();

 private:

   /** Some variables**/
   TH1* m_h_E_full_sum_h[8];
   TH1* m_h_E_layer_sum_h[8];
   TH1* m_h_hit_layer[8];
   TH1* m_h_hit_fiber[8];
   
   std::vector<int>* m_station;
   std::vector<int>* m_plate;
   std::vector<int>* m_fiber;
   std::vector<int>* m_sign;
   std::vector<double>* m_energy;

   std::string m_collection;
   
   TTree * m_tree;
   std::string m_ntupleFileName;   
   std::string m_path; 
   ServiceHandle<ITHistSvc>  m_thistSvc;

};

#endif // ALFA_HIT_ANALYSIS_H
