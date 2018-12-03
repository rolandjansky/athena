/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CSC_RDO_ANALYSIS_H
#define CSC_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "MuonRDO/CscRawDataContainer.h"
#include "MuonSimData/CscSimDataCollection.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class TH1;

class CSC_RDOAnalysis : public AthAlgorithm {

public:
  CSC_RDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~CSC_RDOAnalysis(){}

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

private:
  SG::ReadHandleKey<CscRawDataContainer> m_inputKey;
  SG::ReadHandleKey<CscSimDataCollection> m_inputTruthKey;
  // RDO
  // CscRawDataCollection
  std::vector<uint16_t>* m_collID;
  std::vector<uint16_t>* m_rodID;
  std::vector<uint16_t>* m_subID;
  std::vector<uint16_t>* m_collRpuID;
  std::vector<uint8_t>* m_dataType;
  std::vector<bool>* m_smplPhase;
  std::vector<bool>* m_trigType;
  std::vector<uint8_t>* m_firstBitSum;
  std::vector<uint32_t>* m_eventType;
  std::vector<uint8_t>* m_calAmp;
  std::vector<uint8_t>* m_calLayer;
  std::vector<uint32_t>* m_scaAdd;
  std::vector<uint16_t>* m_numSmpl;
  std::vector<uint16_t>* m_latency;
  std::vector<uint8_t>* m_rate;
  std::vector<bool>* m_sparse;
  std::vector<bool>* m_neutron;
  std::vector<bool>* m_calEn;
  std::vector<uint16_t>* m_spuCt;
  // CscRawData
  std::vector<uint16_t>* m_cscRpuID;
  std::vector<uint16_t>* m_cscID;
  std::vector<uint16_t>* m_cscTime;
  std::vector<uint16_t>* m_cscWidth;
  std::vector<bool>* m_cscTimeComp;
  std::vector< std::vector<uint16_t> >* m_cscSmpl;
  std::vector<uint32_t>* m_cscAdd;
  std::vector<uint32_t>* m_cscHashID;

  // SDO
  std::vector<unsigned long long>* m_sdoID;
  std::vector<int>* m_sdoWord;
  std::vector<int>* m_barcode;
  std::vector<int>* m_eventIndex;
  std::vector<float>* m_energy;
  std::vector<float>* m_ypos;
  std::vector<float>* m_zpos;
  std::vector<float>* m_charge;
  std::vector< std::vector<int> >* m_barcode_vec;
  std::vector< std::vector<int> >* m_eventIndex_vec;
  std::vector< std::vector<float> >* m_energy_vec;
  std::vector< std::vector<float> >* m_ypos_vec;
  std::vector< std::vector<float> >* m_zpos_vec;
  std::vector< std::vector<float> >* m_charge_vec;

  TH1* m_h_collID;
  TH1* m_h_rodID;
  TH1* m_h_subID;
  TH1* m_h_collRpuID;
  TH1* m_h_dataType;
  TH1* m_h_spuCt;
  TH1* m_h_cscRpuID;
  TH1* m_h_cscID;
  TH1* m_h_cscTime;
  TH1* m_h_cscWidth;
  TH1* m_h_cscSmpl;
  TH1* m_h_cscAdd;
  TH1* m_h_cscHashID;

  TH1* m_h_sdoID;
  TH1* m_h_sdoWord;
  TH1* m_h_barcode;
  TH1* m_h_eventIndex;
  TH1* m_h_energy;
  TH1* m_h_ypos;
  TH1* m_h_zpos;
  TH1* m_h_charge;

  TTree* m_tree;
  std::string m_ntupleFileName;
  std::string m_ntupleDirName;
  std::string m_ntupleTreeName;
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;

};

#endif // CSC_RDO_ANALYSIS_H
