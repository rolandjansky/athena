/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCSDOVariables.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "TTree.h"
#include <TString.h> // for Form

StatusCode sTGCSDOVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillNSWsTGCSDOVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

  ATH_CHECK( this->clearVariables() );

  ATH_MSG_DEBUG( "Retrieve sTGC SDO container with name = " << m_ContainerName.c_str() );
  const MuonSimDataCollection* nsw_sTgcSdoContainer = nullptr;
  ATH_CHECK( m_evtStore->retrieve(nsw_sTgcSdoContainer, m_ContainerName.c_str()) );

  for ( auto coll : *nsw_sTgcSdoContainer ) {

    Identifier Id = coll.first;
    const MuonSimData mm_sdo = coll.second;

    // Get information on the SDO
    std::string stName   = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(Id));
    int stationEta       = m_sTgcIdHelper->stationEta(Id);
    int stationPhi       = m_sTgcIdHelper->stationPhi(Id);
    int multiplet        = m_sTgcIdHelper->multilayer(Id);
    int gas_gap          = m_sTgcIdHelper->gasGap(Id);
    int channel          = m_sTgcIdHelper->channel(Id);
    int channel_type     = m_sTgcIdHelper->channelType(Id);


    ATH_MSG_DEBUG(     "sTGC SDO:  Station Name [" << stName << " ]"
                    << " Station Eta ["  << stationEta      << "]"
                    << " Station Phi ["  << stationPhi      << "]"
                    << " Multiplet  ["   << multiplet       << "]"
                    << " GasGap ["       << gas_gap         << "]"
                    << " ChNr ["         << channel         << "]"
                    << " ChType ["       << channel_type    << "]" );

    m_NSWsTGC_sdo_stationName->push_back(stName);
    m_NSWsTGC_sdo_stationEta->push_back(stationEta);
    m_NSWsTGC_sdo_stationPhi->push_back(stationPhi);
    m_NSWsTGC_sdo_multiplet->push_back(multiplet);
    m_NSWsTGC_sdo_gas_gap->push_back(gas_gap);
    m_NSWsTGC_sdo_channel->push_back(channel);
    m_NSWsTGC_sdo_channel_type->push_back(channel_type);

    ATH_MSG_DEBUG( "Get the truth deposits from the SDO." );
    std::vector<MuonSimData::Deposit> deposits;
    mm_sdo.deposits(deposits);

    const Amg::Vector3D hit_gpos = mm_sdo.globalPosition();
    m_NSWsTGC_sdo_globalPosX->push_back( hit_gpos.x() );
    m_NSWsTGC_sdo_globalPosY->push_back( hit_gpos.y() );
    m_NSWsTGC_sdo_globalPosZ->push_back( hit_gpos.z() );

    m_NSWsTGC_sdo_globaltime->push_back( mm_sdo.getTime() );
    m_NSWsTGC_sdo_word->push_back( mm_sdo.word() );

    // use the information of the first deposit
    int barcode = deposits[0].first.barcode();
    double MuonMCdata_firstentry = deposits[0].second.firstEntry();
    double MuonMCdata_secondentry = deposits[0].second.secondEntry();

    ATH_MSG_DEBUG("sTGC SDO barcode=" << barcode);
    ATH_MSG_DEBUG("sTGC SDO energy=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                               << ", tof=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

    m_NSWsTGC_sdo_barcode->push_back( barcode );
    m_NSWsTGC_sdo_E->push_back( MuonMCdata_firstentry );
    m_NSWsTGC_sdo_tof->push_back( MuonMCdata_secondentry );

    // Retrive the detector element and local SDO coordinates
    bool isSmall = stName[2] == 'S';
    const MuonGM::sTgcReadoutElement* rdoEl = MuonDetMgr->getsTgcRElement_fromIdFields(isSmall, stationEta, stationPhi, multiplet);
    if (!rdoEl) throw std::runtime_error(Form("File: %s, Line: %d\nsTGCSDOVariables::fillVariables() - Failed to retrieve sTgcReadoutElement for isSmall=%d, stationEta=%d, stationPhi=%d, multiplet=%d", __FILE__, __LINE__, isSmall, stationEta, stationPhi, multiplet));

    Amg::Vector2D loc_pos(0., 0.);
    rdoEl->surface(Id).globalToLocal(hit_gpos, Amg::Vector3D(0., 0., 0.), loc_pos);

    ATH_MSG_DEBUG("sTGC SDO local position X=" << std::setw(9) << std::setprecision(2) << loc_pos[0]
                               << ", local position Y=" << std::setw(9) << std::setprecision(2) << loc_pos[1]);

    m_NSWsTGC_sdo_localPosX->push_back( loc_pos[0] );
    m_NSWsTGC_sdo_localPosY->push_back( loc_pos[1] );

    m_NSWsTGC_nsdo++;
  }

  ATH_MSG_DEBUG("Processed " << m_NSWsTGC_nsdo << " sTGC SDOs");
  return StatusCode::SUCCESS;
}


