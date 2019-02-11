/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TGC_RDO_ANALYSIS_H
#define TGC_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "MuonRDO/TgcRdoContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class TH1;

class TGC_RDOAnalysis : public AthAlgorithm {

public:
  TGC_RDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TGC_RDOAnalysis(){}

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

private:
  SG::ReadHandleKey<TgcRdoContainer> m_inputKey;
  SG::ReadHandleKey<MuonSimDataCollection> m_inputTruthKey;
  // TgcRdo
  std::vector<uint16_t>* m_tgcID;
  std::vector<uint16_t>* m_tgcSubDetID;
  std::vector<uint16_t>* m_tgcRodID;
  std::vector<uint16_t>* m_tgcTrigType;
  std::vector<uint16_t>* m_tgcBcID;
  std::vector<uint16_t>* m_tgcL1ID;
  //Errors& errors
  //RodStatus& rodStatus
  //LocalStatus& localStatus
  std::vector<uint32_t>* m_tgcOrbit;
  std::vector<uint16_t>* m_tgcVersion;
  //calculateOnlineId?

  // TgcRawData
  // bc tag (central, next, previous)
  std::vector<uint16_t>* m_bcTag;
  // online identifiers
  std::vector<uint16_t>* m_subDetID;
  std::vector<uint16_t>* m_rodID;
  std::vector<uint16_t>* m_sswID;
  std::vector<uint16_t>* m_slbID;
  // bcid + l1id for slb
  std::vector<uint16_t>* m_bcID;
  std::vector<uint16_t>* m_l1ID;
  // hit/coincidence type + slb type
  std::vector<int>* m_type;
  std::vector<int>* m_slbType;
  std::vector<bool>* m_coinc;
  // HIT
  // bitmap position
  std::vector<uint16_t>* m_bitPos;
  // index of associated coincidence
  std::vector<uint16_t>* m_track;
  std::vector<bool>* m_adj;
  // COINCIDENCE
  // coincidence type
  std::vector<bool>* m_forward;
  // coincidence index
  std::vector<uint16_t>* m_index;
  // coincidence data
  std::vector<uint16_t>* m_pos;
  std::vector<uint16_t>* m_delta;
  std::vector<uint16_t>* m_segment;
  std::vector<uint16_t>* m_subMat;
  // hipT data
  std::vector<uint16_t>* m_sector;
  std::vector<uint16_t>* m_chip;
  std::vector<bool>* m_hipt;
  std::vector<uint16_t>* m_hitID;
  std::vector<uint16_t>* m_hsub;
  std::vector<bool>* m_strip;
  std::vector<uint16_t>* m_inner;
  // sector logic
  std::vector<bool>* m_cand3plus;
  std::vector<bool>* m_muplus;
  std::vector<bool>* m_thresh;
  std::vector<bool>* m_overlap;
  std::vector<bool>* m_veto;
  std::vector<uint16_t>* m_roi;


  // SDO
  std::vector<unsigned long long>* m_sdoID;
  std::vector<int>* m_sdoWord;
  std::vector<float>* m_xPos;
  std::vector<float>* m_yPos;
  std::vector<float>* m_zPos;
  // Deposit
  std::vector<int>* m_barcode;
  std::vector<int>* m_eventIndex;
  std::vector<double>* m_radius; // firstEntry of MuonMCData (?)
  std::vector<float>* m_localZ; // secondEntry of MuonMCData (?)
  std::vector< std::vector<int> >* m_barcode_vec;
  std::vector< std::vector<int> >* m_eventIndex_vec;
  std::vector< std::vector<double> >* m_radius_vec;
  std::vector< std::vector<float> >* m_localZ_vec;

  // HISTOGRAMS
  TH1* m_h_tgcID;
  TH1* m_h_tgcSubDetID;
  TH1* m_h_tgcRodID;
  TH1* m_h_tgcTrigType;
  TH1* m_h_tgcBcID;
  TH1* m_h_tgcL1ID;
  TH1* m_h_bcTag;
  TH1* m_h_subDetID;
  TH1* m_h_rodID;
  TH1* m_h_sswID;
  TH1* m_h_slbID;
  TH1* m_h_bcID;
  TH1* m_h_l1ID;
  TH1* m_h_type;
  TH1* m_h_slbType;
  TH1* m_h_bitPos;
  TH1* m_h_track;
  TH1* m_h_adj;

  TH1* m_h_sdoID;
  TH1* m_h_sdoWord;
  TH1* m_h_xPos;
  TH1* m_h_yPos;
  TH1* m_h_zPos;
  TH1* m_h_barcode;
  TH1* m_h_eventIndex;
  TH1* m_h_radius;
  TH1* m_h_localZ;

  TTree* m_tree;
  std::string m_ntupleFileName;
  std::string m_ntupleDirName;
  std::string m_ntupleTreeName;
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;

};

#endif // TGC_RDO_ANALYSIS_H
