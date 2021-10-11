/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCSDOVariables.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "TTree.h"

StatusCode CscSDOVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillCscSDOVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

  ATH_CHECK( this->clearVariables() );

  ATH_MSG_DEBUG( "Retrieve CSC SDO container with name = " << "CSC_SDO" );
  const CscSimDataCollection* cscSdoContainer = nullptr;
  ATH_CHECK( m_evtStore->retrieve(cscSdoContainer, "CSC_SDO") );

  for (const auto& coll : *cscSdoContainer ) {

    const Identifier id = coll.first;
    const CscSimData csc_sdo = coll.second;

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
    std::vector<CscSimData::Deposit> deposits;
    csc_sdo.deposits(deposits);

    int    truth_barcode   = deposits[0].first.barcode();
    double truth_localPosX = deposits[0].second.zpos();
    double truth_localPosY = deposits[0].second.ypos();
    double truth_charge    = deposits[0].second.charge();

    m_csc_sdo_word.push_back( csc_sdo.word() );    

    const MuonGM::CscReadoutElement* rdoEl = MuonDetMgr->getCscReadoutElement(id);
	if (!rdoEl) {
	  ATH_MSG_ERROR("CSCSDOVariables::fillVariables() - Failed to retrieve CscReadoutElement for" << __FILE__ << __LINE__ << m_CscIdHelper->print_to_string(id).c_str());
	  return StatusCode::FAILURE;
	}

    Amg::Vector2D hit_on_surface(truth_localPosX, truth_localPosY);
    Amg::Vector3D hit_gpos(0., 0., 0.);
    rdoEl->surface(id).localToGlobal(hit_on_surface, Amg::Vector3D(0., 0., 0.), hit_gpos);

    ATH_MSG_DEBUG("CSC Digit, truth barcode=" << truth_barcode);
    ATH_MSG_DEBUG("CSC Digit, truth localPosX=" << std::setw(9) << std::setprecision(2) << truth_localPosX
                  << ", truth localPosY=" << std::setw(9) << std::setprecision(2) << truth_localPosY
                  << ", truth charge=" << std::setw(8) << std::setprecision(5) << truth_charge);

    // truth information like positions and barcode
    // to be stored in the ntuple
    m_csc_sdo_barcode.push_back( truth_barcode );

    if ( rdoEl->surface(id).insideBounds(hit_on_surface,0.,0.) ){
      m_csc_sdo_localPosX.push_back( truth_localPosX );
      m_csc_sdo_localPosY.push_back( truth_localPosY );
    }

    if ( rdoEl->surface(id).isOnSurface(hit_gpos,true,0.,0.) ){
      m_csc_sdo_globalPosX.push_back( hit_gpos[0] );
      m_csc_sdo_globalPosY.push_back( hit_gpos[1] );
      m_csc_sdo_globalPosZ.push_back( hit_gpos[2] );
    }

    m_csc_sdo_charge.push_back( truth_charge );


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
  m_csc_sdo_charge.clear();
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
    m_tree->Branch("SDO_CSC_charge",      &m_csc_sdo_charge);
    m_tree->Branch("SDO_CSC_localPosX",     &m_csc_sdo_localPosX);
    m_tree->Branch("SDO_CSC_localPosY",     &m_csc_sdo_localPosY);
  }
  return StatusCode::SUCCESS;
}