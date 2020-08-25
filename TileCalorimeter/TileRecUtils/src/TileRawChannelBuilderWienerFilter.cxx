/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TileEvent/TileRawChannel.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

// Gaudi includes
#include "Gaudi/Property.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

// Atlas includes
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"
#include "xAODEventInfo/EventInfo.h"

// Tile includes
#include "TileRecUtils/TileRawChannelBuilderWienerFilter.h"
#include "TileConditions/TilePulseShapes.h"
#include "CLHEP/Matrix/Matrix.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"

//using namespace std;
#include <algorithm>

//interface stuff
static const InterfaceID IID_ITileRawChannelBuilderWienerFilter("TileRawChannelBuilderWienerFilter", 1, 0);


const InterfaceID& TileRawChannelBuilderWienerFilter::interfaceID() {
  return IID_ITileRawChannelBuilderWienerFilter;
}


#define TILE_WienerFilterBUILDERVERBOSE false

TileRawChannelBuilderWienerFilter::TileRawChannelBuilderWienerFilter(const std::string& type,
    const std::string& name, const IInterface *parent)
  : TileRawChannelBuilder(type, name, parent)
  , m_bunchCrossingTool("BunchCrossingTool")
  , m_nSignal(0)
  , m_nNegative(0)
  , m_nCenter(0)
  , m_nConst(0)
  , m_nSamples(0)
  , m_t0SamplePosition(0)
  , m_maxTime(0.0)
  , m_minTime(0.0)
  , m_weights(0)
{
  //declare interfaces
  declareInterface< TileRawChannelBuilder >( this );
  declareInterface< TileRawChannelBuilderWienerFilter >(this);

  m_rawChannelContainerKey = "TileRawChannelWiener";

  //declare properties
  declareProperty("BunchCrossingTool", m_bunchCrossingTool);
  declareProperty("MaxIterations",m_maxIterations = 5);
  declareProperty("PedestalMode",m_pedestalMode = 17);
  declareProperty("TimeForConvergence",m_timeForConvergence = 0.5);
  declareProperty("ConfTB",m_confTB = false);
  declareProperty("Minus1Iteration",m_minus1Iter = false);
  declareProperty("AmplitudeCorrection",m_correctAmplitude = false);
  declareProperty("TimeCorrection", m_correctTimeNI = false);
  declareProperty("BestPhase",m_bestPhase = false);
  declareProperty("EmulateDSP",m_emulateDsp = false);
  declareProperty("MC",m_isMC = false);
}


TileRawChannelBuilderWienerFilter::~TileRawChannelBuilderWienerFilter() {
}

StatusCode TileRawChannelBuilderWienerFilter::initialize() {

  ATH_MSG_INFO( "initialize()" );

  m_rChType = TileFragHash::WienerFilterOffline; // type for offline Wiener Filter

  // init in superclass
  ATH_CHECK( TileRawChannelBuilder::initialize() );

  // get pointer to weights (loaded by the InfoLoader in TileInfo, from TileWienerFilterWeights)
  m_weights = m_tileInfo->getWienerFilterWeights();
  ATH_MSG_DEBUG( "Weights loaded" );

  if (m_maxIterations != 1) m_correctTimeNI = false;

  // bits 12-15 - various options
  // if (m_correctTimeNI)  m_bsflags |= 0x1000;
  if (m_correctAmplitude) m_bsflags |= 0x2000;
  if (m_maxIterations > 1) m_bsflags |= 0x4000;
  if (m_bestPhase) m_bsflags |= 0x8000;

  ATH_MSG_DEBUG( " MaxIterations=" << m_maxIterations
              << " PedestalMode=" << m_pedestalMode
              << " TimeForConvergence=" << m_timeForConvergence
              << " ConfTB=" << m_confTB
              << " Minus1Iteration=" << m_minus1Iter
              << " AmplitudeCorrection=" << m_correctAmplitude
              << " TimeCorrection=" << m_correctTimeNI
              << " Best Phase " << m_bestPhase );

  m_nSamples = m_tileInfo->NdigitSamples();
  m_t0SamplePosition = m_tileInfo->ItrigSample();
  m_maxTime = 25 * (m_nSamples - m_t0SamplePosition - 1);
  m_minTime = -25 * m_t0SamplePosition;
  ATH_MSG_DEBUG(" NSamples=" << m_nSamples
             << " T0Sample=" << m_t0SamplePosition
             << " minTime=" << m_minTime
             << " maxTime=" << m_maxTime );

  if (m_pedestalMode % 10 > 2 && m_nSamples != m_pedestalMode % 10) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Changing PedestalMode from " << m_pedestalMode;
    m_pedestalMode = (m_pedestalMode / 10) * 10 + m_nSamples;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " to " << m_pedestalMode << endmsg;
  }

  if (m_nSamples != 7 && (m_pedestalMode == 71 || m_pedestalMode == 7621)) {
    ATH_MSG_ERROR( "Incompatable pedestal mode [" << m_pedestalMode
                   << "] and number of samples [" << m_nSamples << "]" );
    return StatusCode::FAILURE;
  }

  m_nSignal = 0;
  m_nNegative = 0;
  m_nCenter = 0;
  m_nConst = 0;

  //=== get TileCondToolNoiseSample
  ATH_CHECK( m_tileToolNoiseSample.retrieve() );

  if (m_bestPhase) {
    //=== get TileToolTiming
    // TileToolTiming can be disabled in the TileRawChannelBuilder
    if (!m_tileToolTiming.isEnabled()) {
      m_tileToolTiming.enable();
    }
    ATH_CHECK( m_tileToolTiming.retrieve() );
  }

  //=== get TrigBunchCrossingTool
  if (m_isMC) {
      m_bunchCrossingTool.setTypeAndName("Trig::MCBunchCrossingTool/BunchCrossingTool");
  } else { // is data
      m_bunchCrossingTool.setTypeAndName("Trig::LHCBunchCrossingTool/BunchCrossingTool");
  }
  ATH_CHECK( m_bunchCrossingTool.retrieve() );

  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelBuilderWienerFilter::finalize() {

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Counters: Signal=" << m_nSignal
      << " Constant=" << m_nConst
      << " Total=" << m_nSignal + m_nConst << endmsg;
  }

  ATH_MSG_DEBUG( "Finalizing" );

  return StatusCode::SUCCESS;
}


