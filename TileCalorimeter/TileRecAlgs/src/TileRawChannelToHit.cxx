/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileRawChannelToHit.cxx
//  Author   : Alexander Solodkov
//  Created  : Feb, 2005
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// access all RawChannels inside container
#include "EventContainers/SelectAllObject.h" 

// Tile includes
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileFragHash.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileInfo.h"
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"
#include "TileEvent/TileRawChannel.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecAlgs/TileRawChannelToHit.h"


//
// Constructor
//
TileRawChannelToHit::TileRawChannelToHit(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_tileInfo(0)
  , m_tileToolEmscale("TileCondToolEmscale")
{
  declareProperty("TileRawChannelContainer", m_rawChannelContainer = "TileRawChannelCnt");  // Raw chan to convert.
  declareProperty("TileHitContainer", m_hitContainer = "TileHitVec");                       // Name of output container
  declareProperty("TileInfoName", m_infoName = "TileInfo");                                 // name of TileInfo store
  declareProperty("UseSamplFract", m_useSamplFract = false);  // if true energy in TileHit is the as it is coming from G4 simulation
                                                              // and by default it is equal to final - TileCell energy
  declareProperty("TileCondToolEmscale"    , m_tileToolEmscale);
}

TileRawChannelToHit::~TileRawChannelToHit()
{}

//****************************************************************************
//* Initialization
//****************************************************************************

StatusCode TileRawChannelToHit::initialize() {

  if (m_useSamplFract)
    ATH_MSG_INFO( "Sampling fraction is taken into account for TileHit energy" );
  else
    ATH_MSG_INFO( "TileHit will contain CELL energy (not divided by sampling fraction)" );

  // retrieve TileID helper and TileIfno from det store
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );

  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );

  ATH_MSG_INFO( "TileRawChannelToHit initialization completed" );

  return StatusCode::SUCCESS;
}


//****************************************************************************
//* Execution
//****************************************************************************

StatusCode TileRawChannelToHit::execute() {

  // disable checks for TileID and remember previous state
  bool do_checks = m_tileID->do_checks();
  m_tileID->set_do_checks(false);

  /* zero all counters and sums */
  int nHit = 0;
  int nChan = 0;
  float eCh = 0.0;
  float eHitTot = 0.0;

  TileHitVector* hits = new TileHitVector;

  //**
  //* Get TileRawChannels
  //**
  const TileRawChannelContainer* rawChannels;

  if (evtStore()->retrieve(rawChannels, m_rawChannelContainer).isFailure()) {
    ATH_MSG_WARNING( " Could not find container " << m_rawChannelContainer );
    ATH_MSG_WARNING( " creating empty TileHitVector container " );

  } else {

    TileRawChannelUnit::UNIT rChUnit = rawChannels->get_unit();
    //TileFragHash::TYPE rChType = rawChannels->get_type();

    // iterate over all collections in a container
    TileRawChannelContainer::const_iterator collItr = rawChannels->begin();
    TileRawChannelContainer::const_iterator lastColl = rawChannels->end();

    for (; collItr != lastColl; ++collItr) {

      HWIdentifier drawer_id = m_tileHWID->drawer_id((*collItr)->identify());
      int ros = m_tileHWID->ros(drawer_id);
      int drawer = m_tileHWID->drawer(drawer_id);
      int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

      bool is_calibration = ((*collItr)->size() == 96);
      if (is_calibration)
        ATH_MSG_DEBUG( "Calibration mode, ignore high gain" );

      // iterate over all raw channels in a collection, creating new TileHits
      // Add each new TileHit to the TileHitContainer.

      TileRawChannelCollection::const_iterator rawItr = (*collItr)->begin();
      TileRawChannelCollection::const_iterator end = (*collItr)->end();

      for (; rawItr != end; ++rawItr) {

        const TileRawChannel * pChannel = (*rawItr);
        HWIdentifier adc_id = pChannel->adc_HWID();
        int channel = m_tileHWID->channel(adc_id);
        int adc = m_tileHWID->adc(adc_id);

        // skip high gain in calibration mode
        if (is_calibration && TileHWID::HIGHGAIN == adc)
          continue;

        float amp = pChannel->amplitude();
        float time = pChannel->time();

        ++nChan;
        eCh += amp;

        Identifier pmt_id = pChannel->pmt_ID();
        if (pmt_id.is_valid()) {

          float ener = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc,
              amp, rChUnit, TileRawChannelUnit::MegaElectronVolts);

          if (m_useSamplFract) // divide by sampling fraction (about 40) 
            ener /= m_tileInfo->HitCalib(pmt_id);

          TileHit * pHit = new TileHit(pmt_id, ener, time);
          eHitTot += ener;
          ++nHit;

          ATH_MSG_VERBOSE( "TileRawChannelToHit: "
                          << " pmt_id=" << m_tileID->to_string(pmt_id, -1)
                          << " adc_id=" << m_tileHWID->to_string(adc_id)
                          << " nHit=" << nHit
                          << " amp="  << amp
                          << " ene=" << ener
                          << " time=" << time );

          hits->push_back(*pHit);
          delete pHit;

        } else {

          ATH_MSG_VERBOSE( "TileRawChannelToHit: "
                << " channel with adc_id=" << m_tileHWID->to_string(adc_id)
                << " is not connected" );
        }
      }
    }
  }
  

  /* Register the set of TileHits to the event store. */

  CHECK( evtStore()->record(hits, m_hitContainer, false ) );

  // Execution completed.
  ATH_MSG_DEBUG( "TileRawChannelToHit execution completed." );
  ATH_MSG_DEBUG( " nChan=" << nChan
                << " RawChSum=" << eCh
                << " nHit=" << nHit
                << " eneTot=" << eHitTot );


  ATH_MSG_VERBOSE( "TileHitVector container registered to the TES with name"
                  << m_hitContainer );

  m_tileID->set_do_checks(do_checks); // set back this flag to TileID

  return StatusCode::SUCCESS;
}

//************************************************************************


//****************************************************************************
//* Finalize
//****************************************************************************

StatusCode TileRawChannelToHit::finalize() {

  ATH_MSG_INFO( "TileRawChannelToHit::finalize() end" );

  return StatusCode::SUCCESS;
}
