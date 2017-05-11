/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PIXEL_FAST_RDO_ANALYSIS_H
#define PIXEL_FAST_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class TH1;

class PixelFastRDOAnalysis : public AthAlgorithm {

 public:
  PixelFastRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelFastRDOAnalysis(){}

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
  // PixelCluster
  std::vector<float>* m_omegax;
  std::vector<float>* m_omegay;
  std::vector<int>* m_tot;
  std::vector<int>* m_totalTot;
  std::vector<float>* m_charge;
  std::vector<float>* m_totalCharge;
  std::vector<float>* m_eLoss;
  std::vector<bool>* m_fake;
  std::vector<bool>* m_ambig;
  std::vector<bool>* m_split;
  std::vector<double>* m_splitProb1;
  std::vector<double>* m_splitProb2;
  std::vector<int>* m_splitInfoRaw;
  std::vector<int>* m_LVL1A;

  // SiCluster - SiWidth + SiDetectorElement
  std::vector<float>* m_siCol;
  std::vector<float>* m_siRow;
  std::vector<float>* m_siPhiR;
  std::vector<float>* m_siZ;
  std::vector<float>* m_siPos_x;
  std::vector<float>* m_siPos_y;
  std::vector<float>* m_siPos_z;
  std::vector<bool>* m_siGangPix;
  std::vector<unsigned long long>* m_siDetID;
  std::vector<bool>* m_siDetPix;
  std::vector<bool>* m_siDetSCT;
  std::vector<bool>* m_siDetBrl;
  std::vector<bool>* m_siDetEc;
  std::vector<bool>* m_siDetBlay;
  std::vector<bool>* m_siDetInPixLay;
  std::vector<bool>* m_siDetNtInPixLay;
  std::vector<bool>* m_siDetDBM;
  std::vector<double>* m_siDetHitDepthDir;
  std::vector<double>* m_siDetHitPhiDir;
  std::vector<double>* m_siDetHitEtaDir;
  std::vector<double>* m_siDetMinR;
  std::vector<double>* m_siDetMaxR;
  std::vector<double>* m_siDetMinZ;
  std::vector<double>* m_siDetMaxZ;
  std::vector<double>* m_siDetMinPhi;
  std::vector<double>* m_siDetMaxPhi;
  std::vector<double>* m_siDetWidth;
  std::vector<double>* m_siDetMinWidth;
  std::vector<double>* m_siDetMaxWidth;
  std::vector<double>* m_siDetLength;
  std::vector<double>* m_siDetThick;
  std::vector<double>* m_siDetEtaPitch;
  std::vector<double>* m_siDetPhiPitch;

  // Trk::PrepRawData
  std::vector<unsigned long long>* m_clusID;
  std::vector<float>* m_locpos_x;
  std::vector<float>* m_locpos_y;
  std::vector<unsigned long long>* m_rdoID_prd;

  // HISTOGRAMS
  TH1* h_omegax;
  TH1* h_omegay;
  TH1* h_tot;
  TH1* h_totalTot;
  TH1* h_charge;
  TH1* h_totalCharge;
  TH1* h_eLoss;
  TH1* h_fake;
  TH1* h_ambig;
  TH1* h_split;
  TH1* h_splitProb1;
  TH1* h_splitProb2;
  TH1* h_splitInfoRaw;
  TH1* h_LVL1A;
  TH1* h_siCol;
  TH1* h_siRow;
  TH1* h_siPhiR;
  TH1* h_siZ;
  TH1* h_siPos_x;
  TH1* h_siPos_y;
  TH1* h_siPos_z;
  TH1* h_siGangPix;
  TH1* h_siDetID;
  TH1* h_siDetPix;
  TH1* h_siDetSCT;
  TH1* h_siDetBrl;
  TH1* h_siDetEc;
  TH1* h_siDetBlay;
  TH1* h_siDetInPixLay;
  TH1* h_siDetNtInPixLay;
  TH1* h_siDetDBM;
  TH1* h_siDetHitDepthDir;
  TH1* h_siDetHitPhiDir;
  TH1* h_siDetHitEtaDir;
  TH1* h_siDetMinR;
  TH1* h_siDetMaxR;
  TH1* h_siDetMinZ;
  TH1* h_siDetMaxZ;
  TH1* h_siDetMinPhi;
  TH1* h_siDetMaxPhi;
  TH1* h_siDetWidth;
  TH1* h_siDetMinWidth;
  TH1* h_siDetMaxWidth;
  TH1* h_siDetLength;
  TH1* h_siDetThick;
  TH1* h_siDetEtaPitch;
  TH1* h_siDetPhiPitch;
  TH1* h_clusID;
  TH1* h_locpos_x;
  TH1* h_locpos_y;
  TH1* h_rdoID_prd;
  

  
  TTree* m_tree;
  std::string m_ntupleFileName;
  std::string m_ntupleDirName;
  std::string m_ntupleTreeName;
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;
};

#endif // PIXEL_FAST_RDO_ANALYSIS_H
