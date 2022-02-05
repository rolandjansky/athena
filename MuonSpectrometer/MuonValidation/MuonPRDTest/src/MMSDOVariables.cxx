/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MMSDOVariables.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include "TTree.h"


StatusCode MMSDOVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillNSWMMSDOVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

  ATH_CHECK( this->clearVariables() );

  ATH_MSG_DEBUG( "Retrieve MM SDO container with name = " << m_ContainerName.c_str() );
  const MuonSimDataCollection* nsw_MmSdoContainer = nullptr;
  ATH_CHECK( m_evtStore->retrieve(nsw_MmSdoContainer, m_ContainerName.c_str()) );

  for ( const auto& coll : *nsw_MmSdoContainer ) {

    Identifier Id = coll.first;
    const MuonSimData mm_sdo = coll.second;

    // Get information on the SDO
    std::string stName   = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(Id));
    int stationEta       = m_MmIdHelper->stationEta(Id);
    int stationPhi       = m_MmIdHelper->stationPhi(Id);
    int multiplet        = m_MmIdHelper->multilayer(Id);
    int gas_gap          = m_MmIdHelper->gasGap(Id);
    int channel          = m_MmIdHelper->channel(Id);


    ATH_MSG_DEBUG(     "MicroMegas SDO:  Station Name [" << stName << " ]"
                    << " Station Eta ["  << stationEta      << "]"
                    << " Station Phi ["  << stationPhi      << "]"
                    << " Multiplet  ["   << multiplet       << "]"
                    << " GasGap ["       << gas_gap         << "]"
                    << " ChNr ["         << channel         << "]" );

    m_NSWMM_sdo_stationName->push_back(stName);
    m_NSWMM_sdo_stationEta->push_back(stationEta);
    m_NSWMM_sdo_stationPhi->push_back(stationPhi);
    m_NSWMM_sdo_multiplet->push_back(multiplet);
    m_NSWMM_sdo_gas_gap->push_back(gas_gap);
    m_NSWMM_sdo_channel->push_back(channel);

    ATH_MSG_DEBUG( "Get the truth deposits from the SDO." );
    std::vector<MuonSimData::Deposit> deposits;
    mm_sdo.deposits(deposits);

    const Amg::Vector3D hit_gpos = mm_sdo.globalPosition();
    m_NSWMM_sdo_globalPosX->push_back( hit_gpos.x() );
    m_NSWMM_sdo_globalPosY->push_back( hit_gpos.y() );
    m_NSWMM_sdo_globalPosZ->push_back( hit_gpos.z() );

    m_NSWMM_sdo_globaltime->push_back( mm_sdo.getTime() );
    m_NSWMM_sdo_word->push_back( mm_sdo.word() );    

    // use the information of the first deposit
    int barcode = deposits[0].first.barcode();
    double MuonMCdata_firstentry = deposits[0].second.firstEntry();
    double MuonMCdata_secondentry = deposits[0].second.secondEntry();

    ATH_MSG_DEBUG("MicroMegas SDO barcode=" << barcode);
    ATH_MSG_DEBUG("MicroMegas SDO localPosX=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                               << ", localPosY=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

    m_NSWMM_sdo_barcode->push_back( barcode );
    m_NSWMM_sdo_localPosX->push_back( MuonMCdata_firstentry );
    m_NSWMM_sdo_localPosY->push_back( MuonMCdata_secondentry );

    m_NSWMM_nsdo++;
  }

  ATH_MSG_DEBUG("Processed " << m_NSWMM_nsdo << " MicroMegas SDOs");
  return StatusCode::SUCCESS;
}


void MMSDOVariables::deleteVariables()
{
  delete m_NSWMM_sdo_stationName;
  delete m_NSWMM_sdo_stationEta;
  delete m_NSWMM_sdo_stationPhi;
  delete m_NSWMM_sdo_multiplet;
  delete m_NSWMM_sdo_gas_gap;
  delete m_NSWMM_sdo_channel;

  delete m_NSWMM_sdo_word;
  delete m_NSWMM_sdo_barcode;
  delete m_NSWMM_sdo_globalPosX;
  delete m_NSWMM_sdo_globalPosY;
  delete m_NSWMM_sdo_globalPosZ;
  delete m_NSWMM_sdo_globaltime;

  delete m_NSWMM_sdo_localPosX;
  delete m_NSWMM_sdo_localPosY;

  m_NSWMM_nsdo    = 0;
  m_NSWMM_sdo_stationName= nullptr;
  m_NSWMM_sdo_stationEta = nullptr;
  m_NSWMM_sdo_stationPhi = nullptr;
  m_NSWMM_sdo_multiplet  = nullptr;
  m_NSWMM_sdo_gas_gap    = nullptr;
  m_NSWMM_sdo_channel    = nullptr;

  m_NSWMM_sdo_word       = nullptr;
  m_NSWMM_sdo_barcode    = nullptr;
  m_NSWMM_sdo_globalPosX = nullptr;
  m_NSWMM_sdo_globalPosY = nullptr;
  m_NSWMM_sdo_globalPosZ = nullptr;
  m_NSWMM_sdo_globaltime = nullptr;

  m_NSWMM_sdo_localPosX  = nullptr;
  m_NSWMM_sdo_localPosY  = nullptr;

  return;
}


