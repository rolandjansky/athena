/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRT_FAST_RDO_ANALYSIS_H
#define TRT_FAST_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class TH1;

class TRT_FastRDOAnalysis : public AthAlgorithm {

 public:
  TRT_FastRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~TRT_FastRDOAnalysis(){}

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  // TRT_DriftCircle
  std::vector<unsigned int>* m_word;
  std::vector<int>* m_driftTimeBin;
  std::vector<int>* m_trailEdge;
  std::vector<bool>* m_highLevel;
  std::vector<bool>* m_firstBin;
  std::vector<bool>* m_lastBin;
  std::vector<double>* m_timeOverThreshold;
  std::vector<double>* m_rawDriftTime;
  std::vector<bool>* m_driftTimeValid;
  std::vector<bool>* m_noise;
  // TRT_BaseElement
  std::vector<int>* m_brl_ec;
  std::vector<unsigned long long>* m_trtID;
  std::vector<int>* m_surfType;
  std::vector<int>* m_bndsType;
  std::vector<float>* m_cntr_x;
  std::vector<float>* m_cntr_y;
  std::vector<float>* m_cntr_z;
  std::vector<float>* m_cntr_r;
  std::vector<float>* m_cntr_phi;
  std::vector<float>* m_norm_x;
  std::vector<float>* m_norm_y;
  std::vector<float>* m_norm_z;
  std::vector<float>* m_norm_r;
  std::vector<float>* m_norm_phi;
  std::vector<int>* m_strSurfType;
  std::vector<int>* m_strBndsType;
  std::vector<float>* m_strCntr_x;
  std::vector<float>* m_strCntr_y;
  std::vector<float>* m_strCntr_z;
  std::vector<float>* m_strCntr_r;
  std::vector<float>* m_strCntr_phi;
  std::vector<float>* m_strNorm_x;
  std::vector<float>* m_strNorm_y;
  std::vector<float>* m_strNorm_z;
  std::vector<float>* m_strNorm_r;
  std::vector<float>* m_strNorm_phi;
  std::vector<int>* m_strawN;
  std::vector<double>* m_strawL;
  std::vector<double>* m_strawD;
  std::vector<float>* m_allStrCntr_x;
  std::vector<float>* m_allStrCntr_y;
  std::vector<float>* m_allStrCntr_z;
  std::vector<float>* m_allStrCntr_r;
  std::vector<float>* m_allStrCntr_phi;
  std::vector<float>* m_allStrAxis_x;
  std::vector<float>* m_allStrAxis_y;
  std::vector<float>* m_allStrAxis_z;
  std::vector<float>* m_allStrAxis_r;
  std::vector<float>* m_allStrAxis_phi;

  // HISTOGRAMS
  TH1* h_word;
  TH1* h_driftTimeBin;
  TH1* h_trailEdge;
  TH1* h_highLevel;
  TH1* h_firstBin;
  TH1* h_lastBin;
  TH1* h_timeOverThreshold;
  TH1* h_rawDriftTime;
  TH1* h_driftTimeValid;
  TH1* h_noise;

  TH1* h_brl_ec;
  TH1* h_trtID;
  TH1* h_surfType;
  TH1* h_bndsType;
  TH1* h_cntr_x;
  TH1* h_cntr_y;
  TH1* h_cntr_z;
  TH1* h_cntr_r;
  TH1* h_cntr_phi;
  TH1* h_norm_x;
  TH1* h_norm_y;
  TH1* h_norm_z;
  TH1* h_norm_r;
  TH1* h_norm_phi;
  TH1* h_strSurfType;
  TH1* h_strBndsType;
  TH1* h_strCntr_x;
  TH1* h_strCntr_y;
  TH1* h_strCntr_z;
  TH1* h_strCntr_r;
  TH1* h_strCntr_phi;
  TH1* h_strNorm_x;
  TH1* h_strNorm_y;
  TH1* h_strNorm_z;
  TH1* h_strNorm_r;
  TH1* h_strNorm_phi;
  TH1* h_strawN;
  TH1* h_strawL;
  TH1* h_strawD;
  TH1* h_allStrCntr_x;
  TH1* h_allStrCntr_y;
  TH1* h_allStrCntr_z;
  TH1* h_allStrCntr_r;
  TH1* h_allStrCntr_phi;
  TH1* h_allStrAxis_x;
  TH1* h_allStrAxis_y;
  TH1* h_allStrAxis_z;
  TH1* h_allStrAxis_r;
  TH1* h_allStrAxis_phi;

 private:
  TTree* m_tree;
  std::string m_ntupleFileName;
  std::string m_ntupleDirName;
  std::string m_ntupleTreeName;
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;
};

#endif // TRT_FAST_RDO_ANALYSIS_H