TileRawChannel * TileRawChannelBuilderWienerFilter::rawChannel(const TileDigits* digits) {

  ++m_chCounter;

  double pedestal = 0.;
  double energy = 0.;
  double time = 0.;
  double chi2 = 0.;
  m_digits = digits->samples();
  m_digits.erase(m_digits.begin(),m_digits.begin()+m_firstSample);
  m_digits.resize(m_nSamples);
  const HWIdentifier adcId = digits->adc_HWID();
  int gain = m_tileHWID->adc(adcId);

  ATH_MSG_VERBOSE( "Building Raw Channel, with WienerFilter, HWID:" << m_tileHWID->to_string(adcId)
                  << " gain=" << gain );

  int ros = m_tileHWID->ros(adcId);
  int drawer = m_tileHWID->drawer(adcId);
  int channel = m_tileHWID->channel(adcId);

  chi2 = filter(ros, drawer, channel, gain, pedestal, energy, time);

  unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

  if (m_calibrateEnergy) {
    energy = m_tileToolEmscale->doCalibCis(drawerIdx, channel, gain, energy);
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Creating WienerFilter RawChannel"
                      << " a=" << energy
                      << " t=" << time
                      << " ped=" << pedestal
                      << " q=" << chi2 << endmsg;

    msg(MSG::VERBOSE) << "digits:";

    for (unsigned int i = 0; i < m_digits.size(); ++i)
      msg(MSG::VERBOSE) << " " << m_digits[i];

    msg(MSG::VERBOSE) << " " << endmsg;
  }

  DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  rawCh->assign (adcId,
                 energy,
                 time,
                 chi2,
                 pedestal);

  if (m_correctTime
      && (time != 0
          && time < m_maxTime
          && time > m_minTime)) {

    time -= m_tileToolTiming->getSignalPhase(drawerIdx, channel, gain);
    rawCh->insertTime(time);

    ATH_MSG_VERBOSE( "Correcting time, new time=" << rawCh->time() );

  }

  if (TileID::HIGHGAIN == gain) {
    ++m_nChH;
    m_RChSumH += energy;
  } else {
    ++m_nChL;
    m_RChSumL += energy;
  }

  return rawCh;
}


int TileRawChannelBuilderWienerFilter::findMaxDigitPosition() {

  ATH_MSG_VERBOSE( "  findMaxDigitPosition()" );

  int iMaxDigit = 0;
  float maxDigit = 0.;
  bool saturated = false;

  for (unsigned int i = 0; i < m_digits.size(); i++) {
    if (m_digits[i] > 1022.99) saturated = true;
    if (maxDigit < m_digits[i]) {
      maxDigit = m_digits[i];
      iMaxDigit = i;
    }
  }

  if (msgLvl(MSG::VERBOSE)) {
    for (unsigned int i = 0; i < m_digits.size(); i++) {
      msg(MSG::VERBOSE) << " " << m_digits[i];
    }

    msg(MSG::VERBOSE) << "; Max: digit[" << iMaxDigit << "]=" << maxDigit << endmsg;

    if (saturated)  msg(MSG::VERBOSE) << " Samples saturated" << endmsg;
  }

  return iMaxDigit;
}


