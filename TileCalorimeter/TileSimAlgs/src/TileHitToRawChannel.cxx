/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// Gaudi Includes
#include "GaudiKernel/ISvcLocator.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"

// Tile includes
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileSimAlgs/TileHitToRawChannel.h"

//CLHEP includes
#include <CLHEP/Random/Randomize.h>

// For the Athena-based random numbers.
#include "AthenaKernel/IAtRndmGenSvc.h"


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
  , m_tileToolEmscale("TileCondToolEmscale")
{
  m_rChUnit = TileRawChannelUnit::ADCcounts;
  m_rChType = TileFragHash::Default;

  declareProperty("TileHitContainer", m_hitContainer = "TileHitCnt");    
  declareProperty("TileRawChannelContainer", m_rawChannelContainer  = "TileRawChannelCnt");
  declareProperty("TileInfoName", m_infoName  = "TileInfo");
  declareProperty("DeltaT",m_deltaT = -1.0); // keep hits only within deltaT;
  declareProperty("calibrateEnergy",m_calibrateEnergy = false); // convert or not to pCb
  declareProperty("TileCondToolEmscale"    , m_tileToolEmscale);
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

  CHECK( detStore()->retrieve(m_tileID) );

  CHECK( detStore()->retrieve(m_tileTBID) );

  CHECK( detStore()->retrieve(m_tileHWID) );

  CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );

  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

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

  ATH_MSG_INFO( "TileHitToRawChannel initialization completed" );

  return StatusCode::SUCCESS;
}

//
// Alg standard execute function
//
StatusCode TileHitToRawChannel::execute() {

  ATH_MSG_DEBUG( "Executing TileHitToRawChannel" );

  // step1: read hits from TES
  const TileHitContainer* hitCont;
  CHECK( evtStore()->retrieve(hitCont, m_hitContainer) );

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
  TileRawChannel * pRawChannel;
  TileRawChannelContainer * pRawChannelContainer;
  pRawChannelContainer = new TileRawChannelContainer(true, m_rChType, m_rChUnit);

  // iterate over all collections in a container
  TileHitContainer::const_iterator collItr = hitCont->begin();
  TileHitContainer::const_iterator lastColl = hitCont->end();

  for (; collItr != lastColl; ++collItr) {

    HWIdentifier drawer_id = m_tileHWID->drawer_id((*collItr)->identify());
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
                    << "Collection = 0x" << (*collItr)->identify() << MSG::dec
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
        adc_ampl[ch] = random[ch] * m_tileInfo->ChannelNoiseSigma(TileID::HIGHGAIN, ch, idhash);
      }
    } else {
      memset(adc_gain, -1, sizeof(adc_gain)); /* TileID::INVALID */
      memset(adc_ampl, 0, sizeof(adc_ampl));
    }

    memset(adc_time, 0, sizeof(adc_time));
    memset(adc_qual, 0, sizeof(adc_qual));

    // iterate over all hits in a collection
    TileHitCollection::const_iterator hitItr = (*collItr)->begin();
    TileHitCollection::const_iterator lastHit = (*collItr)->end();

    for (; hitItr != lastHit; ++hitItr) {

      // Get hit Identifier (= identifier of pmt)
      const TileHit * phit = (*hitItr);
      Identifier pmt_id = phit->pmt_ID();
      HWIdentifier channel_id = phit->pmt_HWID();

      // index for array is the channel number
      ch = m_tileHWID->channel(channel_id);

      /* Get hit amplitude and convert to energy (cell-dependent) */
      double e_hit = phit->energy();
      double time = phit->time();
      if (m_deltaT > 0.0) {
        if (fabs(time) >= m_deltaT) { // reset first hit if it's outside deltaT window
          e_hit = 0.0;
          time = 0.0;
        }
        double etime = e_hit * time;
        for (int i = phit->size() - 1; i > 0; --i) { // don't use first hit (i=0)
          double en = phit->energy(i);
          double ti = phit->time(i);
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
      if (adc_ampl[ch] + amp_ch + m_tileInfo->DigitsPedLevel(TileID::HIGHGAIN, ch, idhash)
          > m_ampMaxHi) {
        gain = TileID::LOWGAIN;
        amp_ch = e_ch / m_tileToolEmscale->channelCalib(drawerIdx, ch, gain, 1., m_rChUnit
                                                        , TileRawChannelUnit::MegaElectronVolts);

        // If Noise is requested, 
        // recalculate noise using the SAME random number as for high.
        if (m_tileNoise) {
          adc_ampl[ch] = random[ch] * m_tileInfo->ChannelNoiseSigma(TileID::LOWGAIN, ch, idhash);
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

          pRawChannel = new TileRawChannel(adc_id, amp, adc_time[ch], adc_qual[ch]);
          pRawChannelContainer->push_back(pRawChannel);

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

  // step3: register the cell container in the TES
  CHECK( evtStore()->record(pRawChannelContainer, m_rawChannelContainer, false) );

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

  return StatusCode::SUCCESS;
}

StatusCode TileHitToRawChannel::finalize() {

  ATH_MSG_INFO( "TileHitToRawChannel::finalize() end." );

  return StatusCode::SUCCESS;
}

