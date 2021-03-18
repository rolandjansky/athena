/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTDigitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/MdtDigit.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MuonDigitContainer/MdtDigitCollection.h"

#include "TTree.h"
/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode MdtDigitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillMdtDigitVariables()");
  CHECK( this->clearVariables() );

  const MdtDigitContainer* MdtDigitContainer = nullptr;
  CHECK( m_evtStore->retrieve(MdtDigitContainer, m_ContainerName.c_str()) );

  ATH_MSG_DEBUG("retrieved MDT Digit Container with size "<<MdtDigitContainer->digit_size());

  if(MdtDigitContainer->size()==0) ATH_MSG_DEBUG(" MDT Digit Continer empty ");
  for(const MdtDigitCollection* coll : *MdtDigitContainer) {
    ATH_MSG_DEBUG( "processing collection with size " << coll->size() );
    for (unsigned int digitNum=0; digitNum<coll->size(); digitNum++) {
      const MdtDigit* digit = coll->at(digitNum);
      Identifier Id = digit->identify();

      std::string stName   = m_MdtIdHelper->stationNameString(m_MdtIdHelper->stationName(Id));
      int stationEta       = m_MdtIdHelper->stationEta(Id);
      int stationPhi       = m_MdtIdHelper->stationPhi(Id);
      int multiplet        = m_MdtIdHelper->multilayer(Id);
      int channel          = m_MdtIdHelper->channel(Id);
      int stationEtaMin    = m_MdtIdHelper->stationEtaMin(Id);
      int stationEtaMax    = m_MdtIdHelper->stationEtaMax(Id);
      int stationPhiMin    = m_MdtIdHelper->stationPhiMin(Id);
      int stationPhiMax    = m_MdtIdHelper->stationPhiMax(Id);
      int NofMultilayers   = m_MdtIdHelper->numberOfMultilayers(Id);
      int multilayerMin    = m_MdtIdHelper->multilayerMin(Id);
      int multilayerMax    = m_MdtIdHelper->multilayerMax(Id);
      int tubeLayerMin     = m_MdtIdHelper->tubeLayerMin(Id);
      int tubeLayerMax     = m_MdtIdHelper->tubeLayerMax(Id);
      int tubeMin          = m_MdtIdHelper->tubeMin(Id);
      int tubeMax          = m_MdtIdHelper->tubeMax(Id);

      ATH_MSG_DEBUG(     "MDT Digit Offline id:  Station Name [" << stName << " ]"
                         << " Station Eta ["  << stationEta      << "]"
                         << " Station Phi ["  << stationPhi      << "]"
                         << " Multiplet  ["   << multiplet       << "]"
                         << " ChNr ["         << channel         << "]"
                         << " Station EtaMin ["  << stationEtaMin      << "]"
                         << " Station EtaMax ["  << stationEtaMax      << "]"
                         << " Station PhiMin ["  << stationPhiMin      << "]"
                         << " Station PhiMax ["  << stationPhiMax      << "]");

      const MuonGM::MdtReadoutElement* rdoEl = MuonDetMgr->getMdtReadoutElement(Id);
      if (!rdoEl) return StatusCode::FAILURE;

      Amg::Vector3D gpos(0.,0.,0.);
      Amg::Vector2D lpos(0.,0.);

      rdoEl->surface(Id).localToGlobal(lpos, gpos,gpos);
      
      m_MDT_dig_localPosX.push_back( lpos.x() );
      m_MDT_dig_localPosY.push_back( lpos.y() );
      m_MDT_dig_globalPosX.push_back( gpos.x() );
      m_MDT_dig_globalPosY.push_back( gpos.y() );
      m_MDT_dig_globalPosZ.push_back( gpos.z() );
      
      m_MDT_dig_stationName.push_back(stName);
      m_MDT_dig_stationEta.push_back(stationEta);
      m_MDT_dig_stationPhi.push_back(stationPhi);
      m_MDT_dig_multiplet.push_back(multiplet);
      m_MDT_dig_channel.push_back(channel);
      m_MDT_dig_stationEtaMin.push_back(stationEtaMin);
      m_MDT_dig_stationEtaMax.push_back(stationEtaMax);
      m_MDT_dig_stationPhiMin.push_back(stationPhiMin);
      m_MDT_dig_stationPhiMax.push_back(stationPhiMax);
      m_MDT_dig_numberOfMultilayers.push_back(NofMultilayers);
      m_MDT_dig_multilayerMin.push_back(multilayerMin);
      m_MDT_dig_multilayerMax.push_back(multilayerMax);
      m_MDT_dig_tubeLayerMin.push_back(tubeLayerMin);
      m_MDT_dig_tubeLayerMax.push_back(tubeLayerMax);
      m_MDT_dig_tubeMin.push_back(tubeMin);
      m_MDT_dig_tubeMax.push_back(tubeMax);
      
      m_MDT_nDigits++;
    }
  }
  ATH_MSG_DEBUG(" finished fillMdtDigitVariables()");
  return StatusCode::SUCCESS;
}

