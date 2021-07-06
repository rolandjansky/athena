/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCSDOVariables.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TTree.h"

StatusCode RpcSDOVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillRpcSDOVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

  ATH_CHECK( this->clearVariables() );

  ATH_MSG_DEBUG( "Retrieve MDT SDO container with name = " << m_ContainerName.c_str() );
  const MuonSimDataCollection* mdtSdoContainer = nullptr;
  ATH_CHECK( m_evtStore->retrieve(mdtSdoContainer, m_ContainerName.c_str()) );

  for (const auto& coll : *mdtSdoContainer ) {

    const Identifier id = coll.first;
    const MuonSimData mdt_sdo = coll.second;

    // Get information on the SDO
    std::string stName   = m_rpcIdHelper->stationNameString(m_rpcIdHelper->stationName(id));
    int stationEta       = m_rpcIdHelper->stationEta(id);
    int stationPhi       = m_rpcIdHelper->stationPhi(id);
    int doubletR         = m_rpcIdHelper->doubletR(id);
    int doubletZ         = m_rpcIdHelper->doubletZ(id);
    int doubletPhi       = m_rpcIdHelper->doubletPhi(id);
    int gas_gap          = m_rpcIdHelper->gasGap(id);
    int measuresPhi      = m_rpcIdHelper->measuresPhi(id);
    int strip            = m_rpcIdHelper->strip(id);
    int channel          = m_rpcIdHelper->channel(id);

    ATH_MSG_DEBUG(     "RPC SDO:  Station Name [" << stName << " ]"
                    << " Station Eta ["  << stationEta      << "]"
                    << " Station Phi ["  << stationPhi      << "]"
                    << " doubletR  ["    << doubletR        << "]"
                    << " doubletZ  ["    << doubletZ        << "]"
                    << " doubletPhi  ["  << doubletPhi      << "]"
                    << " GasGap ["       << gas_gap         << "]"
                    << " measuresPhi ["  << measuresPhi     << "]"
                    << " strip ["        << strip           << "]"
                    << " ChNr ["         << channel         << "]" );

    m_rpc_sdo_stationName.push_back(stName);
    m_rpc_sdo_stationEta.push_back(stationEta);
    m_rpc_sdo_stationPhi.push_back(stationPhi);
    m_rpc_sdo_doubletR.push_back(doubletR);
    m_rpc_sdo_doubletZ.push_back(doubletZ);
    m_rpc_sdo_doubletPhi.push_back(doubletPhi);
    m_rpc_sdo_gas_gap.push_back(gas_gap);
    m_rpc_sdo_measuresPhi.push_back(measuresPhi);
    m_rpc_sdo_strip.push_back(strip);
    m_rpc_sdo_channel.push_back(channel);

    ATH_MSG_DEBUG( "Get the truth deposits from the SDO." );
    std::vector<MuonSimData::Deposit> deposits;
    mdt_sdo.deposits(deposits);

    const Amg::Vector3D hit_gpos = mdt_sdo.globalPosition();
    m_rpc_sdo_globalPosX.push_back( hit_gpos.x() );
    m_rpc_sdo_globalPosY.push_back( hit_gpos.y() );
    m_rpc_sdo_globalPosZ.push_back( hit_gpos.z() );

    m_rpc_sdo_globaltime.push_back( mdt_sdo.getTime() );
    m_rpc_sdo_word.push_back( mdt_sdo.word() );    

    // use the information of the first deposit
    int barcode = deposits[0].first.barcode();
    double MuonMCdata_firstentry = deposits[0].second.firstEntry();
    double MuonMCdata_secondentry = deposits[0].second.secondEntry();

    ATH_MSG_DEBUG("RPC SDO barcode=" << barcode);
    ATH_MSG_DEBUG("RPC SDO localPosX=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                               << ", localPosY=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

    m_rpc_sdo_barcode.push_back( barcode );
    m_rpc_sdo_localPosX.push_back( MuonMCdata_firstentry );
    m_rpc_sdo_localPosY.push_back( MuonMCdata_secondentry );

    m_rpc_nsdo++;
  }
  ATH_MSG_DEBUG("Processed " << m_rpc_nsdo << " MDT SDOs");
  return StatusCode::SUCCESS;
}

StatusCode RpcSDOVariables::clearVariables()
{
  m_rpc_nsdo = 0;
  m_rpc_sdo_stationName.clear();
  m_rpc_sdo_stationEta.clear();
  m_rpc_sdo_stationPhi.clear();
  m_rpc_sdo_doubletR.clear();
  m_rpc_sdo_doubletZ.clear();
  m_rpc_sdo_doubletPhi.clear();
  m_rpc_sdo_gas_gap.clear();
  m_rpc_sdo_measuresPhi.clear();
  m_rpc_sdo_strip.clear();
  m_rpc_sdo_channel.clear();
  m_rpc_sdo_word.clear();
  m_rpc_sdo_barcode.clear();
  m_rpc_sdo_globalPosX.clear();
  m_rpc_sdo_globalPosY.clear();
  m_rpc_sdo_globalPosZ.clear();
  m_rpc_sdo_globaltime.clear();
  m_rpc_sdo_localPosX.clear();
  m_rpc_sdo_localPosY.clear();
  return StatusCode::SUCCESS;
}


StatusCode RpcSDOVariables::initializeVariables()
{
  if(m_tree) {
    m_tree->Branch("SDO_RPC",               &m_rpc_nsdo);
    m_tree->Branch("SDO_RPC_stationName",   &m_rpc_sdo_stationName);
    m_tree->Branch("SDO_RPC_stationEta",    &m_rpc_sdo_stationEta); 
    m_tree->Branch("SDO_RPC_stationPhi",    &m_rpc_sdo_stationPhi);
    m_tree->Branch("SDO_RPC_doubletR",      &m_rpc_sdo_doubletR);
    m_tree->Branch("SDO_RPC_doubletZ",      &m_rpc_sdo_doubletZ);
    m_tree->Branch("SDO_RPC_doubletPhi",    &m_rpc_sdo_doubletPhi);
    m_tree->Branch("SDO_RPC_gas_gap",       &m_rpc_sdo_gas_gap);
    m_tree->Branch("SDO_RPC_measuresPhi",   &m_rpc_sdo_measuresPhi);
    m_tree->Branch("SDO_RPC_strip",         &m_rpc_sdo_strip);
    m_tree->Branch("SDO_RPC_channel",       &m_rpc_sdo_channel);
    m_tree->Branch("SDO_RPC_word",          &m_rpc_sdo_word);
    m_tree->Branch("SDO_RPC_barcode",       &m_rpc_sdo_barcode);
    m_tree->Branch("SDO_RPC_globalPosX",    &m_rpc_sdo_globalPosX);
    m_tree->Branch("SDO_RPC_globalPosY",    &m_rpc_sdo_globalPosY);
    m_tree->Branch("SDO_RPC_globalPosZ",    &m_rpc_sdo_globalPosZ);
    m_tree->Branch("SDO_RPC_global_time",   &m_rpc_sdo_globaltime);
    m_tree->Branch("SDO_RPC_localPosX",     &m_rpc_sdo_localPosX);
    m_tree->Branch("SDO_RPC_localPosY",     &m_rpc_sdo_localPosY);
  }
  return StatusCode::SUCCESS;
}