void sTGCSDOVariables::deleteVariables()
{
  delete m_NSWsTGC_sdo_stationName;
  delete m_NSWsTGC_sdo_stationEta;
  delete m_NSWsTGC_sdo_stationPhi;
  delete m_NSWsTGC_sdo_multiplet;
  delete m_NSWsTGC_sdo_gas_gap;
  delete m_NSWsTGC_sdo_channel;
  delete m_NSWsTGC_sdo_channel_type;

  delete m_NSWsTGC_sdo_word;
  delete m_NSWsTGC_sdo_barcode;
  delete m_NSWsTGC_sdo_globalPosX;
  delete m_NSWsTGC_sdo_globalPosY;
  delete m_NSWsTGC_sdo_globalPosZ;
  delete m_NSWsTGC_sdo_globaltime;

  delete m_NSWsTGC_sdo_E;
  delete m_NSWsTGC_sdo_tof;

  delete m_NSWsTGC_sdo_localPosX;
  delete m_NSWsTGC_sdo_localPosY;

  m_NSWsTGC_nsdo    = 0;
  m_NSWsTGC_sdo_stationName  = nullptr;
  m_NSWsTGC_sdo_stationEta   = nullptr;
  m_NSWsTGC_sdo_stationPhi   = nullptr;
  m_NSWsTGC_sdo_multiplet    = nullptr;
  m_NSWsTGC_sdo_gas_gap      = nullptr;
  m_NSWsTGC_sdo_channel      = nullptr;
  m_NSWsTGC_sdo_channel_type = nullptr;

  m_NSWsTGC_sdo_word       = nullptr;
  m_NSWsTGC_sdo_barcode    = nullptr;
  m_NSWsTGC_sdo_globalPosX = nullptr;
  m_NSWsTGC_sdo_globalPosY = nullptr;
  m_NSWsTGC_sdo_globalPosZ = nullptr;
  m_NSWsTGC_sdo_globaltime = nullptr;

  m_NSWsTGC_sdo_E    = nullptr;
  m_NSWsTGC_sdo_tof  = nullptr;

  m_NSWsTGC_sdo_localPosX = nullptr;
  m_NSWsTGC_sdo_localPosY = nullptr;

  return;
}


StatusCode sTGCSDOVariables::clearVariables()
{
  m_NSWsTGC_nsdo = 0;
  m_NSWsTGC_sdo_stationName->clear();
  m_NSWsTGC_sdo_stationEta->clear();
  m_NSWsTGC_sdo_stationPhi->clear();
  m_NSWsTGC_sdo_multiplet->clear();
  m_NSWsTGC_sdo_gas_gap->clear();
  m_NSWsTGC_sdo_channel->clear();
  m_NSWsTGC_sdo_channel_type->clear();

  m_NSWsTGC_sdo_word->clear();
  m_NSWsTGC_sdo_barcode->clear();
  m_NSWsTGC_sdo_globalPosX->clear();
  m_NSWsTGC_sdo_globalPosY->clear();
  m_NSWsTGC_sdo_globalPosZ->clear();
  m_NSWsTGC_sdo_globaltime->clear();
  m_NSWsTGC_sdo_E->clear();
  m_NSWsTGC_sdo_tof->clear();
  m_NSWsTGC_sdo_localPosX->clear();
  m_NSWsTGC_sdo_localPosY->clear();

  return StatusCode::SUCCESS;
}


