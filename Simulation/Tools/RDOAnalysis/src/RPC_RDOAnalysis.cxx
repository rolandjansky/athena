/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "RPC_RDOAnalysis.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

RPC_RDOAnalysis::RPC_RDOAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_rpcID(0)
  , m_status(0)
  , m_err(0)
  , m_onlineID(0)
  , m_lvl1ID(0)
  , m_bcID(0)
  , m_sector(0)
  , m_coinRpcID(0)
  , m_coinOnID(0)
  , m_coinCrc(0)
  , m_coinFel1ID(0)
  , m_coinFebcID(0)
  , m_firedBcID(0)
  , m_firedTime(0)
  , m_firedIjk(0)
  , m_firedChan(0)
  , m_firedOvl(0)
  , m_firedThr(0)    
  , m_sdoID(0)
  , m_sdoWord(0)
  , m_xPos(0)
  , m_yPos(0)
  , m_zPos(0)
  , m_barcode(0)
  , m_eventIndex(0)
  , m_radius(0)
  , m_localZ(0)
  , m_barcode_vec(0)
  , m_eventIndex_vec(0)
  , m_radius_vec(0)
  , m_localZ_vec(0)

  , h_rpcID(0)
  , h_status(0)
  , h_err(0)
  , h_onlineID(0)
  , h_lvl1ID(0)
  , h_bcID(0)
  , h_sector(0)
  , h_coinRpcID(0)
  , h_coinOnID(0)
  , h_coinCrc(0)
  , h_coinFel1ID(0)
  , h_coinFebcID(0)
  , h_firedBcID(0)
  , h_firedTime(0)
  , h_firedIjk(0)
  , h_firedChan(0)
  , h_firedOvl(0)
  , h_firedThr(0)
  , h_sdoID(0)
  , h_sdoWord(0)
  , h_xPos(0)
  , h_yPos(0)
  , h_zPos(0)
  , h_barcode(0)
  , h_eventIndex(0)
  , h_radius(0)
  , h_localZ(0)
    
  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/RPC_RDOAnalysis/")
  , m_ntupleTreeName("RPC_RDOAna")
  , m_path("/RPC_RDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode RPC_RDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing RPC_RDOAnalysis" );

  ATH_CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(TString(m_ntupleTreeName), "RPC_RDOAna");
  std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
  if (m_tree) {
    m_tree->Branch("rpcID", &m_rpcID);
    m_tree->Branch("status", &m_status);
    m_tree->Branch("err", &m_err);
    m_tree->Branch("onlineID", &m_onlineID);
    m_tree->Branch("lvl1ID", &m_lvl1ID);
    m_tree->Branch("bcID", &m_bcID);
    m_tree->Branch("sector", &m_sector);
    m_tree->Branch("coinRpcID", &m_coinRpcID);
    m_tree->Branch("coinOnID", &m_coinOnID);
    m_tree->Branch("coinCrc", &m_coinCrc);
    m_tree->Branch("coinFel1ID", &m_coinFel1ID);
    m_tree->Branch("coinFebcID", &m_coinFebcID);
    m_tree->Branch("firedBcID", &m_firedBcID);
    m_tree->Branch("firedTime", &m_firedTime);
    m_tree->Branch("firedIjk", &m_firedIjk);
    m_tree->Branch("firedChan", &m_firedChan);
    m_tree->Branch("firedOvl", &m_firedOvl);
    m_tree->Branch("firedThr", &m_firedThr);
    m_tree->Branch("sdoID", &m_sdoID);
    m_tree->Branch("sdoWord", &m_sdoWord);
    m_tree->Branch("xPos", &m_xPos);
    m_tree->Branch("yPos", &m_yPos);
    m_tree->Branch("zPos", &m_zPos);
    m_tree->Branch("barcode", &m_barcode);
    m_tree->Branch("eventIndex", &m_eventIndex);
    m_tree->Branch("radius", &m_radius);
    m_tree->Branch("localZ", &m_localZ);
    m_tree->Branch("barcode_vec", &m_barcode_vec);
    m_tree->Branch("eventIndex_vec", &m_eventIndex_vec);
    m_tree->Branch("radius_vec", &m_radius_vec);
    m_tree->Branch("localZ_vec", &m_localZ_vec); 
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  // HISTOGRAMS
  h_rpcID = new TH1F("h_rpcID", "RPC Pad ID", 100, 0, 1e19);
  h_rpcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_rpcID->GetName(), h_rpcID));

  h_status = new TH1F("h_status", "RPC Pad Status", 100, 0, 10);
  h_status->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_status->GetName(), h_status));

  h_err = new TH1F("h_err", "RPC Pad error code", 100, 0, 10);
  h_err->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_err->GetName(), h_err));

  h_onlineID = new TH1F("h_onlineID", "RPC Pad online ID", 100, 0, 10);
  h_onlineID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_onlineID->GetName(), h_onlineID));

  h_lvl1ID = new TH1F("h_lvl1ID", "RPC Pad LVL1 ID", 100, 0, 10);
  h_lvl1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_lvl1ID->GetName(), h_lvl1ID));

  h_bcID = new TH1F("h_bcID", "RPC Pad BCID", 100, 0, 10);
  h_bcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_bcID->GetName(), h_bcID));

  h_sector = new TH1F("h_sector", "RPC Pad Sector", 100, 0, 75);
  h_sector->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_sector->GetName(), h_sector));

  h_coinRpcID = new TH1F("h_coinRpcID", "CM Pad ID", 100, 0, 1e19);
  h_coinRpcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_coinRpcID->GetName(), h_coinRpcID));

  h_coinOnID = new TH1F("h_coinOnID", "CM Online ID", 100, 0, 10);
  h_coinOnID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_coinOnID->GetName(), h_coinOnID));

  h_coinCrc = new TH1F("h_coinCrc", "CM CRC", 100, 0, 300);
  h_coinCrc->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_coinCrc->GetName(), h_coinCrc));

  h_coinFel1ID = new TH1F("h_coinFel1ID", "CM FEL1ID", 100, 0, 10);
  h_coinFel1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_coinFel1ID->GetName(), h_coinFel1ID));

  h_coinFebcID = new TH1F("h_coinFebcID", "CM FEBCID", 100, 0, 10);
  h_coinFebcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_coinFebcID->GetName(), h_coinFebcID));
  
  h_firedBcID = new TH1F("h_firedBcID", "Fired Channel BCID", 100, 0, 10);
  h_firedBcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_firedBcID->GetName(), h_firedBcID));

  h_firedTime = new TH1F("h_firedTime", "Fired Channel time", 100, 0, 10);
  h_firedTime->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_firedTime->GetName(), h_firedTime));

  h_firedIjk = new TH1F("h_firedIjk", "Fired Channel ijk", 100, 0, 10);
  h_firedIjk->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_firedIjk->GetName(), h_firedIjk));

  h_firedChan = new TH1F("h_firedChan", "Fired Channel number", 100, 0, 35);
  h_firedChan->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_firedChan->GetName(), h_firedChan));

  h_firedOvl = new TH1F("h_firedOvl", "Fired Channel overlap", 100, 0, 10);
  h_firedOvl->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_firedOvl->GetName(), h_firedOvl));

  h_firedThr = new TH1F("h_firedThr", "Fired Channel threshold", 100, 0, 5);
  h_firedThr->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_firedThr->GetName(), h_firedThr));

  h_sdoID = new TH1F("h_sdoID", "sdoID", 100, 0, 1e19);
  h_sdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_sdoID->GetName(), h_sdoID));

  h_sdoWord = new TH1F("h_sdoWord", "sdoWord", 100, 0, 10);
  h_sdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_sdoWord->GetName(), h_sdoWord));

  h_xPos = new TH1F("h_xPos", "Global x-position (SDO)", 100, -15000, 15000);
  h_xPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_xPos->GetName(), h_xPos));

  h_yPos = new TH1F("h_yPos", "Global y-position (SDO)", 100, -15000, 15000);
  h_yPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_yPos->GetName(), h_yPos));

  h_zPos = new TH1F("h_zPos", "Global z-position (SDO)", 100, -15000, 15000);
  h_zPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_zPos->GetName(), h_zPos));

  h_barcode = new TH1F("h_barcode", "Barcode (SDO)", 100, 0, 2.2e9);
  h_barcode->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_barcode->GetName(), h_barcode));

  h_eventIndex = new TH1F("h_eventIndex", "Event index (SDO)", 100, 0, 1000);
  h_eventIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_eventIndex->GetName(), h_eventIndex));

  h_radius = new TH1F("h_radius", "Radius (SDO)", 100, 0, 1);
  h_radius->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_radius->GetName(), h_radius));

  h_localZ = new TH1F("h_localZ", "Local z-position (SDO)", 100, -250, 250);
  h_localZ->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_localZ->GetName(), h_localZ));

  return StatusCode::SUCCESS;
}

