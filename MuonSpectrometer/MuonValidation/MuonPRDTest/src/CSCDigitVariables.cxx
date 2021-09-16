/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCDigitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonDigitContainer/CscDigit.h"

#include "MuonReadoutGeometry/CscReadoutElement.h"

#include <TString.h> // for Form
#include "TTree.h"

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode CSCDigitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG("do fillCSCDigitVariables()");

  // clear variables
  CHECK( this->clearVariables() );

  // get digit container (a container corresponds to a multilayer of a module)
  const CscDigitContainer *csc_DigitContainer = nullptr;
  CHECK( m_evtStore->retrieve(csc_DigitContainer, m_ContainerName.c_str()) );

  if(csc_DigitContainer->size()==0) ATH_MSG_WARNING(" CSC DigitContainer empty ");

  // iteration on all containers, i.e. all multilayers of all modules
  for(auto it : *csc_DigitContainer) {
    // a digit collection is instanciated for each container, i.e. holds all digits of a multilayer
    const CscDigitCollection* coll = it;

    for (auto digit: *coll) {

      // get specific digit and identify it
      Identifier Id = digit->identify();

      std::string stName   = m_CscIdHelper->stationNameString(m_CscIdHelper->stationName(Id));
      int stationEta       = m_CscIdHelper->stationEta(Id);
      int stationPhi       = m_CscIdHelper->stationPhi(Id);
      int chlayer          = m_CscIdHelper->chamberLayer(Id);
      int wlayer           = m_CscIdHelper->wireLayer(Id);
      int gas_gap          = m_CscIdHelper->gasGap(Id);
      int channel          = m_CscIdHelper->strip(Id);
    
      ATH_MSG_DEBUG(     "CSC Digit Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " ChamberLayer  ["   << chlayer       << "]"
                      << " wireLayer  ["   << wlayer       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " ChNr ["         << channel         << "]" );

      // module details down to the level of channel which is closest to the Geant4 hit
      // to be stored in the ntuple
      m_CSC_dig_stationName.push_back(stName);
      m_CSC_dig_stationEta.push_back(stationEta);
      m_CSC_dig_stationPhi.push_back(stationPhi);
      m_CSC_dig_chlayer.push_back(chlayer);
      m_CSC_dig_wlayer.push_back(wlayer);
      m_CSC_dig_gas_gap.push_back(gas_gap);
      m_CSC_dig_channel.push_back(channel);

      const MuonGM::CscReadoutElement* rdoEl = MuonDetMgr->getCscReadoutElement(Id);
	  if (!rdoEl) {
	    ATH_MSG_ERROR("CSCDigitVariables::fillVariables() - Failed to retrieve CscReadoutElement for" << __FILE__ << __LINE__ << m_CscIdHelper->print_to_string(Id).c_str());
	    return StatusCode::FAILURE;
	  }


      Amg::Vector3D gpos(0.,0.,0.);
      Amg::Vector2D lpos(0.,0.);

      rdoEl->surface(Id).localToGlobal(lpos,gpos,gpos);

      m_CSC_dig_globalPosX.push_back( gpos.x() );
      m_CSC_dig_globalPosY.push_back( gpos.y() );
      m_CSC_dig_globalPosZ.push_back( gpos.z() );
      
      // digit counter for the ntuple
      m_CSC_nDigits++;
    }

    // Local digit position information loss after localToGlobal transformation, fill the local positions in another loop for retrieving the local positions
    for (auto Digit: *coll) {

      // get specific digit and identify it
      Identifier id = Digit->identify();

      const MuonGM::CscReadoutElement* rdoEl = MuonDetMgr->getCscReadoutElement(id);
	  if (!rdoEl) {
	    ATH_MSG_ERROR("CSCDigitVariables::fillVariables() - Failed to retrieve CscReadoutElement for" << __FILE__ << __LINE__ << m_CscIdHelper->print_to_string(id).c_str());
	    return StatusCode::FAILURE;
	  }

      Amg::Vector3D glopos(0.,0.,0.);
      Amg::Vector2D lopos(0.,0.);

      rdoEl->surface(id).globalToLocal(glopos,glopos,lopos);
      m_CSC_dig_localPosX.push_back( lopos.x() );
      m_CSC_dig_localPosY.push_back( lopos.y() );
    }

  }

  ATH_MSG_DEBUG("processed " << m_CSC_nDigits << " CSC hits");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode CSCDigitVariables::clearVariables()
{
  m_CSC_nDigits = 0;

  m_CSC_dig_stationName.clear();
  m_CSC_dig_stationEta.clear();
  m_CSC_dig_stationPhi.clear();
  m_CSC_dig_chlayer.clear();
  m_CSC_dig_wlayer.clear();
  m_CSC_dig_gas_gap.clear();
  m_CSC_dig_channel.clear();

  m_CSC_dig_localPosX.clear();
  m_CSC_dig_localPosY.clear();
  m_CSC_dig_globalPosX.clear();
  m_CSC_dig_globalPosY.clear();
  m_CSC_dig_globalPosZ.clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode CSCDigitVariables::initializeVariables()
{

  if(m_tree) {
    m_tree->Branch("Digits_CSC",    &m_CSC_nDigits, "Digits_CSC_n/i");
    m_tree->Branch("Digits_CSC_stationName", &m_CSC_dig_stationName);
    m_tree->Branch("Digits_CSC_stationEta",  &m_CSC_dig_stationEta);
    m_tree->Branch("Digits_CSC_stationPhi",  &m_CSC_dig_stationPhi);
    m_tree->Branch("Digits_CSC_chamberLayer",   &m_CSC_dig_chlayer);
    m_tree->Branch("Digits_CSC_wireLayer",   &m_CSC_dig_wlayer);
    m_tree->Branch("Digits_CSC_gas_gap",     &m_CSC_dig_gas_gap);
    m_tree->Branch("Digits_CSC_channel",     &m_CSC_dig_channel);
    m_tree->Branch("Digits_CSC_localPosX",  &m_CSC_dig_localPosX);
    m_tree->Branch("Digits_CSC_localPosY",  &m_CSC_dig_localPosY);
    m_tree->Branch("Digits_CSC_globalPosX", &m_CSC_dig_globalPosX);
    m_tree->Branch("Digits_CSC_globalPosY", &m_CSC_dig_globalPosY);
    m_tree->Branch("Digits_CSC_globalPosZ", &m_CSC_dig_globalPosZ);
  }

  return StatusCode::SUCCESS;
}

void CSCDigitVariables::deleteVariables()
{
  return;
}