StatusCode sTGCSDOVariables::initializeVariables()
{
  m_NSWsTGC_nsdo = 0;
  m_NSWsTGC_sdo_stationName  = new std::vector<std::string>();
  m_NSWsTGC_sdo_stationEta   = new std::vector<int>();
  m_NSWsTGC_sdo_stationPhi   = new std::vector<int>();
  m_NSWsTGC_sdo_multiplet    = new std::vector<int>();
  m_NSWsTGC_sdo_gas_gap      = new std::vector<int>();
  m_NSWsTGC_sdo_channel      = new std::vector<int>();
  m_NSWsTGC_sdo_channel_type = new std::vector<int>();

  m_NSWsTGC_sdo_word       = new std::vector<int>();
  m_NSWsTGC_sdo_barcode    = new std::vector<int>();
  m_NSWsTGC_sdo_globalPosX = new std::vector<double>();
  m_NSWsTGC_sdo_globalPosY = new std::vector<double>();
  m_NSWsTGC_sdo_globalPosZ = new std::vector<double>();
  m_NSWsTGC_sdo_globaltime = new std::vector<double>();

  m_NSWsTGC_sdo_E    = new std::vector<double>();
  m_NSWsTGC_sdo_tof  = new std::vector<double>();

  m_NSWsTGC_sdo_localPosX = new std::vector<double>;
  m_NSWsTGC_sdo_localPosY = new std::vector<double>;

  if(m_tree) {
    
    m_tree->Branch("SDO_sTGC", &m_NSWsTGC_nsdo, "SDOs_sTGC_n/i");
    m_tree->Branch("SDO_sTGC_stationName",   &m_NSWsTGC_sdo_stationName);
    m_tree->Branch("SDO_sTGC_stationEta",    &m_NSWsTGC_sdo_stationEta); 
    m_tree->Branch("SDO_sTGC_stationPhi",    &m_NSWsTGC_sdo_stationPhi);
    m_tree->Branch("SDO_sTGC_multiplet",     &m_NSWsTGC_sdo_multiplet);
    m_tree->Branch("SDO_sTGC_gas_gap",       &m_NSWsTGC_sdo_gas_gap);
    m_tree->Branch("SDO_sTGC_channel",       &m_NSWsTGC_sdo_channel);
    m_tree->Branch("SDO_sTGC_channel_type",  &m_NSWsTGC_sdo_channel_type);

    m_tree->Branch("SDO_sTGC_word",          &m_NSWsTGC_sdo_word);
    m_tree->Branch("SDO_sTGC_barcode",       &m_NSWsTGC_sdo_barcode);
    m_tree->Branch("SDO_sTGC_globalPosX",    &m_NSWsTGC_sdo_globalPosX);
    m_tree->Branch("SDO_sTGC_globalPosY",    &m_NSWsTGC_sdo_globalPosY);
    m_tree->Branch("SDO_sTGC_globalPosZ",    &m_NSWsTGC_sdo_globalPosZ);
    m_tree->Branch("SDO_sTGC_global_time",   &m_NSWsTGC_sdo_globaltime);
    m_tree->Branch("SDO_sTGC_Energy",        &m_NSWsTGC_sdo_E);
    m_tree->Branch("SDO_sTGC_tof",           &m_NSWsTGC_sdo_tof);

    m_tree->Branch("SDO_sTGC_localPosX",     &m_NSWsTGC_sdo_localPosX);
    m_tree->Branch("SDO_sTGC_localPosY",     &m_NSWsTGC_sdo_localPosY);

  }

  return StatusCode::SUCCESS;
}