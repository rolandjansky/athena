/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCDigitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigit.h"

#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "TTree.h"

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode TGCDigitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG("do fillTGCDigitVariables()");

  // clear variables
  CHECK( this->clearVariables() );

  const TgcDigitContainer *tgc_DigitContainer = nullptr;
  CHECK( m_evtStore->retrieve(tgc_DigitContainer, m_ContainerName.c_str()) );

  if(tgc_DigitContainer->size()==0) ATH_MSG_WARNING(" TGC DigitContainer empty ");

    // iteration on all containers, i.e. all multilayers of all modules
  for(auto it : *tgc_DigitContainer) {
    // a digit collection is instanciated for each container, i.e. holds all digits of a multilayer
    const TgcDigitCollection* coll = it;

    // loop on all digits inside a collection, i.e. multilayer
    for (auto digit: *coll) {

      // get specific digit and identify it
      Identifier Id = digit->identify();

      std::string stName   = m_TgcIdHelper->stationNameString(m_TgcIdHelper->stationName(Id));
      int stationEta       = m_TgcIdHelper->stationEta(Id);
      int stationPhi       = m_TgcIdHelper->stationPhi(Id);
      int gas_gap          = m_TgcIdHelper->gasGap(Id);
      int channel          = m_TgcIdHelper->channel(Id);
      int isStrip          = m_TgcIdHelper->isStrip(Id);
    
      ATH_MSG_DEBUG(     "MicroMegas Digit Offline id:  Station Name [" << stName << " ]"
                         << " Station Eta ["  << stationEta      << "]"
                         << " Station Phi ["  << stationPhi      << "]"
                         << " GasGap ["       << gas_gap         << "]"
                         << " ChNr ["         << channel         << "]"
                         << " isStrip ["      << isStrip         << "]");

      // module details down to the level of channel which is closest to the Geant4 hit
      // to be stored in the ntuple
      m_TGC_dig_stationName.push_back(stName);
      m_TGC_dig_stationEta.push_back(stationEta);
      m_TGC_dig_stationPhi.push_back(stationPhi);
      m_TGC_dig_gas_gap.push_back(gas_gap);
      m_TGC_dig_channel.push_back(channel);
      m_TGC_dig_isStrip.push_back(isStrip);

	  if (!MuonDetMgr->getTgcReadoutElement(Id)) {
	    ATH_MSG_ERROR("TGCDigitVariables::fillVariables() - Failed to retrieve TgcReadoutElement for" << __FILE__ << __LINE__ << m_TgcIdHelper->print_to_string(Id).c_str());
	    return StatusCode::FAILURE;
	  }
      // digit counter for the ntuple
      m_TGC_nDigits++;
    }
  }
  ATH_MSG_DEBUG("processed " << m_TGC_nDigits << " MicroMegas hits");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode TGCDigitVariables::clearVariables()
{
  m_TGC_nDigits = 0;

  // information of the module down to the channel closest to the initial G4 hit
  // size of vector is m_TGC_nDigits
  m_TGC_dig_stationName.clear();
  m_TGC_dig_stationEta.clear();
  m_TGC_dig_stationPhi.clear();
  m_TGC_dig_gas_gap.clear();
  m_TGC_dig_channel.clear();
  m_TGC_dig_isStrip.clear();


  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode TGCDigitVariables::initializeVariables()
{

  if(m_tree) {
    m_tree->Branch("Digits_TGC",    &m_TGC_nDigits, "Digits_TGC_n/i");
    m_tree->Branch("Digits_TGC_stationName", &m_TGC_dig_stationName);
    m_tree->Branch("Digits_TGC_stationEta",  &m_TGC_dig_stationEta);
    m_tree->Branch("Digits_TGC_stationPhi",  &m_TGC_dig_stationPhi);
    m_tree->Branch("Digits_TGC_gas_gap",     &m_TGC_dig_gas_gap);
    m_tree->Branch("Digits_TGC_channel",     &m_TGC_dig_channel);
    m_tree->Branch("Digits_TGC_isStrip",     &m_TGC_dig_isStrip);
  }

  return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called on finalize level of main alg */
void TGCDigitVariables::deleteVariables()
{
  return;
}
