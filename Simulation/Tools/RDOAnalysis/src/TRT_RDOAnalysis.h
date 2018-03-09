/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRT_RDO_ANALYSIS_H
#define TRT_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/TRT_SimHelper.h"
#include "InDetIdentifier/TRT_ID.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class TH1;
class TRT_ID;
class TRT_RDORawData;

class TRT_RDOAnalysis : public AthAlgorithm {

public:
  TRT_RDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~TRT_RDOAnalysis(){}

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

private:
  SG::ReadHandleKey<TRT_RDO_Container> m_inputKey;
  SG::ReadHandleKey<InDetSimDataCollection> m_inputTruthKey;
  const TRT_ID *m_trtID;

  // NTUPLE BRANCHES
  // RDO
  std::vector<unsigned long long>* m_rdoID;
  std::vector<unsigned int>* m_rdoWord;
  // TRT_ID
  std::vector<int>* m_barrelEndcap;
  std::vector<int>* m_phiModule;
  std::vector<int>* m_layerWheel;
  std::vector<int>* m_strawLayer;
  std::vector<int>* m_straw;
  // TRT_RDORawData
  std::vector<bool>* m_highLevel;
  std::vector<double>* m_timeOverThreshold;
  std::vector<int>* m_driftTimeBin;
  std::vector<int>* m_trailEdge;
  std::vector<bool>* m_firstBin;
  std::vector<bool>* m_lastBin;
  // SDO
  std::vector<unsigned long long>* m_sdoID;
  std::vector<int>* m_sdoWord;
  // TRT_ID
  std::vector<int>* m_barrelEndcap_sdo;
  std::vector<int>* m_phiModule_sdo;
  std::vector<int>* m_layerWheel_sdo;
  std::vector<int>* m_strawLayer_sdo;
  std::vector<int>* m_straw_sdo;
  // TRT_SimHelper
  std::vector<bool>* m_aboveThresh;
  std::vector<bool>* m_deadChan;
  std::vector<bool>* m_RODdata;
  std::vector<bool>* m_validStraw;
  std::vector<bool>* m_hit;
  // Deposit - particle link + energy (charge)
  std::vector<int>* m_barcode;
  std::vector<int>* m_eventIndex;
  std::vector<float>* m_charge;
  std::vector< std::vector<int> >* m_barcode_vec;
  std::vector< std::vector<int> >* m_eventIndex_vec;
  std::vector< std::vector<float> >* m_charge_vec;

  // HISTOGRAMS
  TH1* m_h_rdoID;
  TH1* m_h_rdoWord;
  TH1* m_h_barrelEndcap;
  TH1* m_h_phiModule;
  TH1* m_h_layerWheel;
  TH1* m_h_strawLayer;
  TH1* m_h_straw;
  TH1* m_h_ToT;
  TH1* m_h_ToT_HL;
  TH1* m_h_driftTimeBin;
  TH1* m_h_trailEdge;
  // barrel TRT
  TH1* m_h_brlPhiMod;
  TH1* m_h_brlLayer;
  TH1* m_h_brlStrawLayer;
  TH1* m_h_brlStraw;
  TH1* m_h_brlToT;
  TH1* m_h_brlToT_HL;
  TH1* m_h_brlDriftTimeBin;
  TH1* m_h_brlTrailEdge;
  // endcap TRT
  TH1* m_h_ecPhiMod;
  TH1* m_h_ecWheel;
  TH1* m_h_ecStrawLayer;
  TH1* m_h_ecStraw;
  TH1* m_h_ecToT;
  TH1* m_h_ecToT_HL;
  TH1* m_h_ecDriftTimeBin;
  TH1* m_h_ecTrailEdge;
  // SDO
  TH1* m_h_sdoID;
  TH1* m_h_sdoWord;
  TH1* m_h_barrelEndcap_sdo;
  TH1* m_h_phiModule_sdo;
  TH1* m_h_layerWheel_sdo;
  TH1* m_h_strawLayer_sdo;
  TH1* m_h_straw_sdo;
  TH1* m_h_barcode;
  TH1* m_h_eventIndex;
  TH1* m_h_charge;

  TTree* m_tree;
  std::string m_ntupleFileName;
  std::string m_ntupleDirName;
  std::string m_ntupleTreeName;
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;

};

#endif // TRT_RDO_ANALYSIS_H
