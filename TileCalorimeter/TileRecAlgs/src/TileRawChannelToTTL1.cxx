/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileRawChannelToTTL1.cxx
//  Author   : Pedro Amaral, based on TileHitToTTL1
//  Created  : Feb, 2005
//
//  DESCRIPTION:
//      Created to simulate the Tile Level 1 Trigger Towers (TTL1), which are
//      hardware sums of the Tile channels, about 3 or 4 channels per tower.
//      The towers are read out in N time slices, with N=9 as a default.
//      Noise, pedestal and threshold counts are included, in units of mV.
//      Units into TTL1 are in mV.
//
//      Option to have variable phases for each trigger tower, of the sampling
//      of the TTL1 shape.
//      Default = option turned off, all phases set to zero.
//      to turn on, set TileConstantTTL1Shape = False, and 
//      phase info is then read from TileInfo.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
// small hack to be able to modify original TileRawChannel
#include "TileRecAlgs/TileRawChannelToTTL1.h"

#define private public
#include "TileEvent/TileRawChannel.h"
#undef private

#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileFragHash.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/TileBchPrbs.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileInfo.h"
#include "TileEvent/TileRawChannelContainer.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/errorcheck.h"

//CLHEP includes
#include <CLHEP/Random/Randomize.h>

using namespace CLHEP;

//C++ STL includes
#include <vector>

