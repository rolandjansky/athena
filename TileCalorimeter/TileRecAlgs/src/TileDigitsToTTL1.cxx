/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileDigitsToTTL1.cxx
//  Author   : Pedro Amaral following example from TileHitToTTL1
//  Created  : Feb, 2005
//
//  DESCRIPTION:
//      Created to emulate the Tile Level 1 Trigger Towers (TTL1), which are
//      hardware sums of the Tile channels/pmts, from 4 to 6 channels per tower.
//      The towers are read out in N time slices, with N=9 as a default.
//      Each TileDigit contains the corresponding N slices.
//      The resulting TTL1 object should be used by L1Calo simulation
//      This is the "data" (ie, digits) version of the simulation (ie, hit) algorithm
//      Output object TTL1 should be in units of mV.
//
//      For each digit, the first time sample (0) is taken as pedestal, and subtracted
//      from all the digits. This assumes that the pedestal is introduced by the digitizer
//      The digits are then converted from ADC counts to pCb.
//      Then digits with the same eta, within the same drawer are summed, time sample
//      by time sample. ie, sum is done in charge.
//      In the end, the pCb are converted back to mV.
//      And noise of the adder (in mV) is added.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileRecAlgs/TileDigitsToTTL1.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCondToolEmscale.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/errorcheck.h"

//CLHEP includes
#include <CLHEP/Random/Randomize.h>

//C++ STL includes
#include <vector>
#include <memory>


//
// Constructor
//
TileDigitsToTTL1::TileDigitsToTTL1(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_tileInfo(0)
  , m_TT_ID(0)
  , m_tileToolEmscale("TileCondToolEmscale")
{
  // which CIS constants will be used for calibration
  m_rChType = TileFragHash::FitFilter;

  declareProperty("TileInfoName", m_infoName = "TileInfo");
  declareProperty("TileCondToolEmscale"    , m_tileToolEmscale);
}

TileDigitsToTTL1::~TileDigitsToTTL1() {
}

//
// Alg standard initialize function
//
StatusCode TileDigitsToTTL1::initialize() {

  // retrieve CaloLVL1_ID, TileID, TileHWID helpers and TileIfno from det store

  CHECK( detStore()->retrieve(m_TT_ID) );
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );

  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );

  ATH_CHECK(m_digitsContainerKey.initialize());
  ATH_CHECK(m_ttl1ContainerKey.initialize());

  ATH_MSG_INFO( "TileDigitsToTTL1 initialisation completed" );

  return StatusCode::SUCCESS;
}