StatusCode RPC_RDOAnalysis::execute() {
  ATH_MSG_DEBUG( "In RPC_RDOAnalysis::execute()" );

  m_rpcID->clear();
  m_status->clear();
  m_err->clear();
  m_onlineID->clear();
  m_lvl1ID->clear();
  m_bcID->clear();
  m_sector->clear();
  m_coinRpcID->clear();
  m_coinOnID->clear();
  m_coinCrc->clear();
  m_coinFel1ID->clear();
  m_coinFebcID->clear();
  m_firedBcID->clear();
  m_firedTime->clear();
  m_firedIjk->clear();
  m_firedChan->clear();
  m_firedOvl->clear();
  m_firedThr->clear();
  m_sdoID->clear();
  m_sdoWord->clear();
  m_xPos->clear();
  m_yPos->clear();
  m_zPos->clear();
  m_barcode->clear();
  m_eventIndex->clear();
  m_radius->clear();
  m_localZ->clear();
  m_barcode_vec->clear();
  m_eventIndex_vec->clear();
  m_radius_vec->clear();
  m_localZ_vec->clear();

  // RPC Pad
  const RpcPadContainer* p_RPCpadCont;
  if (evtStore()->retrieve(p_RPCpadCont, "RPCPAD") == StatusCode::SUCCESS) {
    // loop over RPC pad container
    RpcPadContainer::const_iterator rpc_itr(p_RPCpadCont->begin());
    const RpcPadContainer::const_iterator rpc_end(p_RPCpadCont->end());
    for ( ; rpc_itr != rpc_end; ++rpc_itr ) {
      const Identifier rpcID((*rpc_itr)->identify());
      const unsigned short int padStat((*rpc_itr)->status());
      const unsigned short int padErr((*rpc_itr)->errorCode());
      const unsigned short int padOnID((*rpc_itr)->onlineId());
      const unsigned short int padLVL1ID((*rpc_itr)->lvl1Id());
      const unsigned short int padBCID((*rpc_itr)->bcId());
      const int padSec((*rpc_itr)->sector());
      
      const unsigned long long rpcID_int = rpcID.get_compact();
      m_rpcID->push_back(rpcID_int);
      m_status->push_back(padStat);
      m_err->push_back(padErr);
      m_onlineID->push_back(padOnID);
      m_lvl1ID->push_back(padLVL1ID);
      m_bcID->push_back(padBCID);
      m_sector->push_back(padSec);

      h_rpcID->Fill(rpcID_int);
      h_status->Fill(padStat);
      h_err->Fill(padErr);
      h_onlineID->Fill(padOnID);
      h_lvl1ID->Fill(padLVL1ID);
      h_bcID->Fill(padBCID);
      h_sector->Fill(padSec);

      // RPC Coincidence Matrix
      const RpcPad* p_rpcCoin(*rpc_itr);
      RpcPad::const_iterator rpcCoin_itr(p_rpcCoin->begin());
      const RpcPad::const_iterator rpcCoin_end(p_rpcCoin->end()); 
      for ( ; rpcCoin_itr != rpcCoin_end; ++rpcCoin_itr ) {
	const Identifier coinRpcID((*rpcCoin_itr)->identify());
	const unsigned short int coinOnID((*rpcCoin_itr)->onlineId());
	const unsigned short int coinCrc((*rpcCoin_itr)->crc());
	const unsigned short int coinFel1ID((*rpcCoin_itr)->fel1Id());
	const unsigned short int coinFebcID((*rpcCoin_itr)->febcId());

	const unsigned long long coinRpcID_int = coinRpcID.get_compact();
	m_coinRpcID->push_back(coinRpcID_int);
	m_coinOnID->push_back(coinOnID);
	m_coinCrc->push_back(coinCrc);
	m_coinFel1ID->push_back(coinFel1ID);
	m_coinFebcID->push_back(coinFebcID);

	h_coinRpcID->Fill(coinRpcID_int);
	h_coinOnID->Fill(coinOnID);
	h_coinCrc->Fill(coinCrc);
	h_coinFel1ID->Fill(coinFel1ID);
	h_coinFebcID->Fill(coinFebcID);

	// RPC Fired Channel
	const RpcCoinMatrix *p_rpcFired(*rpcCoin_itr);
	RpcCoinMatrix::const_iterator rpcFired_itr(p_rpcFired->begin());
	const RpcCoinMatrix::const_iterator rpcFired_end(p_rpcFired->end());
	for ( ; rpcFired_itr != rpcFired_end; ++rpcFired_itr ) {
	  const unsigned short int firedBcID((*rpcFired_itr)->bcid());
	  const unsigned short int firedTime((*rpcFired_itr)->time());
	  const unsigned short int firedIjk((*rpcFired_itr)->ijk());
	  const unsigned short int firedChan((*rpcFired_itr)->channel());
	  const unsigned short int firedOvl((*rpcFired_itr)->ovl());
	  const unsigned short int firedThr((*rpcFired_itr)->thr());

	  m_firedBcID->push_back(firedBcID);
	  m_firedTime->push_back(firedTime);
	  m_firedIjk->push_back(firedIjk);
	  m_firedChan->push_back(firedChan);
	  m_firedOvl->push_back(firedOvl);
	  m_firedThr->push_back(firedThr);

	  h_firedBcID->Fill(firedBcID);
	  h_firedTime->Fill(firedTime);
	  h_firedIjk->Fill(firedIjk);
	  h_firedChan->Fill(firedChan);
	  h_firedOvl->Fill(firedOvl);
	  h_firedThr->Fill(firedThr);

	}
      }
    }
  }

  // SimData
  const MuonSimDataCollection* simDataMapRPC(nullptr);
  if (evtStore()->retrieve(simDataMapRPC, "RPC_SDO") == StatusCode::SUCCESS) {
    MuonSimDataCollection::const_iterator sdo_itr(simDataMapRPC->begin());
    const MuonSimDataCollection::const_iterator sdo_end(simDataMapRPC->end());

    std::vector<int> barcode_vec;
    std::vector<int> eventIndex_vec;
    std::vector<double> radius_vec;
    std::vector<float> localZ_vec;
    for ( ; sdo_itr != sdo_end; ++sdo_itr ) {
      const Identifier sdoID((*sdo_itr).first);
      const MuonSimData& sdo((*sdo_itr).second);

      const unsigned long long sdoID_int = sdoID.get_compact();
      const int sdoWord(sdo.word());
      const Amg::Vector3D gPos(sdo.globalPosition());
      const float xPos(gPos.x());
      const float yPos(gPos.y());
      const float zPos(gPos.z());
      
      m_sdoID->push_back(sdoID_int);
      m_sdoWord->push_back(sdoWord);
      m_xPos->push_back(xPos);
      m_yPos->push_back(yPos);
      m_zPos->push_back(zPos);

      h_sdoID->Fill(sdoID_int);
      h_sdoWord->Fill(sdoWord);
      h_xPos->Fill(xPos);
      h_yPos->Fill(yPos);
      h_zPos->Fill(zPos);

      // loop over deposits
      const std::vector<MuonSimData::Deposit>& deposits = sdo.getdeposits();
      std::vector<MuonSimData::Deposit>::const_iterator dep_itr(deposits.begin());
      const std::vector<MuonSimData::Deposit>::const_iterator dep_end(deposits.end());
      for ( ; dep_itr != dep_end; ++dep_itr ) {
	const HepMcParticleLink& particleLink = (*dep_itr).first;
	const int bar(particleLink.barcode());
	const int eventIx(particleLink.eventIndex());
	const MuonMCData& data = (*dep_itr).second;
	const double radius(data.firstEntry());
	const float localZ(data.secondEntry());
	
	m_barcode->push_back(bar);
	m_eventIndex->push_back(eventIx);
	m_radius->push_back(radius);
	m_localZ->push_back(localZ);

	h_barcode->Fill(bar);
	h_eventIndex->Fill(eventIx);
	h_radius->Fill(radius);
	h_localZ->Fill(localZ);
	
	barcode_vec.push_back(bar);
	eventIndex_vec.push_back(eventIx);
	radius_vec.push_back(radius);
	localZ_vec.push_back(localZ);
      }
      m_barcode_vec->push_back(barcode_vec);
      m_eventIndex_vec->push_back(eventIndex_vec);
      m_radius_vec->push_back(radius_vec);
      m_localZ_vec->push_back(localZ_vec);
      barcode_vec.clear();
      eventIndex_vec.clear();
      radius_vec.clear();
      localZ_vec.clear();
    }
  }

  if (m_tree) {
    m_tree->Fill();
  }

  return StatusCode::SUCCESS;
}

StatusCode RPC_RDOAnalysis::finalize() {
  return StatusCode::SUCCESS;
}
