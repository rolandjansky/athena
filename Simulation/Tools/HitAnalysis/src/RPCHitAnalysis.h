/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_HIT_ANALYSIS_H
#define RPC_HIT_ANALYSIS_H

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


class RPCHitAnalysis : public AthAlgorithm {

 public:

   RPCHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~RPCHitAnalysis(){}

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
   TH1* m_h_hits_lx;
   TH1* m_h_hits_ly;
   TH1* m_h_hits_lz;
   TH1* m_h_hits_time;
   TH1* m_h_hits_edep;
   TH1* m_h_hits_kine;
   TH1* m_h_hits_step;

   std::vector<float>* m_hits_x;
   std::vector<float>* m_hits_y;
   std::vector<float>* m_hits_z;
   std::vector<float>* m_hits_r;
   std::vector<float>* m_hits_eta;
   std::vector<float>* m_hits_phi;
   std::vector<float>* m_hits_lx;
   std::vector<float>* m_hits_ly;
   std::vector<float>* m_hits_lz;
   std::vector<float>* m_hits_time;
   std::vector<float>* m_hits_edep;
   std::vector<float>* m_hits_kine;
   std::vector<float>* m_hits_step;
      
   TTree * m_tree;
   std::string m_ntupleFileName; 
   std::string m_path;
   ServiceHandle<ITHistSvc>  m_thistSvc;

};

#endif // RPC_HIT_ANALYSIS_H
