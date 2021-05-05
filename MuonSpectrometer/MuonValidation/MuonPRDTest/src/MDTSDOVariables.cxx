/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTSDOVariables.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TTree.h"

StatusCode MdtSDOVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillMdtSDOVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

  ATH_CHECK( this->clearVariables() );

  ATH_MSG_DEBUG( "Retrieve MDT SDO container with name = " << m_ContainerName.c_str() );
  const MuonSimDataCollection* mdtSdoContainer = nullptr;
  ATH_CHECK( m_evtStore->retrieve(mdtSdoContainer, m_ContainerName.c_str()) );

  for (const auto& coll : *mdtSdoContainer ) {

    const Identifier id = coll.first;
    const MuonSimData mdt_sdo = coll.second;

    // Get information on the SDO
    std::string stName   = m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id));
    int stationEta       = m_mdtIdHelper->stationEta(id);
    int stationPhi       = m_mdtIdHelper->stationPhi(id);
    int multilayer       = m_mdtIdHelper->multilayer(id);
    int tubeLayer        = m_mdtIdHelper->tubeLayer(id);
    int tube             = m_mdtIdHelper->tube(id);
    int channel          = m_mdtIdHelper->channel(id);

    ATH_MSG_DEBUG(     "MDT SDO:  Station Name [" << stName << " ]"
                    << " Station Eta ["  << stationEta      << "]"
                    << " Station Phi ["  << stationPhi      << "]"
                    << " multilayer  ["  << multilayer      << "]"
                    << " tubeLayer ["    << tubeLayer       << "]"
                    << " tube ["         << tube            << "]" );

    m_mdt_sdo_stationName.push_back(stName);
    m_mdt_sdo_stationEta.push_back(stationEta);
    m_mdt_sdo_stationPhi.push_back(stationPhi);
    m_mdt_sdo_multilayer.push_back(multilayer);
    m_mdt_sdo_tubeLayer.push_back(tubeLayer);
    m_mdt_sdo_tube.push_back(tube);
    m_mdt_sdo_channel.push_back(channel);

    ATH_MSG_DEBUG( "Get the truth deposits from the SDO." );
    std::vector<MuonSimData::Deposit> deposits;
    mdt_sdo.deposits(deposits);

    const Amg::Vector3D hit_gpos = mdt_sdo.globalPosition();
    m_mdt_sdo_globalPosX.push_back( hit_gpos.x() );
    m_mdt_sdo_globalPosY.push_back( hit_gpos.y() );
    m_mdt_sdo_globalPosZ.push_back( hit_gpos.z() );

    m_mdt_sdo_globaltime.push_back( mdt_sdo.getTime() );
    m_mdt_sdo_word.push_back( mdt_sdo.word() );    

    // use the information of the first deposit
    int barcode = deposits[0].first.barcode();
    double MuonMCdata_firstentry = deposits[0].second.firstEntry();
    double MuonMCdata_secondentry = deposits[0].second.secondEntry();

    ATH_MSG_DEBUG("MDT SDO barcode=" << barcode);
    ATH_MSG_DEBUG("MDT SDO localPosX=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                               << ", localPosY=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

    m_mdt_sdo_barcode.push_back( barcode );
    m_mdt_sdo_localPosX.push_back( MuonMCdata_firstentry );
    m_mdt_sdo_localPosY.push_back( MuonMCdata_secondentry );

    m_mdt_nsdo++;
  }
  ATH_MSG_DEBUG("Processed " << m_mdt_nsdo << " MDT SDOs");
  return StatusCode::SUCCESS;
}

StatusCode MdtSDOVariables::clearVariables()
{
  m_mdt_nsdo = 0;
  m_mdt_sdo_stationName.clear();
  m_mdt_sdo_stationEta.clear();
  m_mdt_sdo_stationPhi.clear();
  m_mdt_sdo_multilayer.clear();
  m_mdt_sdo_tubeLayer.clear();
  m_mdt_sdo_tube.clear();
  m_mdt_sdo_channel.clear();
  m_mdt_sdo_word.clear();
  m_mdt_sdo_barcode.clear();
  m_mdt_sdo_globalPosX.clear();
  m_mdt_sdo_globalPosY.clear();
  m_mdt_sdo_globalPosZ.clear();
  m_mdt_sdo_globaltime.clear();
  m_mdt_sdo_localPosX.clear();
  m_mdt_sdo_localPosY.clear();
  return StatusCode::SUCCESS;
}


StatusCode MdtSDOVariables::initializeVariables()
{
  if(m_tree) {
    m_tree->Branch("SDO_MDT",               &m_mdt_nsdo);
    m_tree->Branch("SDO_MDT_stationName",   &m_mdt_sdo_stationName);
    m_tree->Branch("SDO_MDT_stationEta",    &m_mdt_sdo_stationEta); 
    m_tree->Branch("SDO_MDT_stationPhi",    &m_mdt_sdo_stationPhi);
    m_tree->Branch("SDO_MDT_multilayer",    &m_mdt_sdo_multilayer);
    m_tree->Branch("SDO_MDT_tubeLayer",     &m_mdt_sdo_tubeLayer);
    m_tree->Branch("SDO_MDT_tube",          &m_mdt_sdo_tube);
    m_tree->Branch("SDO_MDT_channel",       &m_mdt_sdo_channel);
    m_tree->Branch("SDO_MDT_word",          &m_mdt_sdo_word);
    m_tree->Branch("SDO_MDT_barcode",       &m_mdt_sdo_barcode);
    m_tree->Branch("SDO_MDT_globalPosX",    &m_mdt_sdo_globalPosX);
    m_tree->Branch("SDO_MDT_globalPosY",    &m_mdt_sdo_globalPosY);
    m_tree->Branch("SDO_MDT_globalPosZ",    &m_mdt_sdo_globalPosZ);
    m_tree->Branch("SDO_MDT_global_time",   &m_mdt_sdo_globaltime);
    m_tree->Branch("SDO_MDT_localPosX",     &m_mdt_sdo_localPosX);
    m_tree->Branch("SDO_MDT_localPosY",     &m_mdt_sdo_localPosY);
  }
  return StatusCode::SUCCESS;
}