//
// Constructor
//
TileRawChannelToTTL1::TileRawChannelToTTL1(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_tileInfo(0)
  , m_TT_ID(0)
  , m_phase(0)
  , m_nSamp(0)
  , m_iTrig(0)
  , m_tileBadChanTool("TileBadChanTool")
  , m_tileToolEmscale("TileCondToolEmscale")
{
  declareProperty("TileInfoName", m_infoName = "TileInfo");
  declareProperty("TileConstantTTL1Shape", m_constantTTL1shape = true);
  declareProperty("maskBadChannels", m_maskBadChannels = false);
  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
}

TileRawChannelToTTL1::~TileRawChannelToTTL1() {
}

//
// Alg standard initialize function
//
StatusCode TileRawChannelToTTL1::initialize() {

  // retrieve CaloLVL1_ID, TileID, TileHWID helpers and TileIfno from det store

  CHECK( detStore()->retrieve(m_TT_ID) );
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );

  //=== get TileBadChanTool
  CHECK( m_tileBadChanTool.retrieve() );

  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );


  // Here get already TTL1 Shapes, so as not to perform this on every execute:
  m_nSamp = m_tileInfo->NdigitSamples(); // number of time slices for each chan
  m_iTrig = m_tileInfo->ItrigSample();   // index of the triggering time slice
  m_TTL1Shape.resize(m_nSamp, 0.);
  if (m_constantTTL1shape) {
    m_phase = 0.0;
    m_tileInfo->ttl1Shape(m_nSamp, m_iTrig, m_phase, m_TTL1Shape);
    for (int jsamp = 0; jsamp < m_nSamp; ++jsamp) {
      ATH_MSG_DEBUG( "jsamp=" << jsamp << " ttl1shape=" << m_TTL1Shape[jsamp] );
    }
  }

  ATH_CHECK( m_rawChannelContainerKey.initialize() );
  ATH_CHECK( m_ttl1ContainerKey.initialize() );

  ATH_MSG_INFO( "TileRawChannelToTTL1 initialization completed" );

  return StatusCode::SUCCESS;
}
/*==========================================================================*/
//
// Begin Execution Phase.
//
StatusCode TileRawChannelToTTL1::execute() {

  ATH_MSG_DEBUG( "Executing TileRawChannelToTTL1");

  /*......................................................*/
  // Step 2:  Get all global parameters that will be needed for processing.
  /* Get TileNoise flag from TileInfo (true => generate noise in TileDigits) */
  bool tileNoise = m_tileInfo->TileNoise();
  /* Get TileZeroSuppress flag from TileInfo
   (true => apply threshold to Digits) */
  bool tileThresh = m_tileInfo->TileZeroSuppress();
  // declare array for random number generation for noise in samples.
  double Rndm[16];      // Can't use variable size array
  // declare TTL1 parameters to be obtained from TileInfo
  float ttL1Calib, ttL1NoiseSigma, ttL1Ped, ttL1Thresh;

  ATH_MSG_DEBUG( "nSamp=" << m_nSamp
                << ", iTrig=" << m_iTrig
                << ", tileNoise=" << ((tileNoise) ? "true" : "false")
                << ", tileThresh=" << ((tileThresh) ? "true" : "false") );

  /*......................................................*/
  // step 3:  Get rawChannel container from TES and create TTL1 container
  /* Note that rawChannel container has 256 collections (one for each drawer),
   but TTL1 container has no collections and no structure. */
  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey);
  ATH_CHECK( rawChannelContainer.isValid() );

  TileRawChannelUnit::UNIT rChUnit = rawChannelContainer->get_unit();
  //TileFragHash::TYPE rChType = rawChannelContainer->get_type();

  SG::WriteHandle<TileTTL1Container> ttl1Container(m_ttl1ContainerKey);
  ATH_CHECK( ttl1Container.record(std::make_unique<TileTTL1Container>()) );
  ATH_MSG_DEBUG( "TileTTL1Container registered successfully (" << m_ttl1ContainerKey.key() << ")" );

  /*......................................................*/
  // Step 4: Create temporary arrays for processing signals.
  /* Create array for all TT amplitudes in a single drawer. */
  Identifier ttId[16];  // array of TT identifiers in a single drawer
  float ttAmp[16];      // array of all TT amplitudes in a single drawer
  bool ttRawChannel[16];      // array of TT occupancy in a single drawer
  int nTT;              // number of rawChannel towers in this drawer.
  int nRawChannel;      // number of rawChannels in this drawer.
  int nIgnore;          // number of ignored rawChannels in this drawer.
  int nTTTot = 0;         // total number of rawChannel towers.
  int nRawChannelTot = 0; // total number of rawChannels.
  int nIgnoreTot = 0;     // total number of ignored rawChannels.
  float ttAmpTot = 0;     // total energy in good level-1 towers.
  float ttAmpTotIg = 0.;  // total energy in "ignored" level-1 towers.
  int minieta, maxieta, posneg;

  /* Create array for the nSamp time-samples of a single tower. */
  std::vector<float> ttL1samples(m_nSamp);

  /*......................................................*/
  // Step 5: Begin loop over all collections (collection = electronic drawer). 
  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {

    HWIdentifier drawer_id = m_tileHWID->drawer_id(rawChannelCollection->identify());
    int ros = m_tileHWID->ros(drawer_id);
    int drawer = m_tileHWID->drawer(drawer_id);
    int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

    switch (ros) {
      case TileHWID::BARREL_POS:
        posneg = +1;
        minieta = 0;
        maxieta = 8;
        break;
      case TileHWID::BARREL_NEG:
        posneg = -1;
        minieta = 0;
        maxieta = 8;
        break;
      case TileHWID::EXTBAR_POS:
        posneg = +1;
        minieta = 9;
        maxieta = 14;
        break;
      case TileHWID::EXTBAR_NEG:
        posneg = -1;
        minieta = 9;
        maxieta = 14;
        break;
      default:
        posneg = minieta = maxieta = 0;
    }

    /* Zero temporary array of trigger tower amplitudes (TTL1amp) for this collection. */
    memset(ttAmp, 0, sizeof(ttAmp));
    memset(ttRawChannel, 0, sizeof(ttRawChannel));
    nTT = nIgnore = nRawChannel = 0;

    /*......................................................*/
    // Step 6: Iterate over all rawChannels in this collection, summing amps for each tower.
    TileRawChannelCollection::const_iterator rawChannelItr = rawChannelCollection->begin();
    TileRawChannelCollection::const_iterator lastRawChannel = rawChannelCollection->end();

    for (TileRawChannel* rawChannel : *rawChannelCollection) {

      /* Get rawChannel Identifier */
      HWIdentifier hwid = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(hwid);
      int adc = m_tileHWID->adc(hwid);

      // this is the hack for FDR-2
      // put zero amplitude in all bad channels 
      // so that zero amplitude will be written to the ByteStream 
      if (m_maskBadChannels) {
        TileRawChannel * pRch = static_cast<TileRawChannel *>(rawChannel);
        TileBchStatus status = m_tileBadChanTool->getAdcStatus(hwid);

        if (status.isBad()) {
          //=== check if we have VeryLargeHfNoise, this indicates hot channel overflow
          if (status.contains(TileBchPrbs::VeryLargeHfNoise)) {
            hwid = m_tileHWID->adc_id(m_tileHWID->channel_id(hwid), adc);
            pRch->m_adc_hwid = hwid;
            pRch->m_amplitude[0] = m_tileToolEmscale->channelCalib(drawerIdx,
                channel, adc, 1023., TileRawChannelUnit::ADCcounts, rChUnit);
            pRch->m_time[0] = 0.0;
            pRch->m_quality[0] = 15.;
            pRch->m_pedestal = 0.0;
          }
          //=== dead channel, put zero energy
          else {
            pRch->m_amplitude[0] = 0.0;
            pRch->m_time[0] = 0.0;
            pRch->m_quality[0] = 0.0;
            pRch->m_pedestal = 0.0;
          }
        }

        //=== not bad, but noisy channel
        else if (status.isNoisy()) { // noisy channel ...
          float noise = 0.0; // FIXME::add some noise - but don't know what to add
          pRch->m_amplitude[0] += m_tileToolEmscale->channelCalib(drawerIdx,
              channel, adc, noise, TileRawChannelUnit::ADCcounts, rChUnit);
        }

      }

      // note that amplitude() is in unknown units (can be even online MeV), convert it to MeV first
      float e = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc,
                                                rawChannel->amplitude(), 
                                                rChUnit, 
                                                TileRawChannelUnit::MegaElectronVolts);

      // convert MeV to pCb
      float q = e / m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, 1.0,
              TileRawChannelUnit::PicoCoulombs, TileRawChannelUnit::MegaElectronVolts);

      int ieta = 999;
      int iphi = 999;

      Identifier pmt_id = rawChannel->pmt_ID();
      if (pmt_id.is_valid() && m_tileID->section(pmt_id) < 4
          && m_tileID->section(pmt_id) > 0) {

        /* Get TT Identifier for this pmt */
        Identifier tt_id = rawChannel->tt_ID();
        /* Get eta-phi  indices of TTL1 for this channel. */
        ieta = m_TT_ID->eta(tt_id);
        iphi = m_TT_ID->phi(tt_id); // (same as module).
        if (iphi != drawer)
          ATH_MSG_ERROR( "drawer=" << drawer
                        << ", iphi=" << iphi
                        << "id=" << m_tileID->to_string(pmt_id) );

        if (ttRawChannel[ieta]) { // already exists - just add charge
          ttAmp[ieta] += q;
        } else { // rawChannel in new TT
          ttId[ieta] = tt_id;
          ttRawChannel[ieta] = true;
          ttAmp[ieta] = q;
          if (ieta >= minieta && ieta <= maxieta)
            ++nTT; // count only valid TT
        }
        ++nRawChannel;
        if (ieta < minieta || ieta > maxieta)
          ++nIgnore;
        //Sum cell energy for comparison to other algos.
        if (ieta >= minieta && ieta <= maxieta) {
          ttAmpTot += e;
        } else {
          ttAmpTotIg += e;
        }

        if (msgLvl(MSG::VERBOSE)) {
          /* Diagnostic checks: */
          int side = m_tileID->side(pmt_id);
          int tower = m_tileID->tower(pmt_id);
          int sample = m_tileID->sample(pmt_id);
          int pmt = m_tileID->pmt(pmt_id);
          int channel = m_tileHWID->channel(hwid);
          msg(MSG::VERBOSE) << "New RawChannel:"
                            << " ros=" << ros
                            << ", drawer=" << drawer
                            << ", ch=" << channel
                            << ", side=" << side
                            << ", tower=" << tower
                            << ", sample=" << sample
                            << ", pmt=" << pmt
                            << ", e=" << e
                            << ", ie=" << ieta
                            << ", ip=" << iphi;

          if (ieta >= minieta && ieta <= maxieta)
            msg(MSG::VERBOSE) << endmsg;
          else
            msg(MSG::VERBOSE) << " Outside limits" << endmsg;
        }

      } else {
        ATH_MSG_VERBOSE( "Tile Channel with no tt_id" );
      }

    } // end loop over rawChannels in this drawer.

    nTTTot += nTT;
    nRawChannelTot += nRawChannel;
    nIgnoreTot += nIgnore;

    ATH_MSG_VERBOSE( "Statistics for"
                    << " ROS=" << ros
                    << ", drawer=" << drawer
                    << "; posneg=" << posneg
                    << ", minieta=" << minieta
                    << ", maxieta=" << maxieta
                    << "; nTT=" << nTT
                    << ", nRawChannel=" << nRawChannel
                    << ", nIgnore=" << nIgnore );

    /*......................................................*/
    // Step 7: We now have all the TTL1 amplitudes for this drawer.  
    // Loop over towers to produce the electronics signals (= time samples).
    // If tileNoise is requested, generate random numbers to give noise
    for (int ieta = minieta; ieta <= maxieta; ++ieta) {
      int iphi = drawer;
      bool Good = ttRawChannel[ieta];
      if (tileNoise)
        Good = true;
      if (Good) {
        if (!ttRawChannel[ieta])
          ttId[ieta] = m_TT_ID->tower_id(posneg, 1, 0, ieta, drawer);

        ttL1NoiseSigma = m_tileInfo->TTL1NoiseSigma(ttId[ieta]);
        ttL1Thresh = m_tileInfo->TTL1Thresh(ttId[ieta]);
        ttL1Ped = m_tileInfo->TTL1Ped(ttId[ieta]);
        ttL1Calib = m_tileInfo->TTL1Calib(ttId[ieta]);
        ttAmp[ieta] *= ttL1Calib; // convert pCb to mV
        if (!m_constantTTL1shape) {
          // Get phase of the TTL1 tower. default=0,
          // meaning L1Cal Trigger samples TTL1 pulse right at the peak.
          // ieta: barrel=0-8, ext.barrel=9-14
          m_phase = m_tileInfo->ttl1Phase(posneg, ieta, iphi);
          /* Include shaping fuction, pedestal, and noise. */
          m_tileInfo->ttl1Shape(m_nSamp, m_iTrig, m_phase, m_TTL1Shape);
        }
        if (tileNoise)
          CLHEP::RandGauss::shootArray(m_nSamp, Rndm);
        for (int jsamp = 0; jsamp < m_nSamp; ++jsamp) {
          ttL1samples[jsamp] = ttAmp[ieta] * m_TTL1Shape[jsamp] + ttL1Ped;
          if (tileNoise)
            ttL1samples[jsamp] += ttL1NoiseSigma * Rndm[jsamp];
        }  // end loop over samples
        if (tileThresh) {
          if (ttL1samples[m_iTrig] - ttL1Ped < ttL1Thresh)
            Good = false;
        }
      } // end first "Good" section.
      /* Create the new TTL1 object and store in TTL1Container. */
      if (Good) {
        ATH_MSG_DEBUG( " TTL1:  "
                      << " ros=" << ros
                      << ", ieta=" << ieta
                      << ", iphi=" << iphi
                      << ", rawChannelTrue=" << ttRawChannel[ieta]
                      << ", Good=" << Good
                      << ", amp0=" << ttAmp[ieta]
                      << ", digitIn=" << ttL1samples[m_iTrig] );

        /*
         The following lines are commented out.

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "          ttL1Digits=";
          for (int jsamp = 0; jsamp < nSamp; ++jsamp) {
            msg(MSG::VERBOSE) << ttL1samples[jsamp] << "  ";
          }
          msg(MSG::VERBOSE) << endmsg;

          msg(MSG::VERBOSE) << "               Rndm=";
          for (int jsamp = 0; jsamp < nSamp; ++jsamp) {
            msg(MSG::VERBOSE) << Rndm[jsamp] << "  ";
          }
          msg(MSG::VERBOSE) << endmsg;
        }
        The preceding lines are commented out
        */

        ttl1Container->push_back(std::make_unique<TileTTL1>(ttId[ieta], ttL1samples));
      }  // end second "Good" section.
    } // end loop over towers
  } // end loop over collections

  // Execution completed.
  ATH_MSG_DEBUG( "TileRawChannelToTTL1 execution completed." );
  ATH_MSG_DEBUG( " nTTTot=" << nTTTot
                << " nRawChannelTot=" << nRawChannelTot
                << " nIgnoreTot=" << nIgnoreTot
                << " ttAmpTot=" << ttAmpTot
                << " ttAmpTotIg=" << ttAmpTotIg
                << " =>eneTot=" << ttAmpTot + ttAmpTotIg );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelToTTL1::finalize() {

  ATH_MSG_INFO( "TileRawChannelToTTL1::finalize() end" );

  return StatusCode::SUCCESS;
}



