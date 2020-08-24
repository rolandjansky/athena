/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RDO_ANALYSIS_H
#define SCT_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/SCT_SimHelper.h"
#include "StoreGate/ReadHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TH1.h"
#include "TH2.h"

#include <string>
#include <vector>

class TTree;
class SCT_ID;

class SCT_RDOAnalysis : public AthAlgorithm {

 public:
  SCT_RDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_RDOAnalysis() = default;

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

 private:
  SG::ReadHandleKey<SCT_RDO_Container> m_inputKey{this, "InputKey", "SCT_RDOs"};
  SG::ReadHandleKey<InDetSimDataCollection> m_inputTruthKey{this, "InputTruthKey", "SCT_SDO_Map"};
  const SCT_ID* m_sctID;
  // RDO
  std::vector<unsigned long long>* m_rdoID;
  std::vector<unsigned int>* m_rdoWord;
  // SCT_ID
  std::vector<int>* m_barrelEndcap;
  std::vector<int>* m_layerDisk;
  std::vector<int>* m_phiModule;
  std::vector<int>* m_etaModule;
  std::vector<int>* m_side;
  std::vector<int>* m_strip;
  // SCT_RDORawData
  std::vector<int>* m_groupSize;
  // SDO
  std::vector<unsigned long long>* m_sdoID;
  std::vector<int>* m_sdoWord;
  // SCT_ID
  std::vector<int>* m_barrelEndcap_sdo;
  std::vector<int>* m_layerDisk_sdo;
  std::vector<int>* m_phiModule_sdo;
  std::vector<int>* m_etaModule_sdo;
  std::vector<int>* m_side_sdo;
  std::vector<int>* m_strip_sdo;
  // SCT_SimHelper
  std::vector<bool>* m_noise;
  std::vector<bool>* m_belowThresh;
  std::vector<bool>* m_disabled;
  // Deposit - particle link + energy (charge)
  std::vector<int>* m_barcode;
  std::vector<int>* m_eventIndex;
  std::vector<float>* m_charge;
  std::vector<std::vector<int>>* m_barcode_vec;
  std::vector<std::vector<int>>* m_eventIndex_vec;
  std::vector<std::vector<float>>* m_charge_vec;

  // HISTOGRAMS
  TH1* m_h_rdoID;
  TH1* m_h_rdoWord;
  TH1* m_h_barrelEndcap;
  TH1* m_h_layerDisk;
  TH1* m_h_phiModule;
  TH1* m_h_etaModule;
  TH1* m_h_side;
  TH1* m_h_strip;
  TH1* m_h_groupSize;
  TH2* m_h_phi_v_eta;
  // barrel SCT
  TH1* m_h_brlLayer;
  TH1* m_h_brlPhiMod;
  TH1* m_h_brlEtaMod;
  TH1* m_h_brlSide;
  TH1* m_h_brlStrip;
  TH1* m_h_brlGroupSize;
  TH2* m_h_brl_phi_v_eta;
  // endcap SCT
  TH1* m_h_ecDisk;
  TH1* m_h_ecPhiMod;
  TH1* m_h_ecEtaMod;
  TH1* m_h_ecSide;
  TH1* m_h_ecStrip;
  TH1* m_h_ecGroupSize;
  TH2* m_h_ec_phi_v_eta;

  TH1* m_h_sdoID;
  TH1* m_h_sdoWord;
  TH1* m_h_barrelEndcap_sdo;
  TH1* m_h_layerDisk_sdo;
  TH1* m_h_phiModule_sdo;
  TH1* m_h_etaModule_sdo;
  TH1* m_h_side_sdo;
  TH1* m_h_strip_sdo;
  TH1* m_h_barcode;
  TH1* m_h_eventIndex;
  TH1* m_h_charge;
  TH2* m_h_phi_v_eta_sdo;
  TH1* m_h_TruthMatchedRDOs;


  TTree* m_tree;
  StringProperty m_ntupleFileName{this, "NtupleFileName", "/ntuples/file1"};
  StringProperty m_ntupleDirName{this, "NtupleDirectoryName", "/SCT_RDOAnalysis/"};
  StringProperty m_ntupleTreeName{this, "NtupleTreeName", "SCT_RDOAna"};
  StringProperty m_path{this, "HistPath", "/SCT_RDOAnalysis/"};
  ServiceHandle<ITHistSvc> m_thistSvc;

};

#endif // SCT_RDO_ANALYSIS_H
