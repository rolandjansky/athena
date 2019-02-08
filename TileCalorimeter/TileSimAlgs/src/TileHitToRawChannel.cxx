/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileHitToRawChannel.cxx
//  Author   : Zhifang
//  Created  : April, 2002
//
//  DESCRIPTION:
//     Implement the TileHitToRawChannel class
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileSimAlgs/TileHitToRawChannel.h"
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileMutableRawChannelContainer.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/errorcheck.h"
// For the Athena-based random numbers.
#include "AthenaKernel/IAtRndmGenSvc.h"

//CLHEP includes
#include <CLHEP/Random/Randomize.h>


using CLHEP::RandGaussQ;


//
// Constructor
//
TileHitToRawChannel::TileHitToRawChannel(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_rChUnit(TileRawChannelUnit::ADCcounts)
  , m_rChType(TileFragHash::Default)
  , m_tileID(0)
  , m_tileTBID(0)
  , m_tileHWID(0)
  , m_tileInfo(0)
  , m_cabling(0)
  , m_tileNoise(false)
  , m_tileThresh(false)
  , m_threshHi(0.0)
  , m_ampMaxHi(0.0)
  , m_atRndmGenSvc(0)
  , m_pHRengine(0)
{
  m_rChUnit = TileRawChannelUnit::ADCcounts;
  m_rChType = TileFragHash::Default;

  declareProperty("TileInfoName", m_infoName  = "TileInfo");
  declareProperty("DeltaT", m_deltaT = -1.0); // keep hits only within deltaT;
  declareProperty("calibrateEnergy", m_calibrateEnergy = false); // convert or not to pCb
}

TileHitToRawChannel::~TileHitToRawChannel() {
  std::vector<HWIdentifier *>::iterator itr = m_all_ids.begin();
  std::vector<HWIdentifier *>::iterator last = m_all_ids.end();

  for (; itr != last; ++itr) {
    delete[] (*itr);
  }
}

//
// Alg standard initialize function
//
StatusCode TileHitToRawChannel::initialize() {
  m_rChUnit = (m_calibrateEnergy)
               ? TileRawChannelUnit::ADCcounts
               : TileRawChannelUnit::PicoCoulombs;

  // retrieve TileID helper and TileInfo from det store

  ATH_CHECK( detStore()->retrieve(m_tileID) );

  ATH_CHECK( detStore()->retrieve(m_tileTBID) );

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );

  //=== get TileCondToolEmscale
  ATH_CHECK( m_tileToolEmscale.retrieve() );

  ATH_CHECK( m_tileToolNoiseSample.retrieve() );

  if (m_tileNoise) {
    static const bool CREATEIFNOTTHERE_RNDM(true);
    CHECK( service("AtRndmGenSvc", m_atRndmGenSvc, CREATEIFNOTTHERE_RNDM));
    m_pHRengine = m_atRndmGenSvc->GetEngine("Tile_DigitsMaker");
  }

  m_cabling = TileCablingService::getInstance();

  // Get needed parameters from TileInfo
  m_ampMaxHi = m_tileInfo->ADCmax();

  /* Get TileNoise flag from TileInfo 
   (true => generate noise in RC's). */
  m_tileNoise = m_tileInfo->TileNoise();

  /* Get TileZeroSuppress flag from TileInfo 
   (true => apply threshold cut for RC's). */
  m_tileThresh = m_tileInfo->TileZeroSuppress();

  m_threshHi = m_tileInfo->ThresholdRawChannel(TileID::HIGHGAIN);

  if (m_tileThresh) {
    ATH_MSG_INFO( "ampMaxHi=" << m_ampMaxHi
                   << ", tileNoise=" << ((m_tileNoise) ? "true" : "false")
                   << ", tileThresh=" << "true, thresh_hi=" << m_threshHi );
  } else {
    ATH_MSG_INFO( "ampMaxHi=" << m_ampMaxHi
                   << ", tileNoise=" << ((m_tileNoise) ? "true" : "false")
                   << ", tileThresh=" << "false" );
  }


  IdContext drawer_context = m_tileHWID->drawer_context();
  int ndrawers = m_tileHWID->drawer_hash_max();
  int nchannels = 48;

  m_all_ids.reserve(ndrawers);

  for (int dr = 0; dr < ndrawers; ++dr) {

    HWIdentifier drawer_id;
    /* int result = */m_tileHWID->get_id(dr, drawer_id, &drawer_context);

    HWIdentifier * adc_ids = new HWIdentifier[nchannels];
    if (m_tileHWID->ros(drawer_id) > 0) { // protection against BEAM ROD
      for (int ch = 0; ch < nchannels; ++ch) {
        adc_ids[ch] = m_tileHWID->adc_id(drawer_id, ch, TileID::HIGHGAIN);
      }
    }
    m_all_ids.push_back(adc_ids);
  }

  ATH_CHECK( m_hitContainerKey.initialize() );
  ATH_CHECK( m_rawChannelContainerKey.initialize() );

  ATH_MSG_INFO( "TileHitToRawChannel initialization completed" );

  return StatusCode::SUCCESS;
}

