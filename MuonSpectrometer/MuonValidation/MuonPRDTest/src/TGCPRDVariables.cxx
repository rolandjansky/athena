/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCPRDVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "TTree.h"

StatusCode TGCPRDVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG("do fillTGCPRDVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);
  CHECK( this->clearVariables() );

  const Muon::TgcPrepDataContainer *Tgc_PrepDataContainer = nullptr; 
  CHECK( m_evtStore->retrieve(Tgc_PrepDataContainer, m_ContainerName.c_str()) );

  if(Tgc_PrepDataContainer->size()==0){
    ATH_MSG_DEBUG(" TGC PRD Container empty ");

  } 
  for(const Muon::TgcPrepDataCollection* coll: *Tgc_PrepDataContainer ) {

    for (auto prd: *coll) {

      Identifier Id = prd->identify();

      std::string stName   = m_TgcIdHelper->stationNameString(m_TgcIdHelper->stationName(Id));
      int stationEta       = m_TgcIdHelper->stationEta(Id);
      int stationPhi       = m_TgcIdHelper->stationPhi(Id);
      int channel          = m_TgcIdHelper->channel(Id);
      int gasgap           = m_TgcIdHelper->gasGap(Id);
      int isStrip          = m_TgcIdHelper->isStrip(Id);

      ATH_MSG_DEBUG(     "TGC PRD Offline id:  Station Name [" << stName << "]"
                      << " Station Eta   ["  << stationEta      << "]"
                      << " Station Phi   ["  << stationPhi      << "]"
                      << " ChNr          ["  << channel         << "]"
                      << " gasGap        ["  << gasgap          << "]"
                      << " isStrip       ["  << isStrip         << "]" );

      m_TGC_PRD_stationName.push_back(stName);
      m_TGC_PRD_stationEta.push_back(stationEta);
      m_TGC_PRD_stationPhi.push_back(stationPhi);
      m_TGC_PRD_channel.push_back(channel);
      m_TGC_PRD_gasGap.push_back(gasgap);
      m_TGC_PRD_isStrip.push_back(isStrip);

      const MuonGM::TgcReadoutElement* det = prd->detectorElement();
      if (!det) throw std::runtime_error(Form("File: %s, Line: %d\nTGCPRDVariables::fillVariables() - no associated detectorElement", __FILE__, __LINE__));
      Amg::Vector3D pos = prd->globalPosition();
      Amg::Vector2D loc_pos(0., 0.);

      det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

      m_TGC_PRD_globalPosX.push_back(pos.x());
      m_TGC_PRD_globalPosY.push_back(pos.y());
      m_TGC_PRD_globalPosZ.push_back(pos.z());

      m_TGC_PRD_localPosX.push_back(loc_pos[0]);
      m_TGC_PRD_localPosY.push_back(loc_pos[1]);

      m_TGC_PRD_nPRDs++;
    }
  }

  ATH_MSG_DEBUG("processed " << m_TGC_PRD_nPRDs << " TGC PRD's");

  return StatusCode::SUCCESS;
}

StatusCode TGCPRDVariables::clearVariables()
{

  m_TGC_PRD_nPRDs = 0;

  m_TGC_PRD_stationName.clear();
  m_TGC_PRD_stationEta.clear();
  m_TGC_PRD_stationPhi.clear();
  m_TGC_PRD_channel.clear();
  m_TGC_PRD_gasGap.clear();
  m_TGC_PRD_isStrip.clear();

  m_TGC_PRD_globalPosX.clear();
  m_TGC_PRD_globalPosY.clear();
  m_TGC_PRD_globalPosZ.clear();

  m_TGC_PRD_localPosX.clear();
  m_TGC_PRD_localPosY.clear();

  return StatusCode::SUCCESS;
}

StatusCode TGCPRDVariables::initializeVariables()
{

  if(m_tree) {
    m_tree->Branch("PRD_TGC", &m_TGC_PRD_nPRDs, "PRDs_TGC_n/i");
    m_tree->Branch("PRD_TGC_stationName",   &m_TGC_PRD_stationName);
    m_tree->Branch("PRD_TGC_stationEta",    &m_TGC_PRD_stationEta);
    m_tree->Branch("PRD_TGC_stationPhi",    &m_TGC_PRD_stationPhi);
    m_tree->Branch("PRD_TGC_channel",       &m_TGC_PRD_channel);
    m_tree->Branch("PRD_TGC_gasGap",        &m_TGC_PRD_gasGap);
    m_tree->Branch("PRD_TGC_isStrip",       &m_TGC_PRD_isStrip);

    m_tree->Branch("PRD_TGC_globalPosX",    &m_TGC_PRD_globalPosX);
    m_tree->Branch("PRD_TGC_globalPosY",    &m_TGC_PRD_globalPosY);
    m_tree->Branch("PRD_TGC_globalPosZ",    &m_TGC_PRD_globalPosZ);

    m_tree->Branch("PRD_TGC_localPosX",     &m_TGC_PRD_localPosX);
    m_tree->Branch("PRD_TGC_localPosY",     &m_TGC_PRD_localPosY);
  }

  return StatusCode::SUCCESS;
}
