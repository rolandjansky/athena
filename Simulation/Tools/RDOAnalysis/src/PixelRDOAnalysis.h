/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PIXEL_RDO_ANALYSIS_H
#define PIXEL_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/PixelSimHelper.h"
#include "InDetIdentifier/PixelID.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class PixelID;
class PixelRDORawData;

class PixelRDOAnalysis : public AthAlgorithm {

 public:
  PixelRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelRDOAnalysis(){}

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
  SG::ReadHandleKey<PixelRDO_Container> m_inputKey;
  SG::ReadHandleKey<InDetSimDataCollection> m_inputTruthKey;
  const PixelID *m_pixelID;
  // RDO
  std::vector<unsigned long long>* m_rdoID;
  std::vector<unsigned int>* m_rdoWord;
  // PixelID
  std::vector<int>* m_barrelEndcap;
  std::vector<int>* m_layerDisk;
  std::vector<int>* m_phiModule;
  std::vector<int>* m_etaModule;
  std::vector<int>* m_phiIndex;
  std::vector<int>* m_etaIndex;
  // PixelRDORawData
  std::vector<int>* m_ToT;
  std::vector<int>* m_BCID;
  std::vector<int>* m_LVL1A;
  std::vector<int>* m_LVL1ID;

  // SDO
  std::vector<unsigned long long>* m_sdoID;
  std::vector<int>* m_sdoWord;
  // PixelID
  std::vector<int>* m_barrelEndcap_sdo;
  std::vector<int>* m_layerDisk_sdo;
  std::vector<int>* m_phiModule_sdo;
  std::vector<int>* m_etaModule_sdo;
  std::vector<int>* m_phiIndex_sdo;
  std::vector<int>* m_etaIndex_sdo;
  // PixelSimHelper
  std::vector<bool>* m_noise;
  std::vector<bool>* m_belowThresh;
  std::vector<bool>* m_disabled;
  std::vector<bool>* m_badTOT;
  // Deposit
  std::vector<int>* m_barcode;
  std::vector<int>* m_eventIndex;
  std::vector<int>* m_charge;
  std::vector< std::vector<int> >* m_barcode_vec;
  std::vector< std::vector<int> >* m_eventIndex_vec;
  std::vector< std::vector<float> >* m_charge_vec;

  // HISTOGRAMS
  TH1* h_rdoID;
  TH1* h_rdoWord;
  TH1* h_barrelEndcap;
  TH1* h_layerDisk;
  TH1* h_phiModule;
  TH1* h_etaModule;
  TH1* h_phiIndex;
  TH1* h_etaIndex;
  TH1* h_ToT;
  TH1* h_BCID;
  TH1* h_LVL1A;
  TH1* h_LVL1ID;
  // barrel pixel
  TH1* h_brlLayer;
  TH1* h_brlPhiMod;
  TH1* h_brlEtaMod;
  TH1* h_brlPhiIndex;
  TH1* h_brlEtaIndex;
  TH1* h_brlToT;
  TH1* h_brlBCID;
  TH1* h_brlLVL1A;
  TH1* h_brlLVL1ID;
  // endcap pixel
  TH1* h_ecDisk;
  TH1* h_ecPhiMod;
  TH1* h_ecEtaMod;
  TH1* h_ecPhiIndex;
  TH1* h_ecEtaIndex;
  TH1* h_ecToT;
  TH1* h_ecBCID;
  TH1* h_ecLVL1A;
  TH1* h_ecLVL1ID;

  TH1* h_sdoID;
  TH1* h_sdoWord;
  TH1* h_barrelEndcap_sdo;
  TH1* h_layerDisk_sdo;
  TH1* h_phiModule_sdo;
  TH1* h_etaModule_sdo;
  TH1* h_phiIndex_sdo;
  TH1* h_etaIndex_sdo;
  TH1* h_barcode;
  TH1* h_eventIndex;
  TH1* h_charge;

  TTree* m_tree;
  std::string m_ntupleFileName;
  std::string m_ntupleDirName;
  std::string m_ntupleTreeName;
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;
};

#endif // PIXEL_RDO_ANALYSIS_H

