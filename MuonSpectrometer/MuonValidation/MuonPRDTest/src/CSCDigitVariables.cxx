/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCDigitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonSimData/CscSimDataCollection.h"

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

  // get truth information container of digitization
  const CscSimDataCollection* csc_SdoContainer = nullptr;
  CHECK( m_evtStore->retrieve(csc_SdoContainer, "CSC_SDO") );

  // get digit container (a container corresponds to a multilayer of a module)
  const CscDigitContainer *csc_DigitContainer = nullptr;
  CHECK( m_evtStore->retrieve(csc_DigitContainer, m_ContainerName.c_str()) );

  if(csc_DigitContainer->size()==0) ATH_MSG_WARNING(" CSC DigitContainer empty ");

  // iteration on all containers, i.e. all multilayers of all modules
  for(auto it : *csc_DigitContainer) {
    // a digit collection is instanciated for each container, i.e. holds all digits of a multilayer
    const CscDigitCollection* coll = it;

    // loop on all digits inside a collection, i.e. multilayer
    for (unsigned int item=0; item<coll->size(); item++) {

      // get specific digit and identify it
      const CscDigit* digit = coll->at(item);
      Identifier Id = digit->identify();

      std::string stName   = m_CscIdHelper->stationNameString(m_CscIdHelper->stationName(Id));
      int stationEta       = m_CscIdHelper->stationEta(Id);
      int stationPhi       = m_CscIdHelper->stationPhi(Id);
      int chlayer          = m_CscIdHelper->chamberLayer(Id);
      int wlayer           = m_CscIdHelper->wireLayer(Id);
      int gas_gap          = m_CscIdHelper->gasGap(Id);
      // channel here seems to be sth like the channel/strip closest to the initial Geant4 hit
      // that is entering the digitzation process (probably only of limited use)
      int channel          = m_CscIdHelper->strip(Id);
    
      ATH_MSG_DEBUG(     "MicroMegas Digit Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " ChamberLayer  ["   << chlayer       << "]"
                      << " wireLayer  ["   << wlayer       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " ChNr ["         << channel         << "]" );

      // module details down to the level of channel which is closest to the Geant4 hit
      // to be stored in the ntuple
      m_CSC_dig_stationName->push_back(stName);
      m_CSC_dig_stationEta->push_back(stationEta);
      m_CSC_dig_stationPhi->push_back(stationPhi);
      m_CSC_dig_chlayer->push_back(chlayer);
      m_CSC_dig_wlayer->push_back(wlayer);
      m_CSC_dig_gas_gap->push_back(gas_gap);
      m_CSC_dig_channel->push_back(channel);

      const MuonGM::CscReadoutElement* rdoEl = MuonDetMgr->getCscReadoutElement(Id);
      if (!rdoEl) throw std::runtime_error(Form("File: %s, Line: %d\nCSCDigitVariables::fillVariables() - Failed to retrieve CscReadoutElement for %s", __FILE__, __LINE__, m_CscIdHelper->print_to_string(Id).c_str()));

      // retrieve the MC truth associated with the digit (means the Geant4 hit information)
      if (csc_SdoContainer) {
        // search the truth container with the Id of the digit
        const CscSimData csc_sdo = (csc_SdoContainer->find(Id))->second;
        std::vector<CscSimData::Deposit> deposits;
        // get the truth deposits
        csc_sdo.deposits(deposits);

        // use the information of the first deposit
        int    truth_barcode   = deposits[0].first.barcode();
        double truth_localPosX = deposits[0].second.zpos();
        double truth_localPosY = deposits[0].second.ypos();
        double truth_charge    = deposits[0].second.charge();

        // get global and local coordinates of this first deposit
        Amg::Vector2D hit_on_surface(truth_localPosX, truth_localPosY);
        Amg::Vector3D hit_gpos(0., 0., 0.);
        rdoEl->surface(Id).localToGlobal(hit_on_surface, Amg::Vector3D(0., 0., 0.), hit_gpos);

        ATH_MSG_DEBUG("CSC Digit, truth barcode=" << truth_barcode);
        ATH_MSG_DEBUG("CSC Digit, truth localPosX=" << std::setw(9) << std::setprecision(2) << truth_localPosX
                      << ", truth localPosY=" << std::setw(9) << std::setprecision(2) << truth_localPosY
                      << ", truth charge=" << std::setw(8) << std::setprecision(5) << truth_charge);

        // truth information like positions and barcode
        // to be stored in the ntuple
        m_CSC_dig_truth_barcode->push_back( truth_barcode );
        m_CSC_dig_truth_localPosX->push_back( truth_localPosX );
        m_CSC_dig_truth_localPosY->push_back( truth_localPosY );

        m_CSC_dig_truth_globalPosX->push_back( hit_gpos[0] );
        m_CSC_dig_truth_globalPosY->push_back( hit_gpos[1] );
        m_CSC_dig_truth_globalPosZ->push_back( hit_gpos[2] );

        m_CSC_dig_truth_charge->push_back( truth_charge );

      }

      // digit counter for the ntuple
      m_CSC_nDigits++;
    }
  }

  ATH_MSG_DEBUG("processed " << m_CSC_nDigits << " MicroMegas hits");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode CSCDigitVariables::clearVariables()
{
  m_CSC_nDigits = 0;

  // information of the module down to the channel closest to the initial G4 hit
  // size of vector is m_CSC_nDigits
  m_CSC_dig_stationName->clear();
  m_CSC_dig_stationEta->clear();
  m_CSC_dig_stationPhi->clear();
  m_CSC_dig_chlayer->clear();
  m_CSC_dig_wlayer->clear();
  m_CSC_dig_gas_gap->clear();
  m_CSC_dig_channel->clear();

  // truth information of the (1st) Geant4 hit that caused this
  // digit to be recorded (size is m_CSC_nDigits)
  m_CSC_dig_truth_barcode->clear();
  m_CSC_dig_truth_localPosX->clear();
  m_CSC_dig_truth_localPosY->clear();
  m_CSC_dig_truth_globalPosX->clear();
  m_CSC_dig_truth_globalPosY->clear();
  m_CSC_dig_truth_globalPosZ->clear();
  m_CSC_dig_truth_charge->clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode CSCDigitVariables::initializeVariables()
{
  m_CSC_nDigits = 0;
  m_CSC_dig_stationName = new std::vector<std::string>;
  m_CSC_dig_stationEta  = new std::vector<int>();
  m_CSC_dig_stationPhi  = new std::vector<int>();
  m_CSC_dig_chlayer   = new std::vector<int>();
  m_CSC_dig_gas_gap     = new std::vector<int>();
  m_CSC_dig_channel     = new std::vector<int>();

  m_CSC_dig_truth_barcode    = new std::vector<int>;
  m_CSC_dig_truth_localPosX  = new std::vector<double>;
  m_CSC_dig_truth_localPosY  = new std::vector<double>;
  m_CSC_dig_truth_globalPosX = new std::vector<double>;
  m_CSC_dig_truth_globalPosY = new std::vector<double>;
  m_CSC_dig_truth_globalPosZ = new std::vector<double>;
  m_CSC_dig_truth_charge = new std::vector<double>;

  if(m_tree) {
    m_tree->Branch("Digits_CSC",    &m_CSC_nDigits, "Digits_CSC_n/i");
    m_tree->Branch("Digits_CSC_stationName", &m_CSC_dig_stationName);
    m_tree->Branch("Digits_CSC_stationEta",  &m_CSC_dig_stationEta);
    m_tree->Branch("Digits_CSC_stationPhi",  &m_CSC_dig_stationPhi);
    m_tree->Branch("Digits_CSC_chamberLayer",   &m_CSC_dig_chlayer);
    m_tree->Branch("Digits_CSC_wireLayer",   &m_CSC_dig_wlayer);
    m_tree->Branch("Digits_CSC_gas_gap",     &m_CSC_dig_gas_gap);
    m_tree->Branch("Digits_CSC_channel",     &m_CSC_dig_channel);

    m_tree->Branch("Digits_CSC_truth_barcode",    &m_CSC_dig_truth_barcode);
    m_tree->Branch("Digits_CSC_truth_localPosX",  &m_CSC_dig_truth_localPosX);
    m_tree->Branch("Digits_CSC_truth_localPosY",  &m_CSC_dig_truth_localPosY);
    m_tree->Branch("Digits_CSC_truth_globalPosX", &m_CSC_dig_truth_globalPosX);
    m_tree->Branch("Digits_CSC_truth_globalPosY", &m_CSC_dig_truth_globalPosY);
    m_tree->Branch("Digits_CSC_truth_globalPosZ", &m_CSC_dig_truth_globalPosZ);
    m_tree->Branch("Digits_CSC_truth_charge", &m_CSC_dig_truth_charge);

  }

  return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called on finalize level of main alg */
void CSCDigitVariables::deleteVariables()
{
  delete m_CSC_dig_stationName;
  delete m_CSC_dig_stationEta;
  delete m_CSC_dig_stationPhi;
  delete m_CSC_dig_chlayer;
  delete m_CSC_dig_wlayer;
  delete m_CSC_dig_gas_gap;
  delete m_CSC_dig_channel;

  delete m_CSC_dig_truth_barcode;
  delete m_CSC_dig_truth_localPosX;
  delete m_CSC_dig_truth_localPosY;
  delete m_CSC_dig_truth_globalPosX;
  delete m_CSC_dig_truth_globalPosY;
  delete m_CSC_dig_truth_globalPosZ;
  delete m_CSC_dig_truth_charge;

  m_CSC_nDigits = 0;
  m_CSC_dig_stationName = nullptr;
  m_CSC_dig_stationEta = nullptr;
  m_CSC_dig_stationPhi = nullptr;
  m_CSC_dig_chlayer = nullptr;
  m_CSC_dig_wlayer = nullptr;
  m_CSC_dig_gas_gap = nullptr;
  m_CSC_dig_channel = nullptr;
  
  m_CSC_dig_truth_barcode    = nullptr;
  m_CSC_dig_truth_localPosX  = nullptr;
  m_CSC_dig_truth_localPosY  = nullptr;
  m_CSC_dig_truth_globalPosX = nullptr;
  m_CSC_dig_truth_globalPosY = nullptr;
  m_CSC_dig_truth_globalPosZ = nullptr;
  m_CSC_dig_truth_charge = nullptr;

  return;
}
