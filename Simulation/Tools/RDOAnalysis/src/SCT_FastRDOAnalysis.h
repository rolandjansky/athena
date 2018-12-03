/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SCT_FAST_RDO_ANALYSIS_H
#define SCT_FAST_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class TH1;

class SCT_FastRDOAnalysis : public AthAlgorithm {

public:
  SCT_FastRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_FastRDOAnalysis(){}

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

private:
  SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_inputKey;
  // SCT_Cluster
  std::vector<uint16_t>* m_hitsTimeBin3;

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
  TH1* m_h_hitsTimeBin3;
  TH1* m_h_siCol;
  TH1* m_h_siRow;
  TH1* m_h_siPhiR;
  TH1* m_h_siZ;
  TH1* m_h_siPos_x;
  TH1* m_h_siPos_y;
  TH1* m_h_siPos_z;
  TH1* m_h_siGangPix;
  TH1* m_h_siDetID;
  TH1* m_h_siDetPix;
  TH1* m_h_siDetSCT;
  TH1* m_h_siDetBrl;
  TH1* m_h_siDetEc;
  TH1* m_h_siDetBlay;
  TH1* m_h_siDetInPixLay;
  TH1* m_h_siDetNtInPixLay;
  TH1* m_h_siDetDBM;
  TH1* m_h_siDetHitDepthDir;
  TH1* m_h_siDetHitPhiDir;
  TH1* m_h_siDetHitEtaDir;
  TH1* m_h_siDetMinR;
  TH1* m_h_siDetMaxR;
  TH1* m_h_siDetMinZ;
  TH1* m_h_siDetMaxZ;
  TH1* m_h_siDetMinPhi;
  TH1* m_h_siDetMaxPhi;
  TH1* m_h_siDetWidth;
  TH1* m_h_siDetMinWidth;
  TH1* m_h_siDetMaxWidth;
  TH1* m_h_siDetLength;
  TH1* m_h_siDetThick;
  TH1* m_h_siDetEtaPitch;
  TH1* m_h_siDetPhiPitch;
  TH1* m_h_clusID;
  TH1* m_h_locpos_x;
  TH1* m_h_locpos_y;
  TH1* m_h_rdoID_prd;

  TTree* m_tree;
  std::string m_ntupleFileName;
  std::string m_ntupleDirName;
  std::string m_ntupleTreeName;
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;
};

#endif // SCT_FAST_RDO_ANALYSIS_H
