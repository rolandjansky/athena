/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "RPC_RDOAnalysis.h"
#include "StoreGate/ReadHandle.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

RPC_RDOAnalysis::RPC_RDOAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_inputKey("RPCPAD")
  , m_inputTruthKey("RPC_SDO")
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

  , m_h_rpcID(0)
  , m_h_status(0)
  , m_h_err(0)
  , m_h_onlineID(0)
  , m_h_lvl1ID(0)
  , m_h_bcID(0)
  , m_h_sector(0)
  , m_h_coinRpcID(0)
  , m_h_coinOnID(0)
  , m_h_coinCrc(0)
  , m_h_coinFel1ID(0)
  , m_h_coinFebcID(0)
  , m_h_firedBcID(0)
  , m_h_firedTime(0)
  , m_h_firedIjk(0)
  , m_h_firedChan(0)
  , m_h_firedOvl(0)
  , m_h_firedThr(0)
  , m_h_sdoID(0)
  , m_h_sdoWord(0)
  , m_h_xPos(0)
  , m_h_yPos(0)
  , m_h_zPos(0)
  , m_h_barcode(0)
  , m_h_eventIndex(0)
  , m_h_radius(0)
  , m_h_localZ(0)

  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/RPC_RDOAnalysis/")
  , m_ntupleTreeName("RPC_RDOAna")
  , m_path("/RPC_RDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("InputKey", m_inputKey);
  declareProperty("InputTruthKey", m_inputTruthKey);
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode RPC_RDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing RPC_RDOAnalysis" );

  // This will check that the properties were initialized
  // properly by job configuration.
  ATH_CHECK( m_inputKey.initialize() );
  ATH_CHECK( m_inputTruthKey.initialize() );

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
  m_h_rpcID = new TH1F("h_rpcID", "RPC Pad ID", 100, 0, 1e19);
  m_h_rpcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rpcID->GetName(), m_h_rpcID));

  m_h_status = new TH1F("h_status", "RPC Pad Status", 100, 0, 10);
  m_h_status->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_status->GetName(), m_h_status));

  m_h_err = new TH1F("h_err", "RPC Pad error code", 100, 0, 10);
  m_h_err->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_err->GetName(), m_h_err));

  m_h_onlineID = new TH1F("h_onlineID", "RPC Pad online ID", 100, 0, 10);
  m_h_onlineID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_onlineID->GetName(), m_h_onlineID));

  m_h_lvl1ID = new TH1F("h_lvl1ID", "RPC Pad LVL1 ID", 100, 0, 10);
  m_h_lvl1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_lvl1ID->GetName(), m_h_lvl1ID));

  m_h_bcID = new TH1F("h_bcID", "RPC Pad BCID", 100, 0, 10);
  m_h_bcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_bcID->GetName(), m_h_bcID));

  m_h_sector = new TH1F("h_sector", "RPC Pad Sector", 100, 0, 75);
  m_h_sector->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_sector->GetName(), m_h_sector));

  m_h_coinRpcID = new TH1F("h_coinRpcID", "CM Pad ID", 100, 0, 1e19);
  m_h_coinRpcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_coinRpcID->GetName(), m_h_coinRpcID));

  m_h_coinOnID = new TH1F("h_coinOnID", "CM Online ID", 100, 0, 10);
  m_h_coinOnID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_coinOnID->GetName(), m_h_coinOnID));

  m_h_coinCrc = new TH1F("h_coinCrc", "CM CRC", 100, 0, 300);
  m_h_coinCrc->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_coinCrc->GetName(), m_h_coinCrc));

  m_h_coinFel1ID = new TH1F("h_coinFel1ID", "CM FEL1ID", 100, 0, 10);
  m_h_coinFel1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_coinFel1ID->GetName(), m_h_coinFel1ID));

  m_h_coinFebcID = new TH1F("h_coinFebcID", "CM FEBCID", 100, 0, 10);
  m_h_coinFebcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_coinFebcID->GetName(), m_h_coinFebcID));

  m_h_firedBcID = new TH1F("h_firedBcID", "Fired Channel BCID", 100, 0, 10);
  m_h_firedBcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_firedBcID->GetName(), m_h_firedBcID));

  m_h_firedTime = new TH1F("h_firedTime", "Fired Channel time", 100, 0, 10);
  m_h_firedTime->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_firedTime->GetName(), m_h_firedTime));

  m_h_firedIjk = new TH1F("h_firedIjk", "Fired Channel ijk", 100, 0, 10);
  m_h_firedIjk->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_firedIjk->GetName(), m_h_firedIjk));

  m_h_firedChan = new TH1F("h_firedChan", "Fired Channel number", 100, 0, 35);
  m_h_firedChan->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_firedChan->GetName(), m_h_firedChan));

  m_h_firedOvl = new TH1F("h_firedOvl", "Fired Channel overlap", 100, 0, 10);
  m_h_firedOvl->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_firedOvl->GetName(), m_h_firedOvl));

  m_h_firedThr = new TH1F("h_firedThr", "Fired Channel threshold", 100, 0, 5);
  m_h_firedThr->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_firedThr->GetName(), m_h_firedThr));

  m_h_sdoID = new TH1F("h_sdoID", "sdoID", 100, 0, 1e19);
  m_h_sdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_sdoID->GetName(), m_h_sdoID));

  m_h_sdoWord = new TH1F("h_sdoWord", "sdoWord", 100, 0, 10);
  m_h_sdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_sdoWord->GetName(), m_h_sdoWord));

  m_h_xPos = new TH1F("h_xPos", "Global x-position (SDO)", 100, -15000, 15000);
  m_h_xPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_xPos->GetName(), m_h_xPos));

  m_h_yPos = new TH1F("h_yPos", "Global y-position (SDO)", 100, -15000, 15000);
  m_h_yPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_yPos->GetName(), m_h_yPos));

  m_h_zPos = new TH1F("h_zPos", "Global z-position (SDO)", 100, -15000, 15000);
  m_h_zPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_zPos->GetName(), m_h_zPos));

  m_h_barcode = new TH1F("h_barcode", "Barcode (SDO)", 100, 0, 2.2e9);
  m_h_barcode->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_barcode->GetName(), m_h_barcode));

  m_h_eventIndex = new TH1F("h_eventIndex", "Event index (SDO)", 100, 0, 1000);
  m_h_eventIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_eventIndex->GetName(), m_h_eventIndex));

  m_h_radius = new TH1F("h_radius", "Radius (SDO)", 100, 0, 1);
  m_h_radius->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_radius->GetName(), m_h_radius));

  m_h_localZ = new TH1F("h_localZ", "Local z-position (SDO)", 100, -250, 250);
  m_h_localZ->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_localZ->GetName(), m_h_localZ));

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
  SG::ReadHandle<RpcPadContainer> p_RPCpadCont (m_inputKey);
  if(p_RPCpadCont.isValid()) {
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

      m_h_rpcID->Fill(rpcID_int);
      m_h_status->Fill(padStat);
      m_h_err->Fill(padErr);
      m_h_onlineID->Fill(padOnID);
      m_h_lvl1ID->Fill(padLVL1ID);
      m_h_bcID->Fill(padBCID);
      m_h_sector->Fill(padSec);

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

        m_h_coinRpcID->Fill(coinRpcID_int);
        m_h_coinOnID->Fill(coinOnID);
        m_h_coinCrc->Fill(coinCrc);
        m_h_coinFel1ID->Fill(coinFel1ID);
        m_h_coinFebcID->Fill(coinFebcID);

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

          m_h_firedBcID->Fill(firedBcID);
          m_h_firedTime->Fill(firedTime);
          m_h_firedIjk->Fill(firedIjk);
          m_h_firedChan->Fill(firedChan);
          m_h_firedOvl->Fill(firedOvl);
          m_h_firedThr->Fill(firedThr);

        }
      }
    }
  }

  // SimData
  SG::ReadHandle<MuonSimDataCollection> simDataMapRPC (m_inputTruthKey);
  if(simDataMapRPC.isValid()) {
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

      m_h_sdoID->Fill(sdoID_int);
      m_h_sdoWord->Fill(sdoWord);
      m_h_xPos->Fill(xPos);
      m_h_yPos->Fill(yPos);
      m_h_zPos->Fill(zPos);

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

        m_h_barcode->Fill(bar);
        m_h_eventIndex->Fill(eventIx);
        m_h_radius->Fill(radius);
        m_h_localZ->Fill(localZ);

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