StatusCode MMSDOVariables::clearVariables()
{
  m_NSWMM_nsdo = 0;
  m_NSWMM_sdo_stationName->clear();
  m_NSWMM_sdo_stationEta->clear();
  m_NSWMM_sdo_stationPhi->clear();
  m_NSWMM_sdo_multiplet->clear();
  m_NSWMM_sdo_gas_gap->clear();
  m_NSWMM_sdo_channel->clear();

  m_NSWMM_sdo_word->clear();
  m_NSWMM_sdo_barcode->clear();
  m_NSWMM_sdo_globalPosX->clear();
  m_NSWMM_sdo_globalPosY->clear();
  m_NSWMM_sdo_globalPosZ->clear();
  m_NSWMM_sdo_globaltime->clear();
  m_NSWMM_sdo_localPosX->clear();
  m_NSWMM_sdo_localPosY->clear();

  return StatusCode::SUCCESS;
}


StatusCode MMSDOVariables::initializeVariables()
{
  m_NSWMM_nsdo = 0;
  m_NSWMM_sdo_stationName = new std::vector<std::string>();
  m_NSWMM_sdo_stationEta = new std::vector<int>();
  m_NSWMM_sdo_stationPhi = new std::vector<int>();
  m_NSWMM_sdo_multiplet  = new std::vector<int>();
  m_NSWMM_sdo_gas_gap    = new std::vector<int>();
  m_NSWMM_sdo_channel    = new std::vector<int>();

  m_NSWMM_sdo_word       = new std::vector<int>();
  m_NSWMM_sdo_barcode    = new std::vector<int>();
  m_NSWMM_sdo_globalPosX = new std::vector<double>();
  m_NSWMM_sdo_globalPosY = new std::vector<double>();
  m_NSWMM_sdo_globalPosZ = new std::vector<double>();
  m_NSWMM_sdo_globaltime = new std::vector<double>();

  m_NSWMM_sdo_localPosX  = new std::vector<double>();
  m_NSWMM_sdo_localPosY  = new std::vector<double>();

  if(m_tree) {
    m_tree->Branch("SDO_MM", &m_NSWMM_nsdo, "SDOs_MM_n/i");
    m_tree->Branch("SDO_MM_stationName",   &m_NSWMM_sdo_stationName);
    m_tree->Branch("SDO_MM_stationEta",    &m_NSWMM_sdo_stationEta); 
    m_tree->Branch("SDO_MM_stationPhi",    &m_NSWMM_sdo_stationPhi);
    m_tree->Branch("SDO_MM_multiplet",     &m_NSWMM_sdo_multiplet);
    m_tree->Branch("SDO_MM_gas_gap",       &m_NSWMM_sdo_gas_gap);
    m_tree->Branch("SDO_MM_channel",       &m_NSWMM_sdo_channel);

    m_tree->Branch("SDO_MM_word",          &m_NSWMM_sdo_word);
    m_tree->Branch("SDO_MM_barcode",       &m_NSWMM_sdo_barcode);
    m_tree->Branch("SDO_MM_globalPosX",    &m_NSWMM_sdo_globalPosX);
    m_tree->Branch("SDO_MM_globalPosY",    &m_NSWMM_sdo_globalPosY);
    m_tree->Branch("SDO_MM_globalPosZ",    &m_NSWMM_sdo_globalPosZ);
    m_tree->Branch("SDO_MM_global_time",   &m_NSWMM_sdo_globaltime);
    m_tree->Branch("SDO_MM_localPosX",     &m_NSWMM_sdo_localPosX);
    m_tree->Branch("SDO_MM_localPosY",     &m_NSWMM_sdo_localPosY);

  }

  return StatusCode::SUCCESS;
}
