/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MMDigitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonDigitContainer/MmDigit.h"

#include "MuonReadoutGeometry/MMReadoutElement.h"

#include "TTree.h"
#include <TString.h> // for Form

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode MMDigitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG("do fillNSWMMDigitVariables()");

  // clear variables
  CHECK( this->clearVariables() );

  // get digit container (a container corresponds to a multilayer of a module)
  const MmDigitContainer *nsw_MmDigitContainer = nullptr;
  CHECK( m_evtStore->retrieve(nsw_MmDigitContainer, m_ContainerName.c_str()) );

  if(nsw_MmDigitContainer->size()==0) ATH_MSG_WARNING(" MM DigitContainer empty ");

  // iteration on all containers, i.e. all multilayers of all modules
  for(auto it : *nsw_MmDigitContainer) {
    // a digit collection is instanciated for each container, i.e. holds all digits of a multilayer
    const MmDigitCollection* coll = it;

    // loop on all digits inside a collection, i.e. multilayer
    for (unsigned int item=0; item<coll->size(); item++) {

      // get specific digit and identify it
      const MmDigit* digit = coll->at(item);
      Identifier Id = digit->identify();

      std::string stName   = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(Id));
      int stationEta       = m_MmIdHelper->stationEta(Id);
      int stationPhi       = m_MmIdHelper->stationPhi(Id);
      int multiplet        = m_MmIdHelper->multilayer(Id);
      int gas_gap          = m_MmIdHelper->gasGap(Id);
      // channel here seems to be sth like the channel/strip closest to the initial Geant4 hit
      // that is entering the digitzation process (probably only of limited use)
      int channel          = m_MmIdHelper->channel(Id);
    
      ATH_MSG_DEBUG(     "MicroMegas Digit Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " Multiplet  ["   << multiplet       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " ChNr ["         << channel         << "]" );

      // module details down to the level of channel which is closest to the Geant4 hit
      // to be stored in the ntuple
      m_NSWMM_dig_stationName->push_back(stName);
      m_NSWMM_dig_stationEta->push_back(stationEta);
      m_NSWMM_dig_stationPhi->push_back(stationPhi);
      m_NSWMM_dig_multiplet->push_back(multiplet);
      m_NSWMM_dig_gas_gap->push_back(gas_gap);
      m_NSWMM_dig_channel->push_back(channel);

      // get the readout element class where the digit is recorded
      int isSmall = (stName[2] == 'S');
      const MuonGM::MMReadoutElement* rdoEl = MuonDetMgr->getMMRElement_fromIdFields(isSmall, stationEta, stationPhi, multiplet);
      if (!rdoEl) throw std::runtime_error(Form("File: %s, Line: %d\nMMDigitVariables::fillVariables() - Failed to retrieve MMReadoutElement for isSmall=%d, stationEta=%d, stationPhi=%d, multiplet=%d", __FILE__, __LINE__, isSmall, stationEta, stationPhi, multiplet));

      // information from VMM chip
      std::vector<float>  time          = digit->chipResponseTime();
      std::vector<float>  charge        = digit->chipResponseCharge();
      std::vector<int>    stripPosition = digit->chipResponsePosition();

      std::vector<double> localPosX;
      std::vector<double> localPosY;
      std::vector<double> globalPosX;
      std::vector<double> globalPosY;
      std::vector<double> globalPosZ;

      // information from strip
      std::vector<float>  sr_time          = digit->stripResponseTime();
      std::vector<float>  sr_charge        = digit->stripResponseCharge();
      std::vector<int>    sr_stripPosition = digit->stripResponsePosition();

      std::vector<double> sr_localPosX;
      std::vector<double> sr_localPosY;
      std::vector<double> sr_globalPosX;
      std::vector<double> sr_globalPosY;
      std::vector<double> sr_globalPosZ;

      // information for trigger
      std::vector<float>  time_trigger          = digit->stripTimeForTrigger();
      std::vector<float>  charge_trigger        = digit->stripChargeForTrigger();
      std::vector<int>    position_trigger      = digit->stripPositionForTrigger();
      std::vector<int>    MMFE_VMM_id_trigger   = digit->MMFE_VMM_idForTrigger();
      std::vector<int>    VMM_id_trigger        = digit->VMM_idForTrigger();

      // check if VMM chip and stirp agree
      if ( stripPosition.size() != sr_stripPosition.size() )
        ATH_MSG_DEBUG("MicroMegas digitization: number of strip out from the strip response different from that out from the chip response");

      bool isValid;
      int stationName  = m_MmIdHelper->stationName(Id);

      // Geant4 hit converted into digit might have fired more than one strip:
      // iterating on all strips associated to that digit (from chip response)
      for (unsigned int i=0;i<stripPosition.size();i++) {
        // take strip index form chip information
        int cr_strip = stripPosition.at(i);

        localPosX.push_back(0.);
        localPosY.push_back(0.);
        globalPosX.push_back(0.);
        globalPosY.push_back(0.);
        globalPosZ.push_back(0.);

        //retrieve the detailed identifier for the strip form the chip response
        Identifier cr_id = m_MmIdHelper->channelID(stationName, stationEta, stationPhi, multiplet, gas_gap, cr_strip, true, &isValid);
        if (!isValid) {
          ATH_MSG_WARNING("MMDigitVariables: failed to create a valid ID for (chip response) strip n. " << cr_strip
                       << "; associated positions will be set to 0.0.");
        } else {
          // asking the detector element to get local position of strip
          Amg::Vector2D cr_strip_pos(0., 0.);
          if ( !rdoEl->stripPosition(cr_id,cr_strip_pos) ) {
            ATH_MSG_WARNING("MMDigitVariables: failed to associate a valid local position for (chip response) strip n. " << cr_strip
                         << "; associated positions will be set to 0.0.");
          } else {
            localPosX.at(i) = cr_strip_pos.x();
            localPosY.at(i) = cr_strip_pos.y();
          }

          // asking the detector element to transform this local to the global position
          Amg::Vector3D cr_strip_gpos(0., 0., 0.);
          rdoEl->surface(cr_id).localToGlobal(cr_strip_pos, Amg::Vector3D(0., 0., 0.), cr_strip_gpos);
          globalPosX.at(i) = cr_strip_gpos[0];
          globalPosY.at(i) = cr_strip_gpos[1];
          globalPosZ.at(i) = cr_strip_gpos[2];

          // check if local and global position are congruent with the transform
          Amg::Vector3D lpos = rdoEl->transform(cr_id).inverse() * cr_strip_gpos;
          double dx = cr_strip_pos.x() - lpos.x();
          double dy = cr_strip_pos.y() - lpos.y();

          if ( fabs(dx)>0.1 || fabs(dy)>0.1 ) {
            ATH_MSG_WARNING("MicroMegas digitization: inconsistency between local/global position and transform:");
            ATH_MSG_WARNING("                         X from stripPosition: " << std::setw(10) << std::setprecision(3) << cr_strip_pos.x()
                         << ", X from global*transform(id).inverse(): " << lpos.x() );
            ATH_MSG_WARNING("                         Y from stripPosition: " << std::setw(10) << std::setprecision(3) << cr_strip_pos.y()
                         << ", Y from global*transform(Id).inverse(): " << lpos.y() );
          }
       }
     }

      // Geant4 hit converted into digit might have fired more than one strip:
      // iterating on all strips associated to that digit (from strip response)
     for (unsigned int i=0;i<sr_stripPosition.size();i++) {
        int sr_strip = sr_stripPosition.at(i);

        sr_localPosX.push_back(0.);
        sr_localPosY.push_back(0.);
        sr_globalPosX.push_back(0.);
        sr_globalPosY.push_back(0.);
        sr_globalPosZ.push_back(0.);

        //retrieve the detailed identifier for the strip form the strip response
        Identifier sr_id = m_MmIdHelper->channelID(stationName, stationEta, stationPhi, multiplet, gas_gap, sr_strip, true, &isValid);
        if (!isValid) {
          ATH_MSG_WARNING("MMDigitVariables: failed to create a valid ID for (chip response) strip n. " << sr_strip
                       << "; associated positions will be set to 0.0.");
        } else {
          // asking the detector element to transform this local to the global position
          Amg::Vector2D sr_strip_pos(0., 0.);
          if ( !rdoEl->stripPosition(sr_id,sr_strip_pos) ) {
            ATH_MSG_WARNING("MMDigitVariables: failed to associate a valid local position for (chip response) strip n. " << sr_strip
                         << "; associated positions will be set to 0.0.");
          } else {
            sr_localPosX.at(i) = sr_strip_pos.x();
            sr_localPosY.at(i) = sr_strip_pos.y();
          }

          // asking the detector element to transform this local to the global position
          Amg::Vector3D sr_strip_gpos(0., 0., 0.);
          rdoEl->surface(sr_id).localToGlobal(sr_strip_pos, Amg::Vector3D(0., 0., 0.), sr_strip_gpos);
          sr_globalPosX.at(i) = sr_strip_gpos[0];
          sr_globalPosY.at(i) = sr_strip_gpos[1];
          sr_globalPosZ.at(i) = sr_strip_gpos[2];

          // check if local and global position are congruent with the transform
          Amg::Vector3D lpos = rdoEl->transform(sr_id).inverse() * sr_strip_gpos;
          double dx = sr_strip_pos.x() - lpos.x();
          double dy = sr_strip_pos.y() - lpos.y();

          if ( fabs(dx)>0.1 || fabs(dy)>0.1 ) {
            ATH_MSG_WARNING("MicroMegas digitization: inconsistency between local/global position and transform:");
            ATH_MSG_WARNING("                         X from Center(): " << std::setw(10) << std::setprecision(3) << sr_strip_pos.x()
                         << ", X from local*transform: " << lpos.x() );
            ATH_MSG_WARNING("                         Y from Center(): " << std::setw(10) << std::setprecision(3) << sr_strip_pos.y()
                         << ", Y from local*transform: " << lpos.y() );
          }

        }
      }

      unsigned int cl = (time.size()<=sr_time.size()) ? time.size() : sr_time.size();

      for (unsigned int i=0;i<cl;i++) {
        ATH_MSG_DEBUG("MicroMegas Digit, chip response time    :" << std::setw(6) << std::setprecision(2) << time.at(i)         
                                 << ",  strip response time    :" << std::setw(6) << std::setprecision(2) << sr_time.at(i));
        ATH_MSG_DEBUG("MicroMegas Digit, chip response charge  :" << std::setw(6) << std::setprecision(2) << charge.at(i)   
                                 << ",  strip response charge  :" << std::setw(6) << std::setprecision(2) << sr_charge.at(i));
        ATH_MSG_DEBUG("MicroMegas Digit, chip response position:" << std::setw(6) << std::setprecision(2) << stripPosition.at(i)
                                 << ",  strip response position:" << std::setw(6) << std::setprecision(2) << sr_stripPosition.at(i));
      }

      // local/global positions (+charge/time/strip index) of the strips from chip response 
      // to be stored in the ntuple
      m_NSWMM_dig_time->push_back(time);
      m_NSWMM_dig_charge->push_back(charge);
      m_NSWMM_dig_stripPosition->push_back(stripPosition);
      m_NSWMM_dig_stripLposX->push_back(localPosX);
      m_NSWMM_dig_stripLposY->push_back(localPosY);
      m_NSWMM_dig_stripGposX->push_back(globalPosX);
      m_NSWMM_dig_stripGposY->push_back(globalPosY);
      m_NSWMM_dig_stripGposZ->push_back(globalPosZ);

      // local/global positions (+charge/time/strip index) of the strips from strip response 
      // to be stored in the ntuple
      m_NSWMM_dig_sr_time->push_back(sr_time);
      m_NSWMM_dig_sr_charge->push_back(sr_charge);
      m_NSWMM_dig_sr_stripPosition->push_back(sr_stripPosition);
      m_NSWMM_dig_sr_stripLposX->push_back(sr_localPosX);
      m_NSWMM_dig_sr_stripLposY->push_back(sr_localPosY);
      m_NSWMM_dig_sr_stripGposX->push_back(sr_globalPosX);
      m_NSWMM_dig_sr_stripGposY->push_back(sr_globalPosY);
      m_NSWMM_dig_sr_stripGposZ->push_back(sr_globalPosZ);

      // some more information of the digit to be stored in the ntuple
      m_NSWMM_dig_time_trigger->push_back(time_trigger);
      m_NSWMM_dig_charge_trigger->push_back(charge_trigger);
      m_NSWMM_dig_position_trigger->push_back(position_trigger);
      m_NSWMM_dig_MMFE_VMM_id_trigger->push_back(MMFE_VMM_id_trigger);
      m_NSWMM_dig_VMM_id_trigger->push_back(VMM_id_trigger);

      // digit counter for the ntuple
      m_NSWMM_nDigits++;
    }
  }

  ATH_MSG_DEBUG("processed " << m_NSWMM_nDigits << " MicroMegas hits");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode MMDigitVariables::clearVariables()
{
  m_NSWMM_nDigits = 0;

  // information of the module down to the channel closest to the initial G4 hit
  // size of vector is m_NSWMM_nDigits
  m_NSWMM_dig_stationName->clear();
  m_NSWMM_dig_stationEta->clear();
  m_NSWMM_dig_stationPhi->clear();
  m_NSWMM_dig_multiplet->clear();
  m_NSWMM_dig_gas_gap->clear();
  m_NSWMM_dig_channel->clear();

  // vectors of size m_NSWMM_nDigits that hold vectors in which an entry
  // corresponds to a strip that was decided to be fired by the digit
  // (information from VMM chip response emulation)
  m_NSWMM_dig_time->clear();
  m_NSWMM_dig_charge->clear();
  m_NSWMM_dig_stripPosition->clear();
  m_NSWMM_dig_stripLposX->clear();
  m_NSWMM_dig_stripLposY->clear();
  m_NSWMM_dig_stripGposX->clear();
  m_NSWMM_dig_stripGposY->clear();
  m_NSWMM_dig_stripGposZ->clear();

  // vectors of size m_NSWMM_nDigits that hold vectors in which an entry
  // corresponds to a strip that was decided to be fired by the digit
  // (information from VMM chip strip emulation)
  m_NSWMM_dig_sr_time->clear();
  m_NSWMM_dig_sr_charge->clear();
  m_NSWMM_dig_sr_stripPosition->clear();
  m_NSWMM_dig_sr_stripLposX->clear();
  m_NSWMM_dig_sr_stripLposY->clear();
  m_NSWMM_dig_sr_stripGposX->clear();
  m_NSWMM_dig_sr_stripGposY->clear();
  m_NSWMM_dig_sr_stripGposZ->clear();

  // more information for trigger
  m_NSWMM_dig_time_trigger->clear();
  m_NSWMM_dig_charge_trigger->clear();
  m_NSWMM_dig_position_trigger->clear();
  m_NSWMM_dig_MMFE_VMM_id_trigger->clear();
  m_NSWMM_dig_VMM_id_trigger->clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode MMDigitVariables::initializeVariables()
{
  m_NSWMM_nDigits = 0;
  m_NSWMM_dig_stationName = new std::vector<std::string>();
  m_NSWMM_dig_stationEta  = new std::vector<int>();
  m_NSWMM_dig_stationPhi  = new std::vector<int>();
  m_NSWMM_dig_multiplet   = new std::vector<int>();
  m_NSWMM_dig_gas_gap     = new std::vector<int>();
  m_NSWMM_dig_channel     = new std::vector<int>();

  m_NSWMM_dig_time          = new std::vector< std::vector<float> >;
  m_NSWMM_dig_charge        = new std::vector< std::vector<float> >;
  m_NSWMM_dig_stripPosition = new std::vector< std::vector<int> >;
  m_NSWMM_dig_stripLposX    = new std::vector< std::vector<double> >;
  m_NSWMM_dig_stripLposY    = new std::vector< std::vector<double> >;
  m_NSWMM_dig_stripGposX    = new std::vector< std::vector<double> >;
  m_NSWMM_dig_stripGposY    = new std::vector< std::vector<double> >;
  m_NSWMM_dig_stripGposZ    = new std::vector< std::vector<double> >;

  m_NSWMM_dig_sr_time          = new std::vector< std::vector<float> >;
  m_NSWMM_dig_sr_charge        = new std::vector< std::vector<float> >;
  m_NSWMM_dig_sr_stripPosition = new std::vector< std::vector<int> >;
  m_NSWMM_dig_sr_stripLposX    = new std::vector< std::vector<double> >;
  m_NSWMM_dig_sr_stripLposY    = new std::vector< std::vector<double> >;
  m_NSWMM_dig_sr_stripGposX    = new std::vector< std::vector<double> >;
  m_NSWMM_dig_sr_stripGposY    = new std::vector< std::vector<double> >;
  m_NSWMM_dig_sr_stripGposZ    = new std::vector< std::vector<double> >;

  m_NSWMM_dig_time_trigger        = new std::vector< std::vector<float> >;
  m_NSWMM_dig_charge_trigger      = new std::vector< std::vector<float> >;
  m_NSWMM_dig_position_trigger    = new std::vector< std::vector<int> >;
  m_NSWMM_dig_MMFE_VMM_id_trigger = new std::vector< std::vector<int> >;
  m_NSWMM_dig_VMM_id_trigger      = new std::vector< std::vector<int> >;

  if(m_tree) {
    m_tree->Branch("Digits_MM",    &m_NSWMM_nDigits, "Digits_MM_n/i");
    m_tree->Branch("Digits_MM_stationName", &m_NSWMM_dig_stationName);
    m_tree->Branch("Digits_MM_stationEta",  &m_NSWMM_dig_stationEta);
    m_tree->Branch("Digits_MM_stationPhi",  &m_NSWMM_dig_stationPhi);
    m_tree->Branch("Digits_MM_multiplet",   &m_NSWMM_dig_multiplet);
    m_tree->Branch("Digits_MM_gas_gap",     &m_NSWMM_dig_gas_gap);
    m_tree->Branch("Digits_MM_channel",     &m_NSWMM_dig_channel);

    m_tree->Branch("Digits_MM_time",          &m_NSWMM_dig_time);
    m_tree->Branch("Digits_MM_charge",        &m_NSWMM_dig_charge);
    m_tree->Branch("Digits_MM_stripPosition", &m_NSWMM_dig_stripPosition);
    m_tree->Branch("Digits_MM_stripLposX",    &m_NSWMM_dig_stripLposX);
    m_tree->Branch("Digits_MM_stripLposY",    &m_NSWMM_dig_stripLposY);
    m_tree->Branch("Digits_MM_stripGposX",    &m_NSWMM_dig_stripGposX);
    m_tree->Branch("Digits_MM_stripGposY",    &m_NSWMM_dig_stripGposY);
    m_tree->Branch("Digits_MM_stripGposZ",    &m_NSWMM_dig_stripGposZ);

    m_tree->Branch("Digits_MM_stripResponse_time",          &m_NSWMM_dig_sr_time);
    m_tree->Branch("Digits_MM_stripResponse_charge",        &m_NSWMM_dig_sr_charge);
    m_tree->Branch("Digits_MM_stripResponse_stripPosition", &m_NSWMM_dig_sr_stripPosition);
    m_tree->Branch("Digits_MM_stripResponse_stripLposX",    &m_NSWMM_dig_sr_stripLposX);
    m_tree->Branch("Digits_MM_stripResponse_stripLposY",    &m_NSWMM_dig_sr_stripLposY);
    m_tree->Branch("Digits_MM_stripresponse_stripGposX",    &m_NSWMM_dig_sr_stripGposX);
    m_tree->Branch("Digits_MM_stripResponse_stripGposY",    &m_NSWMM_dig_sr_stripGposY);
    m_tree->Branch("Digits_MM_stripResponse_stripGposZ",    &m_NSWMM_dig_sr_stripGposZ);
      
    m_tree->Branch("Digits_MM_time_trigger",          &m_NSWMM_dig_time_trigger);
    m_tree->Branch("Digits_MM_charge_trigger",        &m_NSWMM_dig_charge_trigger);
    m_tree->Branch("Digits_MM_position_trigger",      &m_NSWMM_dig_position_trigger);
    m_tree->Branch("Digits_MM_MMFE_VMM_id_trigger",   &m_NSWMM_dig_MMFE_VMM_id_trigger);
    m_tree->Branch("Digits_MM_VMM_id_trigger",        &m_NSWMM_dig_VMM_id_trigger);
  }

  return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called on finalize level of main alg */
void MMDigitVariables::deleteVariables()
{
  delete m_NSWMM_dig_stationName;
  delete m_NSWMM_dig_stationEta;
  delete m_NSWMM_dig_stationPhi;
  delete m_NSWMM_dig_multiplet;
  delete m_NSWMM_dig_gas_gap;
  delete m_NSWMM_dig_channel;
  
  delete m_NSWMM_dig_time;
  delete m_NSWMM_dig_charge;
  delete m_NSWMM_dig_stripPosition;
  delete m_NSWMM_dig_stripLposX;
  delete m_NSWMM_dig_stripLposY;
  delete m_NSWMM_dig_stripGposX;
  delete m_NSWMM_dig_stripGposY;
  delete m_NSWMM_dig_stripGposZ;

  delete m_NSWMM_dig_sr_time;
  delete m_NSWMM_dig_sr_charge;
  delete m_NSWMM_dig_sr_stripPosition;
  delete m_NSWMM_dig_sr_stripLposX;
  delete m_NSWMM_dig_sr_stripLposY;
  delete m_NSWMM_dig_sr_stripGposX;
  delete m_NSWMM_dig_sr_stripGposY;
  delete m_NSWMM_dig_sr_stripGposZ;

  delete m_NSWMM_dig_time_trigger;
  delete m_NSWMM_dig_charge_trigger;
  delete m_NSWMM_dig_position_trigger;
  delete m_NSWMM_dig_MMFE_VMM_id_trigger;
  delete m_NSWMM_dig_VMM_id_trigger;

  m_NSWMM_nDigits = 0;
  m_NSWMM_dig_stationName = nullptr;
  m_NSWMM_dig_stationEta = nullptr;
  m_NSWMM_dig_stationPhi = nullptr;
  m_NSWMM_dig_multiplet = nullptr;
  m_NSWMM_dig_gas_gap = nullptr;
  m_NSWMM_dig_channel = nullptr;
  
  m_NSWMM_dig_time = nullptr;
  m_NSWMM_dig_charge = nullptr;
  m_NSWMM_dig_stripPosition = nullptr;
  m_NSWMM_dig_stripLposX = nullptr;
  m_NSWMM_dig_stripLposY = nullptr;
  m_NSWMM_dig_stripGposX = nullptr;
  m_NSWMM_dig_stripGposY = nullptr;
  m_NSWMM_dig_stripGposZ = nullptr;

  m_NSWMM_dig_sr_time = nullptr;
  m_NSWMM_dig_sr_charge = nullptr;
  m_NSWMM_dig_sr_stripPosition = nullptr;
  m_NSWMM_dig_sr_stripLposX = nullptr;
  m_NSWMM_dig_sr_stripLposY = nullptr;
  m_NSWMM_dig_sr_stripGposX = nullptr;
  m_NSWMM_dig_sr_stripGposY = nullptr;
  m_NSWMM_dig_sr_stripGposZ = nullptr;

  m_NSWMM_dig_time_trigger = nullptr;
  m_NSWMM_dig_charge_trigger = nullptr;
  m_NSWMM_dig_position_trigger = nullptr;
  m_NSWMM_dig_MMFE_VMM_id_trigger = nullptr;
  m_NSWMM_dig_VMM_id_trigger = nullptr;

  return;
}
