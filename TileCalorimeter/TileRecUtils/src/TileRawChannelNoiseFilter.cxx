/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Atlas includes
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// Tile includes
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileRawChannel.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileRecUtils/TileRawChannelNoiseFilter.h"
#include "TileConditions/ITileBadChanTool.h"
//#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileRecUtils/TileBeamInfoProvider.h"

static const InterfaceID IID_ITileRawChannelNoiseFilter("TileRawChannelNoiseFilter", 1, 0);

const InterfaceID& TileRawChannelNoiseFilter::interfaceID() {
  return IID_ITileRawChannelNoiseFilter;
}

//========================================================
// constructor
TileRawChannelNoiseFilter::TileRawChannelNoiseFilter(const std::string& type,
    const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_tileHWID(0)
    , m_tileToolEmscale("TileCondToolEmscale")
    , m_tileToolNoiseSample("TileCondToolNoiseSample")
    , m_tileBadChanTool("TileBadChanTool")
    , m_beamInfo( "TileBeamInfoProvider/TileBeamInfoProvider")
    , m_truncationThresholdOnAbsEinSigma(3.0) // 3 sigma of ADC HF noise by default
    , m_minimumNumberOfTruncatedChannels(0.6) // at least 60% of channels should be below threshold
    , m_useTwoGaussNoise(false) // do not use 2G - has no sense for ADC HF noise for the moment
    , m_useGapCells(false) // use gap cells for noise filter as all normal cells
{
  declareInterface<ITileRawChannelTool>(this);
  declareInterface<TileRawChannelNoiseFilter>(this);

  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample);
  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("BeamInfo", m_beamInfo);

  declareProperty("TruncationThresholdOnAbsEinSigma", m_truncationThresholdOnAbsEinSigma);
  declareProperty("MinimumNumberOfTruncatedChannels", m_minimumNumberOfTruncatedChannels);
  declareProperty("UseTwoGaussNoise", m_useTwoGaussNoise);
  declareProperty("UseGapCells", m_useGapCells);
  declareProperty("MaxNoiseSigma", m_maxNoiseSigma = 5.0, "Channels with noise more than that value are igonred in calculation of correction");
}

//========================================================
// Initialize
StatusCode TileRawChannelNoiseFilter::initialize() {
  ATH_MSG_INFO("Initializing...");

  if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "TruncationThresholdOnAbsEinSigma = " 
                      << m_truncationThresholdOnAbsEinSigma << endmsg;
      msg(MSG::DEBUG) << "MinimumNumberOfTruncatedChannels = " 
                      << m_minimumNumberOfTruncatedChannels << endmsg;
      msg(MSG::DEBUG) << "UseTwoGaussNoise = " 
                      << ((m_useTwoGaussNoise)?"true":"false") << endmsg;
      msg(MSG::DEBUG) << "UseGapCells = " 
                      << ((m_useGapCells)?"true":"false") << endmsg;
  }

  CHECK( detStore()->retrieve(m_tileHWID) );

  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  //=== get TileCondToolNoiseSample
  CHECK( m_tileToolNoiseSample.retrieve() );

  //=== get TileBadChanTool
  CHECK( m_tileBadChanTool.retrieve() );

  //=== get TileBeamInfo
  CHECK( m_beamInfo.retrieve() );

  return StatusCode::SUCCESS;
}

