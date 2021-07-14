/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCSDOVariables.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TTree.h"

StatusCode CscSDOVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillCscSDOVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

  ATH_CHECK( this->clearVariables() );

  ATH_MSG_DEBUG( "Retrieve CSC SDO container with name = " << m_ContainerName.c_str() );
  const MuonSimDataCollection* cscSdoContainer = nullptr;
  ATH_CHECK( m_evtStore->retrieve(cscSdoContainer, m_ContainerName.c_str()) );

  for (const auto& coll : *cscSdoContainer ) {

    const Identifier id = coll.first;
    const MuonSimData csc_sdo = coll.second;

    // Get information on the SDO
    std::string stName   = m_CscIdHelper->stationNameString(m_CscIdHelper->stationName(id));
    int stationEta       = m_CscIdHelper->stationEta(id);
    int stationPhi       = m_CscIdHelper->stationPhi(id);
    int channel          = m_CscIdHelper->channel(id);
    int chamberLayer     = m_CscIdHelper->chamberLayer(id);
    int wireLayer        = m_CscIdHelper->wireLayer(id);
    int strip            = m_CscIdHelper->strip(id);



    ATH_MSG_DEBUG(     "CSC SDO Offline id:  Station Name [" << stName << " ]"
                    << " Station Eta ["  << stationEta      << "]"
                    << " Station Phi ["  << stationPhi      << "]"
                    << " channel ["      << channel         << "]"
                    << " chamberLayer [" << chamberLayer    << "]"
                    << " wireLayer ["    << wireLayer       << "]"
                    << " strip ["        << strip           << "]");


    m_csc_sdo_stationName.push_back(stName);
    m_csc_sdo_stationEta.push_back(stationEta);
    m_csc_sdo_stationPhi.push_back(stationPhi);
    m_csc_sdo_channel.push_back(channel);
    m_csc_sdo_chamberLayer.push_back(chamberLayer);
    m_csc_sdo_wireLayer.push_back(wireLayer);
    m_csc_sdo_Strip.push_back(strip);

    ATH_MSG_DEBUG( "Get the truth deposits from the SDO." );
    std::vector<MuonSimData::Deposit> deposits;
    csc_sdo.deposits(deposits);

    const Amg::Vector3D hit_gpos = csc_sdo.globalPosition();
    m_csc_sdo_globalPosX.push_back( hit_gpos.x() );
    m_csc_sdo_globalPosY.push_back( hit_gpos.y() );
    m_csc_sdo_globalPosZ.push_back( hit_gpos.z() );

    m_csc_sdo_globaltime.push_back( csc_sdo.getTime() );
    m_csc_sdo_word.push_back( csc_sdo.word() );    

    // use the information of the first deposit
    int barcode = deposits[0].first.barcode();
    double MuonMCdata_firstentry = deposits[0].second.firstEntry();
    double MuonMCdata_secondentry = deposits[0].second.secondEntry();

    ATH_MSG_DEBUG("CSC SDO barcode=" << barcode);
    ATH_MSG_DEBUG("CSC SDO localPosX=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                               << ", localPosY=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

    m_csc_sdo_barcode.push_back( barcode );
    m_csc_sdo_localPosX.push_back( MuonMCdata_firstentry );
    m_csc_sdo_localPosY.push_back( MuonMCdata_secondentry );

    m_csc_nsdo++;
  }
  ATH_MSG_DEBUG("Processed " << m_csc_nsdo << " CSC SDOs");
  return StatusCode::SUCCESS;
}

StatusCode CscSDOVariables::clearVariables()
{
  m_csc_nsdo = 0;
  m_csc_sdo_stationName.clear();
  m_csc_sdo_stationEta.clear();
  m_csc_sdo_stationPhi.clear();
  m_csc_sdo_channel.clear();
  m_csc_sdo_chamberLayer.clear();
  m_csc_sdo_wireLayer.clear();
  m_csc_sdo_Strip.clear();
  m_csc_sdo_word.clear();
  m_csc_sdo_barcode.clear();
  m_csc_sdo_globalPosX.clear();
  m_csc_sdo_globalPosY.clear();
  m_csc_sdo_globalPosZ.clear();
  m_csc_sdo_globaltime.clear();
  m_csc_sdo_localPosX.clear();
  m_csc_sdo_localPosY.clear();
  return StatusCode::SUCCESS;
}


StatusCode CscSDOVariables::initializeVariables()
{
  if(m_tree) {
    m_tree->Branch("SDO_CSC",               &m_csc_nsdo);
    m_tree->Branch("SDO_CSC_stationName",   &m_csc_sdo_stationName);
    m_tree->Branch("SDO_CSC_stationEta",    &m_csc_sdo_stationEta); 
    m_tree->Branch("SDO_CSC_stationPhi",    &m_csc_sdo_stationPhi);
    m_tree->Branch("SDO_CSC_channel",       &m_csc_sdo_channel);
    m_tree->Branch("SDO_CSC_chamberLayer",  &m_csc_sdo_chamberLayer);
    m_tree->Branch("SDO_CSC_wireLayer",     &m_csc_sdo_wireLayer);
    m_tree->Branch("SDO_CSC_strip",         &m_csc_sdo_Strip);
    m_tree->Branch("SDO_CSC_word",          &m_csc_sdo_word);
    m_tree->Branch("SDO_CSC_barcode",       &m_csc_sdo_barcode);
    m_tree->Branch("SDO_CSC_globalPosX",    &m_csc_sdo_globalPosX);
    m_tree->Branch("SDO_CSC_globalPosY",    &m_csc_sdo_globalPosY);
    m_tree->Branch("SDO_CSC_globalPosZ",    &m_csc_sdo_globalPosZ);
    m_tree->Branch("SDO_CSC_global_time",   &m_csc_sdo_globaltime);
    m_tree->Branch("SDO_CSC_localPosX",     &m_csc_sdo_localPosX);
    m_tree->Branch("SDO_CSC_localPosY",     &m_csc_sdo_localPosY);
  }
  return StatusCode::SUCCESS;
}