//
// Alg standard execute function
//
StatusCode TileHitToRawChannel::execute() {

  ATH_MSG_DEBUG( "Executing TileHitToRawChannel" );

  // step1: read hits from TES
  SG::ReadHandle<TileHitContainer> hitContainer(m_hitContainerKey);
  ATH_CHECK( hitContainer.isValid() );

  //Zero sums for monitoring.
  int nChan = 0;
  int nChLG = 0;
  int nChHG = 0;
  double eHitTot = 0.;
  double eCh = 0.;
  double eChLG = 0.;
  double eChHG = 0.;

  // prepare arrays for all channels in one drawer (collection)
  const int nChMax = 48;
  double random[nChMax];
  memset(random, 0, sizeof(random));
  HWIdentifier * adc_ids;
  int adc_gain[nChMax];
  double adc_ampl[nChMax];
  double adc_time[nChMax];
  double adc_qual[nChMax];

  IdContext drawer_context = m_tileHWID->drawer_context();

  // step2: form raw channels, and put them in container
  auto rawChannelContainer = std::make_unique<TileMutableRawChannelContainer>(true, m_rChType, m_rChUnit);
  ATH_CHECK( rawChannelContainer->status() );

  // iterate over all collections in a container
  for (const TileHitCollection* hitCollection : *hitContainer) {

    HWIdentifier drawer_id = m_tileHWID->drawer_id(hitCollection->identify());
    int ros = m_tileHWID->ros(drawer_id);
    int drawer = m_tileHWID->drawer(drawer_id);
    int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    if (m_cabling->connected(ros, drawer)) {
      ATH_MSG_VERBOSE( "ROS " << ros
                      << " drawer " << drawer
                      << " is connected" );
    } else {
      continue;
    }
    int ch;

    ATH_MSG_VERBOSE( MSG::hex
                    << "Collection = 0x" << hitCollection->identify() << MSG::dec
                    << " : ROS=" << ros
                    << ", drawer=" << drawer );

    IdentifierHash idhash;
    /* int result = */m_tileHWID->get_hash(drawer_id, idhash, &drawer_context);
    adc_ids = m_all_ids[idhash];

    // If tileNoise is requested, generate random numbers to give noise
    if (m_tileNoise) {

      RandGaussQ::shootArray(m_pHRengine, nChMax, random, 0.0, 1.0);

      for (ch = 0; ch < nChMax; ++ch) {
        adc_gain[ch] = TileID::HIGHGAIN;
        adc_ampl[ch] = random[ch] * m_tileToolNoiseSample->getHfn(drawerIdx, ch, TileID::HIGHGAIN)
                                  * m_tileInfo->getNoiseScaleFactor();

      }
    } else {
      memset(adc_gain, -1, sizeof(adc_gain)); /* TileID::INVALID */
      memset(adc_ampl, 0, sizeof(adc_ampl));
    }

    memset(adc_time, 0, sizeof(adc_time));
    memset(adc_qual, 0, sizeof(adc_qual));

    // iterate over all hits in a collection
    for (const TileHit* tile_hit : *hitCollection) {

      // Get hit Identifier (= identifier of pmt)
      Identifier pmt_id = tile_hit->pmt_ID();
      HWIdentifier channel_id = tile_hit->pmt_HWID();

      // index for array is the channel number
      ch = m_tileHWID->channel(channel_id);

      /* Get hit amplitude and convert to energy (cell-dependent) */
      double e_hit = tile_hit->energy();
      double time = tile_hit->time();
      if (m_deltaT > 0.0) {
        if (fabs(time) >= m_deltaT) { // reset first hit if it's outside deltaT window
          e_hit = 0.0;
          time = 0.0;
        }
        double etime = e_hit * time;
        for (int i = tile_hit->size() - 1; i > 0; --i) { // don't use first hit (i=0)
          double en = tile_hit->energy(i);
          double ti = tile_hit->time(i);
          if (fabs(ti) < m_deltaT && en > 0.0) {
            e_hit += en;
            etime += en * ti;
          }
        }
        if (e_hit > 0.0)
          time = etime / e_hit;
        else
          continue; // go to next hit, do not create channels with zero energy
      }
      adc_time[ch] = time;
      double hit_calib = m_tileInfo->HitCalib(pmt_id);
      double e_ch = e_hit * hit_calib;

      /* Convert to amplitude of channel (assuming high gain) */
      /* need to divide here, because "calib" converts amp to energy */
      int gain = TileID::HIGHGAIN;
      double amp_ch = e_ch / m_tileToolEmscale->channelCalib(drawerIdx, ch, gain, 1., m_rChUnit
                                                             , TileRawChannelUnit::MegaElectronVolts);
      double noise;
      // If high saturates, convert adc_id to low-gain value and recalculate.
      if (adc_ampl[ch] + amp_ch + m_tileToolNoiseSample->getPed(drawerIdx, ch, gain) > m_ampMaxHi) {

        gain = TileID::LOWGAIN;
        amp_ch = e_ch / m_tileToolEmscale->channelCalib(drawerIdx, ch, gain, 1., m_rChUnit
                                                        , TileRawChannelUnit::MegaElectronVolts);

        // If Noise is requested, 
        // recalculate noise using the SAME random number as for high.
        if (m_tileNoise) {
          adc_ampl[ch] = random[ch] * m_tileToolNoiseSample->getHfn(drawerIdx, ch, TileID::LOWGAIN)
                                    * m_tileInfo->getNoiseScaleFactor();
        }
      }

      adc_gain[ch] = gain;
      noise =  adc_ampl[ch];
      adc_ampl[ch] += amp_ch; // add real amplitude to the noise

      ATH_MSG_VERBOSE( "ch=" << ch << ((gain == TileID::HIGHGAIN) ? " Hi" :" Lo")
                      << " pmt=" << m_tileID->to_string(pmt_id, -1)
                      << " adc=" << m_tileHWID->to_string(channel_id, -1) << "/" << gain
                      << " noise=" << noise
                      << " amp=" << amp_ch
                      << " tot=" << adc_ampl[ch]);

      ++nChan;
      eHitTot += e_hit;
      eCh += e_ch;
    }

    // store raw channels for one drawer

    for (int ch = 0; ch < nChMax; ++ch) {

      int gain = adc_gain[ch];

      // without noise most of channels do not exist
      // select only valid channels
      if (gain != -1) {

        HWIdentifier adc_id = adc_ids[ch];
        double amp = adc_ampl[ch];

        bool lrcGood = true;
        double thresh = -1024.;

        if (TileID::HIGHGAIN != gain) {
          // change ADC ID (channel switched to low gain)
          adc_id = m_tileHWID->adc_id(m_tileHWID->channel_id(adc_id), gain);

        } else {
          // Apply threshold cut to small signals (always high gain)
          if (m_tileThresh) {
            thresh = m_threshHi;
            if (thresh < 0) {
              if (fabs(amp) < fabs(thresh))
                lrcGood = false;
            } else {
              if (amp < thresh)
                lrcGood = false;
            }
          }
        }

        if (lrcGood) {

          auto rawChannel = std::make_unique<TileRawChannel>(adc_id, amp, adc_time[ch], adc_qual[ch]);
          ATH_CHECK( rawChannelContainer->push_back(std::move(rawChannel)) );

          if (TileID::HIGHGAIN == gain) {
            ++nChHG;
            eChHG += amp;
          } else {
            ++nChLG;
            eChLG += amp;
          }

          ATH_MSG_VERBOSE( "Accept RC thr=" << ((thresh < 0) ? "+" : "" ) << thresh
                          << ", id=" << m_tileHWID->to_string(adc_id)
                          << ", amp=" << amp );

        } else {

          ATH_MSG_VERBOSE( "Reject RC thr=" << ((thresh < 0) ? "+" : "") << thresh
                          << ", id=" << m_tileHWID->to_string(adc_id)
                          << ", amp=" << amp );
        }
      }
    }
  }


  // Execution completed.
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "TileHitToRawChannel execution completed." << endmsg;
    msg(MSG::DEBUG) << " nChan=" << nChan
                    << " eHitTot=" << eHitTot
                    << " eneTot=" << eCh
                    << " nchLG=" << nChLG
                    << " eChLG=" << eChLG
                    << " nchHG=" << nChHG
                    << " eChHG=" << eChHG << endmsg;
  }


  SG::WriteHandle<TileRawChannelContainer> rawChannelCnt(m_rawChannelContainerKey);
  ATH_CHECK( rawChannelCnt.record(std::move(rawChannelContainer)) );

  return StatusCode::SUCCESS;
}

StatusCode TileHitToRawChannel::finalize() {

  ATH_MSG_INFO( "TileHitToRawChannel::finalize() end." );

  return StatusCode::SUCCESS;
}