// ============================================================================
// process container
StatusCode TileRawChannelNoiseFilter::process(
    TileRawChannelContainer *rchCont) {

  ATH_MSG_DEBUG("in process()");

  TileRawChannelUnit::UNIT rChUnit = rchCont->get_unit();
  std::string units[8] = { "ADC counts", "pC", "CspC", "MeV",
      "online ADC counts", "online pC", "online CspC", "online MeV" };

  if (rChUnit > TileRawChannelUnit::ADCcounts
      && rChUnit < TileRawChannelUnit::OnlineADCcounts) {

    ATH_MSG_ERROR( "Units in container is " << units[rChUnit] );
    ATH_MSG_ERROR( "Due to non-linear CIS constants noise filter is possible only with ADC counts ");
    ATH_MSG_ERROR( "Please, disable CIS calibration in optimal filter " );

    return StatusCode::FAILURE;
  }

  bool undoOnlCalib = (rChUnit > TileRawChannelUnit::OnlineADCcounts);
  ATH_MSG_VERBOSE( "Units in container is " << units[rChUnit] );

  // Now retrieve the TileDQStatus
  const TileDQstatus* DQstatus = m_beamInfo->getDQstatus();

  TileRawChannelContainer::const_iterator collItr = rchCont->begin();
  TileRawChannelContainer::const_iterator lastColl = rchCont->end();
  for (; collItr != lastColl; ++collItr) {

    /* Get drawer ID and build drawer index. */
    HWIdentifier drawer_id = m_tileHWID->drawer_id((*collItr)->identify());
    int ros = m_tileHWID->ros(drawer_id);
    int drawer = m_tileHWID->drawer(drawer_id);
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    bool eb = (ros > 2);
    bool ebspD4 = ((ros == 3 && drawer == 14) || (ros == 4 && drawer == 17));
    bool ebNsp  = !ebspD4 && eb;
    bool ebspC10 = (ebNsp && ((drawer>37 && drawer<42) || (drawer>53 && drawer<58) ) );

    static const int maxChannelDrawer = 48; // number of channels in one drawer
    static const int maxChannel = 12; // number of channels per motherboard
    static const int maxMOB = 4; // number of motherboards in one drawer

    float calib[maxChannelDrawer];
    float commonmode[maxMOB];
    int nemptychan[maxMOB];
    int ngoodchan[maxMOB];
    int chanmap[maxChannelDrawer];
    memset(calib, 0, sizeof(calib));
    memset(commonmode, 0, sizeof(commonmode));
    memset(nemptychan, 0, sizeof(nemptychan));
    memset(ngoodchan, 0, sizeof(ngoodchan));
    memset(chanmap, 0, sizeof(chanmap));

    // iterate over all channels in a collection
    const TileRawChannelCollection* coll = *collItr;
    TileRawChannelCollection::const_iterator rchItr = coll->cbegin();
    TileRawChannelCollection::const_iterator lastRch = coll->cend();

    for (; rchItr != lastRch; ++rchItr) {
      const TileRawChannel* rch = (*rchItr);

      HWIdentifier adc_id = rch->adc_HWID();
      //int index,pmt;
      //Identifier cell_id = rch->cell_ID_index(index,pmt);
      //if ( index == -1 ) continue; // this is to ignore disconnected channels - just for tests
      //if ( index < 0 )   continue; // this is to ingnore disconnected channels and MBTS - just for tests

      int chan = m_tileHWID->channel(adc_id);
      int gain = m_tileHWID->adc(adc_id);
      int mob = chan / maxChannel;
      bool empty = (eb && ( (chan > 41) || (chan > 23 && chan < 30) || (ebspD4 && chan < 3) ) );

      // use only good channel
      float ped=rch->pedestal();
      if (empty || ped > 55000. || (ped>2000. && ped < 39500.) // all bad patterns, ped=2047, underflow, overflow
          || m_tileBadChanTool->getAdcStatus(drawerIdx, chan, gain).isBad()
          || (!DQstatus->isAdcDQgood(ros, drawer, chan, gain))) continue;


      bool usechan = m_useGapCells ||  // always true if we want to use gap cells
                   ( ! ( ( ebNsp && (chan==0 || chan==1  || chan==12 || chan==13)) ||
                         ( ebspC10 && (chan==4 || chan==5)) ||
                         ( ebspD4 && (chan==18 || chan==19 || chan==12 || chan==13)) ) );

      ++chanmap[chan];
      // do not count good channels twice
      if (chanmap[chan] < 2 && usechan) ++ngoodchan[mob];
      // use only high gain
      if (gain != TileHWID::HIGHGAIN) continue;

      float amp = rch->amplitude();
      if (undoOnlCalib) {
        calib[chan] = m_tileToolEmscale->undoOnlCalib(drawerIdx, chan, gain, 1.0, rChUnit);
        amp *= calib[chan];
      } else {
        calib[chan] = 1.0;
      }

      
      if (usechan) {

        float noise_sigma = 1.5; // default value of HFN in high gain channel
        if (m_useTwoGaussNoise) {
          //float sigma1 = m_tileToolNoiseSample->getHfn1(drawerIdx, chan, gain);
          //float sigma2 = m_tileToolNoiseSample->getHfn2(drawerIdx, chan, gain);
          //float norm   = m_tileToolNoiseSample->getHfnNorm(drawerIdx, chan, gain);
          // still need to define noise_sigma in this case 
          // noise_sigma = ...
        } else {
          // take single gauss noise sigma from DB (high frequency noise)
          noise_sigma = m_tileToolNoiseSample->getHfn(drawerIdx, chan, gain);
        }
        
        float significance = 999.999;
        if ((noise_sigma != 0.0) 
            && (noise_sigma < m_maxNoiseSigma)
            /* && (!m_tileBadChanTool->getAdcStatus(drawerIdx, chan, gain).isNoisy()) */) {

          significance = fabs(amp / noise_sigma); // caluclate signal/noise ratio
        } else {
          --ngoodchan[mob]; // ignore completely channels with zero sigma
        }

        ATH_MSG_VERBOSE( "HWID " << m_tileHWID->to_string(adc_id)
                         << " calib " << 1. / calib[chan]
                         << " amp " << amp
                         << " noise " << noise_sigma
                         << " significance " << significance );

        if (significance > m_truncationThresholdOnAbsEinSigma) continue;

        commonmode[mob] += amp;
        ++nemptychan[mob];

      } else {

        ATH_MSG_VERBOSE( "HWID " << m_tileHWID->to_string(adc_id)
                         << " calib " << 1. / calib[chan]
                         << " amp " << amp
                         << " channel is not used" );
      }
      
    }

    int ncorr = 0;
    int nchmin = m_minimumNumberOfTruncatedChannels;

    for (int k = 0; k < maxMOB; k++) {

      if (m_minimumNumberOfTruncatedChannels < 1.0) {
        nchmin = ceil(m_minimumNumberOfTruncatedChannels * ngoodchan[k]);
        if (nchmin < 2) nchmin = 2;
      }

      if (nemptychan[k] >= nchmin) {
        commonmode[k] /= nemptychan[k];
        ++ncorr;

        ATH_MSG_VERBOSE( "ros " << ros
                        << " drawer " << std::setw(2) << drawer
                        << " mb " << k << " mean " << commonmode[k]
                        << " taken from " << nemptychan[k] << " channels"
                        << " nchgood " << ngoodchan[k]
                        << " nchmin " << nchmin );

      } else {
        if (msgLvl(MSG::VERBOSE)) {
          if (commonmode[k] != 0.0) {
            msg(MSG::VERBOSE) << "ros " << ros
                              << " drawer " << std::setw(2) << drawer
                              << " mb " << k
                              << " mean is zero instead of " << commonmode[k] << " / " << nemptychan[k]
                              << " nchgood " << ngoodchan[k]
                              << " nchmin " << nchmin
                              << endmsg;
          } else {
            msg(MSG::VERBOSE) << "ros "
                              << ros << " drawer " << std::setw(2) << drawer
                              << " mb " << k
                              << " mean is zero - nothing to correct"
                              << " nchgood " << ngoodchan[k]
                              << " nchmin " << nchmin
                              << endmsg;
          }
        }
        commonmode[k] = 0.0;
      }
    }

    if (ncorr == 0) continue; // nothing to correct

    // iterate over all channels in a collection again
    for (rchItr = coll->begin(); rchItr != lastRch; ++rchItr) {
      /// FIXME: const_cast
      TileRawChannel* rch = const_cast<TileRawChannel*>(*rchItr);
      int chan = m_tileHWID->channel(rch->adc_HWID());
      int gain = m_tileHWID->adc(rch->adc_HWID());

      // use only good channel and high gain - for them calib was set to non-zero value above
      if (calib[chan] > 0.0 && gain == TileHWID::HIGHGAIN) {
        // correct amplitude directly in channel
        // (will change this to set() method once it is available in TileRawChannel)
        int mob = chan/maxChannel;
        if (undoOnlCalib)
          rch->setAmplitude (rch->amplitude() - commonmode[mob] / calib[chan]);
        else
          rch->setAmplitude (rch->amplitude() - commonmode[mob]);
        rch->setPedestal (rch->pedestal() + commonmode[mob]);
      }
    }
  }

  return StatusCode::SUCCESS;
}

// ============================================================================
// finalize
StatusCode TileRawChannelNoiseFilter::finalize() {
  return StatusCode::SUCCESS;
}

