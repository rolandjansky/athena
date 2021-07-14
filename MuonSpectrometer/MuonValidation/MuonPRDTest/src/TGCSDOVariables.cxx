/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCSDOVariables.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TTree.h"

StatusCode TgcSDOVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillTgcSDOVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

  ATH_CHECK( this->clearVariables() );

  ATH_MSG_DEBUG( "Retrieve TGC SDO container with name = " << m_ContainerName.c_str() );
  const MuonSimDataCollection* tgcSdoContainer = nullptr;
  ATH_CHECK( m_evtStore->retrieve(tgcSdoContainer, m_ContainerName.c_str()) );

  for (const auto& coll : *tgcSdoContainer ) {

    const Identifier id = coll.first;
    const MuonSimData tgc_sdo = coll.second;

    // Get information on the SDO
    std::string stName   = m_tgcIdHelper->stationNameString(m_tgcIdHelper->stationName(id));
    int stationEta       = m_tgcIdHelper->stationEta(id);
    int stationPhi       = m_tgcIdHelper->stationPhi(id);
    int gas_gap          = m_tgcIdHelper->gasGap(id);
    int channel          = m_tgcIdHelper->channel(id);
    int isStrip          = m_tgcIdHelper->isStrip(id);


    ATH_MSG_DEBUG(     "TGC SDO Offline id:  Station Name [" << stName << " ]"
                       << " Station Eta ["  << stationEta      << "]"
                       << " Station Phi ["  << stationPhi      << "]"
                       << " GasGap ["       << gas_gap         << "]"
                       << " ChNr ["         << channel         << "]"
                       << " isStrip ["      << isStrip         << "]");


    m_tgc_sdo_stationName.push_back(stName);
    m_tgc_sdo_stationEta.push_back(stationEta);
    m_tgc_sdo_stationPhi.push_back(stationPhi);
    m_tgc_sdo_gas_gap.push_back(gas_gap);
    m_tgc_sdo_channel.push_back(channel);
    m_tgc_sdo_isStrip.push_back(isStrip);

    ATH_MSG_DEBUG( "Get the truth deposits from the SDO." );
    std::vector<MuonSimData::Deposit> deposits;
    tgc_sdo.deposits(deposits);

    const Amg::Vector3D hit_gpos = tgc_sdo.globalPosition();
    m_tgc_sdo_globalPosX.push_back( hit_gpos.x() );
    m_tgc_sdo_globalPosY.push_back( hit_gpos.y() );
    m_tgc_sdo_globalPosZ.push_back( hit_gpos.z() );

    m_tgc_sdo_globaltime.push_back( tgc_sdo.getTime() );
    m_tgc_sdo_word.push_back( tgc_sdo.word() );    

    // use the information of the first deposit
    int barcode = deposits[0].first.barcode();
    double MuonMCdata_firstentry = deposits[0].second.firstEntry();
    double MuonMCdata_secondentry = deposits[0].second.secondEntry();

    ATH_MSG_DEBUG("TGC SDO barcode=" << barcode);
    ATH_MSG_DEBUG("TGC SDO localPosX=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                               << ", localPosY=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

    m_tgc_sdo_barcode.push_back( barcode );
    m_tgc_sdo_localPosX.push_back( MuonMCdata_firstentry );
    m_tgc_sdo_localPosY.push_back( MuonMCdata_secondentry );

    m_tgc_nsdo++;
  }
  ATH_MSG_DEBUG("Processed " << m_tgc_nsdo << " TGC SDOs");
  return StatusCode::SUCCESS;
}

StatusCode TgcSDOVariables::clearVariables()
{
  m_tgc_nsdo = 0;
  m_tgc_sdo_stationName.clear();
  m_tgc_sdo_stationEta.clear();
  m_tgc_sdo_stationPhi.clear();
  m_tgc_sdo_gas_gap.clear();
  m_tgc_sdo_channel.clear();
  m_tgc_sdo_isStrip.clear();
  m_tgc_sdo_word.clear();
  m_tgc_sdo_barcode.clear();
  m_tgc_sdo_globalPosX.clear();
  m_tgc_sdo_globalPosY.clear();
  m_tgc_sdo_globalPosZ.clear();
  m_tgc_sdo_globaltime.clear();
  m_tgc_sdo_localPosX.clear();
  m_tgc_sdo_localPosY.clear();
  return StatusCode::SUCCESS;
}


StatusCode TgcSDOVariables::initializeVariables()
{
  if(m_tree) {
    m_tree->Branch("SDO_TGC",               &m_tgc_nsdo);
    m_tree->Branch("SDO_TGC_stationName",   &m_tgc_sdo_stationName);
    m_tree->Branch("SDO_TGC_stationEta",    &m_tgc_sdo_stationEta); 
    m_tree->Branch("SDO_TGC_stationPhi",    &m_tgc_sdo_stationPhi);
    m_tree->Branch("SDO_TGC_gas_gap",       &m_tgc_sdo_gas_gap);
    m_tree->Branch("SDO_TGC_channel",       &m_tgc_sdo_channel);
    m_tree->Branch("SDO_TGC_isStrip",       &m_tgc_sdo_isStrip);
    m_tree->Branch("SDO_TGC_word",          &m_tgc_sdo_word);
    m_tree->Branch("SDO_TGC_barcode",       &m_tgc_sdo_barcode);
    m_tree->Branch("SDO_TGC_globalPosX",    &m_tgc_sdo_globalPosX);
    m_tree->Branch("SDO_TGC_globalPosY",    &m_tgc_sdo_globalPosY);
    m_tree->Branch("SDO_TGC_globalPosZ",    &m_tgc_sdo_globalPosZ);
    m_tree->Branch("SDO_TGC_global_time",   &m_tgc_sdo_globaltime);
    m_tree->Branch("SDO_TGC_localPosX",     &m_tgc_sdo_localPosX);
    m_tree->Branch("SDO_TGC_localPosY",     &m_tgc_sdo_localPosY);
  }
  return StatusCode::SUCCESS;
}
