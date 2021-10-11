/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCPRDVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonPrepRawData/CscPrepDataContainer.h"

#include "MuonReadoutGeometry/CscReadoutElement.h"

#include "TTree.h"

StatusCode CSCPRDVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG("do fillCSCPRDVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);
  CHECK( this->clearVariables() );

  const Muon::CscPrepDataContainer *Csc_PrepDataContainer = nullptr; 
  CHECK( m_evtStore->retrieve(Csc_PrepDataContainer, m_ContainerName.c_str()) );

  if(Csc_PrepDataContainer->size()==0) ATH_MSG_WARNING(" CSC PRD Container empty ");

  for(auto it : *Csc_PrepDataContainer ) {

    const Muon::CscPrepDataCollection* coll = it;
    
    for (auto prd: *coll) {

      Identifier Id = prd->identify();

      std::string stName   = m_CscIdHelper->stationNameString(m_CscIdHelper->stationName(Id));
      int stationEta       = m_CscIdHelper->stationEta(Id);
      int stationPhi       = m_CscIdHelper->stationPhi(Id);
      int channel          = m_CscIdHelper->channel(Id);
      int chlayer          = m_CscIdHelper->chamberLayer(Id);
      int wlayer           = m_CscIdHelper->wireLayer(Id);
      int measuresPhi       = m_CscIdHelper->measuresPhi(Id);
      int strip          = m_CscIdHelper->strip(Id);
      int charge           = prd->charge();
      int time           = prd->time();

      ATH_MSG_DEBUG(     "CSC PRD Offline id:  Station Name [" << stName << "]"
                      << " Station Eta   ["  << stationEta      << "]"
                      << " Station Phi   ["  << stationPhi      << "]"
                      << " ChNr          ["  << channel         << "]"
                      << " chamber layer ["  << chlayer         << "]"
                      << " wire layer    ["  << wlayer          << "]"
                      << " measures phi   ["  << measuresPhi      << "]"
                      << " strip         ["  << strip           << "]" );

      m_CSC_PRD_stationName.push_back(stName);
      m_CSC_PRD_stationEta.push_back(stationEta);
      m_CSC_PRD_stationPhi.push_back(stationPhi);
      m_CSC_PRD_channel.push_back(channel);
      m_CSC_PRD_chlayer.push_back(chlayer);
      m_CSC_PRD_wlayer.push_back(wlayer);
      m_CSC_PRD_measuresPhi.push_back(measuresPhi);
      m_CSC_PRD_strip.push_back(strip);
      m_CSC_PRD_charge.push_back(charge);
      m_CSC_PRD_time.push_back(time);

      const MuonGM::CscReadoutElement* det = prd->detectorElement();
      if (!det) throw std::runtime_error(Form("File: %s, Line: %d\nCSCPRDVariables::fillVariables() - no associated detectorElement", __FILE__, __LINE__));
      Amg::Vector3D pos = prd->globalPosition();
      Amg::Vector2D loc_pos(0., 0.);

      det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

      m_CSC_PRD_globalPosX.push_back(pos.x());
      m_CSC_PRD_globalPosY.push_back(pos.y());
      m_CSC_PRD_globalPosZ.push_back(pos.z());

      m_CSC_PRD_localPosX.push_back(loc_pos[0]);
      m_CSC_PRD_localPosY.push_back(loc_pos[1]);

      m_CSC_PRD_nPRDs++;
    }
  }

  ATH_MSG_DEBUG("processed " << m_CSC_PRD_nPRDs << " CSC PRD's");

  return StatusCode::SUCCESS;
}

StatusCode CSCPRDVariables::clearVariables()
{

  m_CSC_PRD_nPRDs = 0;

  m_CSC_PRD_stationName.clear();
  m_CSC_PRD_stationEta.clear();
  m_CSC_PRD_stationPhi.clear();
  m_CSC_PRD_channel.clear();
  m_CSC_PRD_chlayer.clear();
  m_CSC_PRD_wlayer.clear();
  m_CSC_PRD_measuresPhi.clear();
  m_CSC_PRD_strip.clear();
  m_CSC_PRD_charge.clear();
  m_CSC_PRD_time.clear();

  m_CSC_PRD_globalPosX.clear();
  m_CSC_PRD_globalPosY.clear();
  m_CSC_PRD_globalPosZ.clear();

  m_CSC_PRD_localPosX.clear();
  m_CSC_PRD_localPosY.clear();

  return StatusCode::SUCCESS;
}

StatusCode CSCPRDVariables::initializeVariables()
{

  if(m_tree) {
    m_tree->Branch("PRD_CSC", &m_CSC_PRD_nPRDs, "PRDs_CSC_n/i");
    m_tree->Branch("PRD_CSC_stationName",   &m_CSC_PRD_stationName);
    m_tree->Branch("PRD_CSC_stationEta",    &m_CSC_PRD_stationEta);
    m_tree->Branch("PRD_CSC_stationPhi",    &m_CSC_PRD_stationPhi);
    m_tree->Branch("PRD_CSC_channel",       &m_CSC_PRD_channel);
    m_tree->Branch("PRD_CSC_chlayer",       &m_CSC_PRD_chlayer);
    m_tree->Branch("PRD_CSC_wlayer",        &m_CSC_PRD_wlayer);
    m_tree->Branch("PRD_CSC_measuresPhi",    &m_CSC_PRD_measuresPhi);
    m_tree->Branch("PRD_CSC_strip",         &m_CSC_PRD_strip);
    m_tree->Branch("PRD_CSC_charge",         &m_CSC_PRD_charge);
    m_tree->Branch("PRD_CSC_time",         &m_CSC_PRD_time);

    m_tree->Branch("PRD_CSC_globalPosX",    &m_CSC_PRD_globalPosX);
    m_tree->Branch("PRD_CSC_globalPosY",    &m_CSC_PRD_globalPosY);
    m_tree->Branch("PRD_CSC_globalPosZ",    &m_CSC_PRD_globalPosZ);

    m_tree->Branch("PRD_CSC_localPosX",     &m_CSC_PRD_localPosX);
    m_tree->Branch("PRD_CSC_localPosY",     &m_CSC_PRD_localPosY);
  }

  return StatusCode::SUCCESS;
}
