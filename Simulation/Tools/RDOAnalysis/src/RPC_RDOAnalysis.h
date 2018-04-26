/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RPC_RDO_ANALYSIS_H
#define RPC_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class TH1;

class RPC_RDOAnalysis : public AthAlgorithm {

public:
  RPC_RDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~RPC_RDOAnalysis(){}

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

private:
  SG::ReadHandleKey<RpcPadContainer> m_inputKey;
  SG::ReadHandleKey<MuonSimDataCollection> m_inputTruthKey;
  std::vector<unsigned long long>* m_rpcID;
  std::vector<unsigned short int>* m_status;
  std::vector<unsigned short int>* m_err;
  std::vector<unsigned short int>* m_onlineID;
  std::vector<unsigned short int>* m_lvl1ID;
  std::vector<unsigned short int>* m_bcID;
  std::vector<int>* m_sector;

  std::vector<unsigned long long>* m_coinRpcID;
  std::vector<unsigned short int>* m_coinOnID;
  std::vector<unsigned short int>* m_coinCrc;
  std::vector<unsigned short int>* m_coinFel1ID;
  std::vector<unsigned short int>* m_coinFebcID;

  std::vector<unsigned short int>* m_firedBcID;
  std::vector<unsigned short int>* m_firedTime;
  std::vector<unsigned short int>* m_firedIjk;
  std::vector<unsigned short int>* m_firedChan;
  std::vector<unsigned short int>* m_firedOvl;
  std::vector<unsigned short int>* m_firedThr;

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

  TH1* m_h_rpcID;
  TH1* m_h_status;
  TH1* m_h_err;
  TH1* m_h_onlineID;
  TH1* m_h_lvl1ID;
  TH1* m_h_bcID;
  TH1* m_h_sector;

  TH1* m_h_coinRpcID;
  TH1* m_h_coinOnID;
  TH1* m_h_coinCrc;
  TH1* m_h_coinFel1ID;
  TH1* m_h_coinFebcID;

  TH1* m_h_firedBcID;
  TH1* m_h_firedTime;
  TH1* m_h_firedIjk;
  TH1* m_h_firedChan;
  TH1* m_h_firedOvl;
  TH1* m_h_firedThr;

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

#endif // RPC_RDO_ANALYSIS_H