/*==========================================================================*/
//
// Begin Execution Phase.
//
StatusCode TileDigitsToTTL1::execute() {

  ATH_MSG_DEBUG( "Executing TileDigitsToTTL1" );

  /*......................................................*/
  // Step 2:  Get all global parameters that will be needed for processing.
  int nSamp = m_tileInfo->NdigitSamples(); // number of time slices for each chan
  int iTrig = m_tileInfo->ItrigSample();   // index of the triggering time slice

  /* Get TileNoise flag from TileInfo (true => generate noise in TileDigits) */
  bool tileNoise = m_tileInfo->TileNoise();
  /* Get TileZeroSuppress flag from TileInfo
   (true => apply threshold to Digits) */
  bool tileThresh = m_tileInfo->TileZeroSuppress();
  // declare array for random number generation for noise in samples.
  double Rndm[16];      // Can't use variable size array

  ATH_MSG_DEBUG( "nSamp=" << nSamp
                << ", iTrig=" << iTrig
                << ", tileNoise=" << ((tileNoise) ? "true" : "false")
                << ", tileThresh=" << ((tileThresh) ? "true" : "false") );

  /*......................................................*/
  // step 3:  Get digit container from TES and create TTL1 container
  /* Note that digit container has 256 collections (one for each drawer),
   but TTL1 container has no collections and no structure. */
  SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey);
  ATH_CHECK( digitsContainer.isValid() );

  SG::WriteHandle<TileTTL1Container> ttl1Container(m_ttl1ContainerKey);
  ATH_CHECK( ttl1Container.record(std::make_unique<TileTTL1Container>()) );
  ATH_MSG_DEBUG( "TileTTL1Container registered successfully (" << m_ttl1ContainerKey.key() << ")" );

  /*......................................................*/
  // Step 4: Create temporary arrays for processing signals.
  /* Create array for all TT amplitudes in a single drawer. */
  Identifier ttId[16]; // array of TT identifiers in a single drawer
  bool ttDigit[16];    // array of TT occupancy in a single drawer
  int nTT;             // number of digit towers in this drawer.
  int nDigit;          // number of digits in this drawer.
  int nIgnore;         // number of ignored digits in this drawer.
  int nTTTot = 0;        // total number of digit towers.
  int nDigitTot = 0;     // total number of digits.
  int nIgnoreTot = 0;    // total number of ignored digits.
  float ttAmpTot = 0;    // total energy in good level-1 towers.
  float ttAmpTotIg = 0.; // total energy in "ignored" level-1 towers.
  int minieta, maxieta, posneg;

  /* Create array for the nSamp time-samples of a single tower. */
  std::vector<float> ttL1samples(nSamp);

  /*......................................................*/
  // Step 5: Begin loop over all collections (collection = electronic drawer). 
  for (const TileDigitsCollection* digitsCollection : *digitsContainer) {

    HWIdentifier drawer_id = m_tileHWID->drawer_id(digitsCollection->identify());
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
    memset(ttDigit, 0, sizeof(ttDigit));
    std::vector<std::vector<float> > myttAmp(16);
    for (int ii = 0; ii < 16; ++ii)
      myttAmp[ii].resize(nSamp);

    nTT = nIgnore = nDigit = 0;

    /*......................................................*/
    // Step 6: Iterate over all digits in this collection, summing amps for each tower.
    for (const TileDigits* tile_digits : *digitsCollection) {

      // get digits
      std::vector<float> samples = tile_digits->samples();
      // get number of time samples & compare with 
      // int nSamp = m_tileInfo->NdigitSamples();
      int nSamp2 = samples.size();
      if (nSamp2 != nSamp) {
        ATH_MSG_ERROR( "nSamp from TileInfo=" << nSamp
                      << " nSamp from digits= " << nSamp2 );
      }
      /* Get digit HWIdentifier (= channel_id) */
      HWIdentifier adcId = tile_digits->adc_HWID();
      int channel = m_tileHWID->channel(adcId);
      int adc = m_tileHWID->adc(adcId);
      // Subtract pedestal, that is samples[0] and convert from ADC counts to pCb. 
      float pedestal = samples[0];
      for (int jsamp = 0; jsamp < nSamp; ++jsamp) {

        samples[jsamp] = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc,
            (samples[jsamp] - pedestal), TileRawChannelUnit::ADCcounts, TileRawChannelUnit::PicoCoulombs);

      }
      Identifier pmt_id = tile_digits->pmt_ID();
      if (pmt_id.is_valid()) {

        /* Get TT Identifier for this pmt */
        Identifier tt_id = tile_digits->tt_ID();
        /* Get eta-phi  indices of TTL1 for this channel. */
        int ieta = m_TT_ID->eta(tt_id);
        int iphi = m_TT_ID->phi(tt_id); // (same as module).
        if (iphi != drawer)
          ATH_MSG_ERROR( "drawer=" << drawer << ", iphi=" << iphi );

        if (ttDigit[ieta]) { // already exists - just add charge for each sample
          for (int jsamp = 0; jsamp < nSamp; ++jsamp) {
            myttAmp[ieta][jsamp] += samples[jsamp];
          }
        } else { // digit in new TT
          ttId[ieta] = tt_id;
          ttDigit[ieta] = true;
          for (int jsamp = 0; jsamp < nSamp; ++jsamp) {
            myttAmp[ieta][jsamp] = samples[jsamp];
          }
          if (ieta >= minieta && ieta <= maxieta)
            ++nTT; // count only valid TT
        }
        ++nDigit;
        if (ieta < minieta || ieta > maxieta)
          ++nIgnore;

        //Sum cell energy for comparison to other algos.
        // convert pCb to MeV
        float e = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc,
            samples[iTrig], TileRawChannelUnit::PicoCoulombs, TileRawChannelUnit::MegaElectronVolts);

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
          int channel = m_tileHWID->channel(adcId);

          msg(MSG::VERBOSE) << "New Digit:"
                            << " ros=" << ros
                            << ", drawer=" << drawer
                            << ",  ch=" << channel
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
    } // end loop over digits in this drawer.

    nTTTot += nTT;
    nDigitTot += nDigit;
    nIgnoreTot += nIgnore;

    ATH_MSG_VERBOSE( "      Statistics for"
                    << " ROS=" << ros
                    << ", drawer=" << drawer
                    << ";  posneg=" << posneg
                    << ", minieta=" << minieta
                    << ", maxieta=" << maxieta
                    << ";  nTT=" << nTT
                    << ", nDigit=" << nDigit
                    << ", nIgnore=" << nIgnore );

    /*......................................................*/
    // Step 7: We now have all the TTL1 amplitudes for this drawer.  
    // Loop over towers to produce the electronics signals (= time samples).
    // If tileNoise is requested, generate random numbers to give noise
    for (int ieta = minieta; ieta <= maxieta; ++ieta) {
      int iphi = drawer;
      bool Good = ttDigit[ieta];
      if (tileNoise)
        Good = true;

      if (Good) {
        if (!ttDigit[ieta])
          ttId[ieta] = m_TT_ID->tower_id(posneg, 1, 0, ieta, drawer);

        float ttL1Calib = m_tileInfo->TTL1Calib(ttId[ieta]);
        for (int jsamp = 0; jsamp < nSamp; ++jsamp) {
          myttAmp[ieta][jsamp] *= ttL1Calib; // convert pCb to mV
        }

        /* Include shaping fuction, pedestal, and noise. */
        if (tileNoise)
          CLHEP::RandGauss::shootArray(nSamp, Rndm);
        float ttL1Ped = m_tileInfo->TTL1Ped(ttId[ieta]);
        float ttL1NoiseSigma = m_tileInfo->TTL1NoiseSigma(ttId[ieta]);
        for (int jsamp = 0; jsamp < nSamp; ++jsamp) {
          ttL1samples[jsamp] = myttAmp[ieta][jsamp] + ttL1Ped;
          if (tileNoise)
            ttL1samples[jsamp] += ttL1NoiseSigma * Rndm[jsamp];
        }  // end loop over samples
        if (tileThresh) {
          float ttL1Thresh = m_tileInfo->TTL1Thresh(ttId[ieta]);
          if (ttL1samples[iTrig] - ttL1Ped < ttL1Thresh)
            Good = false;
        }
      } // end first "Good" section.
      /* Create the new TTL1 object and store in TTL1Container. */
      if (Good) {
        ATH_MSG_DEBUG( " TTL1:  "
                      << " ros=" << ros
                      << ", ieta=" << ieta
                      << ", iphi=" << iphi
                      << ", digitTrue=" << ttDigit[ieta]
                      << ", Good=" << Good
                      << ", amp0=" << myttAmp[ieta][iTrig]
                      << ", digitIn=" << ttL1samples[iTrig] );

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
         The preceding lines are commented out.
         */

        ttl1Container->push_back(std::make_unique<TileTTL1>(ttId[ieta], ttL1samples));
      }  // end second "Good" section.
    } // end loop over towers
  } // end loop over collections


  // Execution completed.
  ATH_MSG_DEBUG( "TileDigitsToTTL1 execution completed." );
  ATH_MSG_DEBUG( " nTTTot=" << nTTTot
                << " nDigitTot=" << nDigitTot
                << " nIgnoreTot=" << nIgnoreTot
                << "  ttAmpTot=" << ttAmpTot
                << " ttAmpTotIg=" << ttAmpTotIg
                << " =>eneTot=" << ttAmpTot + ttAmpTotIg );

  return StatusCode::SUCCESS;
}

StatusCode TileDigitsToTTL1::finalize() {

  ATH_MSG_INFO( "TileDigitsToTTL1::finalize() end" );

  return StatusCode::SUCCESS;
}

