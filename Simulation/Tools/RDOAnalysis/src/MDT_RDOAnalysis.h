/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MDT_RDO_ANALYSIS_H
#define MDT_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "MuonRDO/MdtCsmContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class TH1;

class MDT_RDOAnalysis : public AthAlgorithm {

public:
  MDT_RDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~MDT_RDOAnalysis(){}

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

private:
  SG::ReadHandleKey<MdtCsmContainer> m_inputKey;
  SG::ReadHandleKey<MuonSimDataCollection> m_inputTruthKey;
  // RDO
  // MdtCsm - AmtHits of single Mdt chamber
  std::vector<unsigned long long>* m_mdtID; // csm offline id
  std::vector<unsigned int>* m_mdtIDhash;
  std::vector<uint16_t>* m_subID; // sub-detector id
  std::vector<uint16_t>* m_mrodID; // mrod id from csm header
  std::vector<uint16_t>* m_csmID; // csm online id
  // MdtAmtHit - single channel of AMT (Atlas Muon TDC)
  std::vector<uint16_t>* m_tdcID;
  std::vector<uint16_t>* m_chanID;
  std::vector<uint16_t>* m_coarse;
  std::vector<uint16_t>* m_fine;
  std::vector<uint16_t>* m_width;
  std::vector<bool>* m_leading;
  std::vector<bool>* m_isMasked;
  std::vector<uint32_t>* m_dataWords;

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
  TH1* m_h_mdtID;
  TH1* m_h_subID;
  TH1* m_h_mrodID;
  TH1* m_h_csmID;
  TH1* m_h_tdcID;
  TH1* m_h_chanID;
  TH1* m_h_coarse;
  TH1* m_h_fine;
  TH1* m_h_width;

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

#endif // MDT_RDO_ANALYSIS_H
