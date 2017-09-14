/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_HIT_ANALYSIS_H
#define AFP_HIT_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TTree.h"

class TH1;
class TTree;


class AFPHitAnalysis : public AthAlgorithm {

 public:

   AFPHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~AFPHitAnalysis(){}

   virtual StatusCode initialize();
   virtual StatusCode execute();

 private:

   /** Some histograms**/
   TH1*   h_hitID;
   TH1*   h_pdgID;
   TH1*   h_trackID;
   TH1*   h_kine;
   TH1*   h_edep;
   TH1*   h_stepX;
   TH1*   h_stepY;
   TH1*   h_stepZ;
   TH1*   h_time;
   TH1*   h_stationID;
   TH1*   h_detID;
   TH1*   h_pixelRow;
   TH1*   h_pixelCol;

   std::vector<float>*   m_hitID;
   std::vector<float>*   m_pdgID;
   std::vector<float>*   m_trackID;
   std::vector<float>*   m_kine;
   std::vector<float>*   m_edep;
   std::vector<float>*   m_stepX;
   std::vector<float>*   m_stepY;
   std::vector<float>*   m_stepZ;
   std::vector<float>*   m_time;
   std::vector<int>*   m_stationID;
   std::vector<int>*   m_detID;
   std::vector<int>*   m_pixelRow;
   std::vector<int>*   m_pixelCol;
   
   TTree * m_tree;
   std::string m_ntupleFileName;
   std::string m_path; 
   ServiceHandle<ITHistSvc>  m_thistSvc;

};

#endif // AFP_HIT_ANALYSIS_H