/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode MdtDigitVariables::clearVariables()
{
  m_MDT_nDigits = 0;
  m_MDT_dig_stationName.clear();
  m_MDT_dig_stationEta.clear();
  m_MDT_dig_stationPhi.clear();
  m_MDT_dig_multiplet.clear();
  m_MDT_dig_channel.clear();
  m_MDT_dig_stationEtaMin.clear();
  m_MDT_dig_stationEtaMax.clear();
  m_MDT_dig_stationPhiMin.clear();
  m_MDT_dig_stationPhiMax.clear();
  m_MDT_dig_numberOfMultilayers.clear();
  m_MDT_dig_multilayerMin.clear();
  m_MDT_dig_multilayerMax.clear();
  m_MDT_dig_tubeLayerMin.clear();
  m_MDT_dig_tubeLayerMax.clear();
  m_MDT_dig_tubeMin.clear();
  m_MDT_dig_tubeMax.clear(); 
  m_MDT_dig_channelPosX.clear();
  m_MDT_dig_channelPosY.clear();
  m_MDT_dig_localPosX.clear();
  m_MDT_dig_localPosY.clear();
  m_MDT_dig_globalPosX.clear();
  m_MDT_dig_globalPosY.clear();
  m_MDT_dig_globalPosZ.clear();

  return StatusCode::SUCCESS;
}

/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode MdtDigitVariables::initializeVariables()
{

  if(m_tree) {
    ATH_MSG_DEBUG("MDT digit:  init m_tree ");
    m_tree->Branch("Digits_MDT",              &m_MDT_nDigits, "Digits_MDT_n/i");
    m_tree->Branch("Digits_MDT_stationName", &m_MDT_dig_stationName);
    m_tree->Branch("Digits_MDT_stationEta",  &m_MDT_dig_stationEta);
    m_tree->Branch("Digits_MDT_stationPhi",  &m_MDT_dig_stationPhi);
    m_tree->Branch("Digits_MDT_stationEtaMin",  &m_MDT_dig_stationEtaMin);
    m_tree->Branch("Digits_MDT_stationEtaMax",  &m_MDT_dig_stationEtaMax);
    m_tree->Branch("Digits_MDT_stationPhiMin",  &m_MDT_dig_stationPhiMin);
    m_tree->Branch("Digits_MDT_stationPhiMax",  &m_MDT_dig_stationPhiMax);
    m_tree->Branch("Digits_MDT_numberOfMultilayers",  &m_MDT_dig_numberOfMultilayers);
    m_tree->Branch("Digits_MDT_multilayerMin",  &m_MDT_dig_multilayerMin);
    m_tree->Branch("Digits_MDT_multilayerMax",  &m_MDT_dig_multilayerMax);
    
    m_tree->Branch("Digits_MDT_tubeLayerMin",  &m_MDT_dig_tubeLayerMin);
    m_tree->Branch("Digits_MDT_tubeLayerMax",  &m_MDT_dig_tubeLayerMax);
    m_tree->Branch("Digits_MDT_tubeMin",  &m_MDT_dig_tubeMin);
    m_tree->Branch("Digits_MDT_tubeMax",  &m_MDT_dig_tubeMax);
    
    m_tree->Branch("Digits_MDT_channelPosX",  &m_MDT_dig_channelPosX);
    m_tree->Branch("Digits_MDT_channelPosY",  &m_MDT_dig_channelPosY);
    m_tree->Branch("Digits_MDT_localPosX", &m_MDT_dig_localPosX);
    m_tree->Branch("Digits_MDT_localPosY", &m_MDT_dig_localPosY);
    m_tree->Branch("Digits_MDT_globalPosX", &m_MDT_dig_globalPosX);
    m_tree->Branch("Digits_MDT_globalPosY",  &m_MDT_dig_globalPosY);
    m_tree->Branch("Digits_MDT_globalPosZ",  &m_MDT_dig_globalPosZ);
  }

  return StatusCode::SUCCESS;
}