float TileRawChannelBuilderWienerFilter::getPedestal(int ros, int drawer, int channel, int gain) {
  float pedestal = 0.;

  switch (m_pedestalMode) {
    case -1:
      // use pedestal from conditions DB
      pedestal = m_tileToolNoiseSample->getPed(TileCalibUtils::getDrawerIdx(ros, drawer), channel, gain);
      break;
    case 7:
      pedestal = m_digits[6];
      break;
    case 9:
      pedestal = m_digits[8];
      break;
    case 12:
      pedestal = .5 * (m_digits[0] + m_digits[1]);
      break;
    case 17:
      pedestal = .5 * (m_digits[0] + m_digits[6]);
      break;
    case 19:
      pedestal = .5 * (m_digits[0] + m_digits[8]);
      break;
    case 71:
      pedestal = std::min(m_digits[0], m_digits[6]);
      break;
    case 7621:
      pedestal = 0.5 * std::min(m_digits[0] + m_digits[1], m_digits[5] + m_digits[6]);
      break;
    default:
      pedestal = m_digits[0];
      break;
  }

  ATH_MSG_VERBOSE("getPedestal(): pedestal=" << pedestal);

  return pedestal;
}

int TileRawChannelBuilderWienerFilter::getBCIDIndex() {
  int bcidIndex = -1;

  const xAOD::EventInfo* eventInfo(0);

  if (evtStore()->retrieve(eventInfo).isSuccess()) {
    int bcid      = eventInfo->bcid();
    int distFront = m_bunchCrossingTool->distanceFromFront(bcid, Trig::IBunchCrossingTool::BunchCrossings);
    int distTail  = m_bunchCrossingTool->distanceFromTail(bcid, Trig::IBunchCrossingTool::BunchCrossings);

    ATH_MSG_VERBOSE( "EventInfo loaded: "
                  << " BCID=" << bcid
                  << " DistFront=" << distFront
                  << " DistTail=" << distTail);

    if (distFront == -1 || distTail == -1) {
      bcidIndex = -1;
    } else if (distFront < 3 && distTail > distFront) {
      bcidIndex = distFront;
    } else if (distTail < 3 && distTail < distFront) {
      bcidIndex = 6 - distTail;
    } else {
      bcidIndex = 3;
    }
  } else {
    ATH_MSG_VERBOSE("EventInfo not available");
  }

  ATH_MSG_VERBOSE("getBCIDIndex(): bcidIndex=" << bcidIndex);

  return bcidIndex;
}


double TileRawChannelBuilderWienerFilter::filter(int ros, int drawer, int channel
    , int &gain, double &pedestal, double &amplitude, double &time) {

  ATH_MSG_VERBOSE( "filter()" );

  amplitude = 0.;
  time = 0.;
  double chi2 = 0.;

  auto minMaxDigits = std::minmax_element(m_digits.begin(), m_digits.end());
  float minDigit = *minMaxDigits.first;
  float maxDigit = *minMaxDigits.second;

  if (maxDigit - minDigit < 0.01) { // constant value in all samples

    pedestal = minDigit;
    chi2 = 0.;
    ATH_MSG_VERBOSE( "CASE NO SIGNAL: maxdig-mindig = " << maxDigit << "-" << minDigit
        << " = " << maxDigit - minDigit );

    m_nConst++;

  } else {
    double weights[8];
    memset(weights, 0, sizeof(weights));

    int bcidIndex = getBCIDIndex();

    // check if it is E3 or E4 cells and if the BCID is available
    if (ros > 2 && (channel == 0 || channel == 1) && bcidIndex > -1) {
      ATH_MSG_VERBOSE( "Switch to optimal weights");
      for (unsigned int i = 0; i < 8; i++) {
        weights[i] = m_weights->optimalWeights[bcidIndex][i];
      }
    } else {
      ATH_MSG_VERBOSE( "Switch to general weights");
      for (unsigned int i = 0; i < 8; i++) {
        weights[i] = m_weights->generalWeights[ros-1][channel][i];
      }
    }

    // apply filter
    for (unsigned int i = 0; i < m_digits.size(); i++) {
      amplitude += weights[i] * m_digits[i]; // Wiener coefs
    }
    amplitude += weights[7]; // Wiener bias

    double phase = 0.;
    pedestal = getPedestal(ros, drawer, channel, gain);

    chi2 = compute(ros, drawer, channel, gain, pedestal, amplitude, time, phase);

    m_nSignal++;
  }

  return chi2;
}

double TileRawChannelBuilderWienerFilter::compute(int ros, int drawer, int channel, int gain,
    double &pedestal, double &amplitude, double &time, double& phase) {

 ATH_MSG_VERBOSE( "compute();"
               << " ros=" << ros
               << " drawer="  << drawer
               << " channel=" << channel
               << " gain=" << gain
               << " pedestal=" << pedestal
               << " amplitude=" << amplitude
               << " time=" << time
               << " phase=" << phase );

  double chi2 = 0.;
  return chi2;
}
