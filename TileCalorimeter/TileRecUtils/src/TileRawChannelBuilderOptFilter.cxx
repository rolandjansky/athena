/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
//
//     Tilecal Valencia, Ximo.Poveda@cern.ch. June 2007
//
//     TileRawChannelBuilderOptFilter.cxx
//
//     implementation of the Optimal Filtering based on Lagrange multipliers
//       for energy/time reconstruction in TileCal
//
//////////////////////////////////////////////////////////////////////

#include "TileEvent/TileRawChannel.h"

// Gaudi includes
#include "GaudiKernel/Property.h"

// Atlas includes
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"


// Tile includes
#include "TileRecUtils/TileRawChannelBuilderOptFilter.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileOptFilterWeights.h"
#include "TileConditions/TilePulseShapes.h"

#include "CLHEP/Matrix/Matrix.h"


#include <algorithm>

//interface stuff
static const InterfaceID IID_ITileRawChannelBuilderOptFilter("TileRawChannelBuilderOptFilter", 1, 0);

const InterfaceID& TileRawChannelBuilderOptFilter::interfaceID() {
  return IID_ITileRawChannelBuilderOptFilter;
}


#define TILE_OptFilterBUILDERVERBOSE false

TileRawChannelBuilderOptFilter::TileRawChannelBuilderOptFilter(const std::string& type,
    const std::string& name, const IInterface *parent)
  : TileRawChannelBuilder(type, name, parent)
  , m_nSignal(0)
  , m_nNegative(0)
  , m_nCenter(0)
  , m_nSamples(0)
  , m_t0SamplePosition(0)
  , m_maxTime(0.0)
  , m_minTime(0.0)
  , m_pulseShapes(0)
  , m_weights(0)
{
  //declare interfaces
  declareInterface< TileRawChannelBuilder >( this );
  declareInterface< TileRawChannelBuilderOptFilter >(this);

  m_rawChannelContainerKey = "TileRawChannelOpt";

  //declare properties
  declareProperty("MaxIterations", m_maxIterations = 5);
  declareProperty("PedestalMode", m_pedestalMode = 17);
  declareProperty("TimeForConvergence", m_timeForConvergence = 0.5);
  declareProperty("ConfTB", m_confTB = false);
  declareProperty("OF2", m_of2 = true);
  declareProperty("Minus1Iteration", m_minus1Iter = false);
  declareProperty("AmplitudeCorrection", m_correctAmplitude = false);
  declareProperty("TimeCorrection", m_correctTimeNI = false);
}


TileRawChannelBuilderOptFilter::~TileRawChannelBuilderOptFilter() {
}


StatusCode TileRawChannelBuilderOptFilter::initialize() {

  ATH_MSG_INFO( "initialize()" );

  m_rChType = TileFragHash::OptFilterOffline; // type for offline Opt Filter

  // init in superclass
  CHECK( TileRawChannelBuilder::initialize() );

  if (m_maxIterations != 1) m_correctTimeNI = false;

  // bits 12-15 - various options
  // if (m_correctTimeNI)  m_bsflags |= 0x1000;
  if (m_correctAmplitude) m_bsflags |= 0x2000;
  if (m_maxIterations > 1) m_bsflags |= 0x4000;

  ATH_MSG_DEBUG( " MaxIterations=" << m_maxIterations
                 << " PedestalMode=" << m_pedestalMode
                 << " TimeForConvergence=" << m_timeForConvergence
                 << " ConfTB=" << m_confTB
                 << " OF2=" << m_of2
                 << " Minus1Iteration=" << m_minus1Iter
                 << " AmplitudeCorrection=" << m_correctAmplitude 
                 << " TimeCorrection=" << m_correctTimeNI);

  m_nSamples = m_tileInfo->NdigitSamples();
  m_t0SamplePosition = m_tileInfo->ItrigSample();
  m_maxTime = 25 * (m_nSamples - m_t0SamplePosition - 1);
  m_minTime = -25 * m_t0SamplePosition;
  ATH_MSG_DEBUG( " NSamples=" << m_nSamples
                << " T0Sample=" << m_t0SamplePosition
                << " minTime=" << m_minTime
                << " maxTime=" << m_maxTime );

  if (m_pedestalMode % 10 > 2 && m_nSamples != m_pedestalMode % 10) {
    int oldPedestalMode(m_pedestalMode);
    m_pedestalMode = (m_pedestalMode / 10) * 10 + m_nSamples;
    ATH_MSG_DEBUG( "Changing PedestalMode from " << oldPedestalMode
                  << " to " << m_pedestalMode );
  }

  //get pointer to pulse shapes build pulse shapes)
  m_pulseShapes = m_tileInfo->getPulseShapes();

  std::vector<double> pulseShapeYCisLG = m_pulseShapes->m_ylcis;
  std::vector<double> pulseShapeTCisLG = m_pulseShapes->m_tlcis;
  std::vector<double> pulseShapeYCisHG = m_pulseShapes->m_yhcis;
  std::vector<double> pulseShapeTCisHG = m_pulseShapes->m_thcis;

  std::vector<double> pulseShapeYPhysLG = m_pulseShapes->m_ylphys;
  std::vector<double> pulseShapeTPhysLG = m_pulseShapes->m_tlphys;
  std::vector<double> pulseShapeYPhysHG = m_pulseShapes->m_yhphys;
  std::vector<double> pulseShapeTPhysHG = m_pulseShapes->m_thphys;

  std::vector<double> pulseShapeDerivativeYCisLG = m_pulseShapes->m_ydlcis;
  std::vector<double> pulseShapeDerivativeTCisLG = m_pulseShapes->m_tdlcis;
  std::vector<double> pulseShapeDerivativeYCisHG = m_pulseShapes->m_ydhcis;
  std::vector<double> pulseShapeDerivativeTCisHG = m_pulseShapes->m_tdhcis;

  std::vector<double> pulseShapeDerivativeYPhysLG = m_pulseShapes->m_ydlphys;
  std::vector<double> pulseShapeDerivativeTPhysLG = m_pulseShapes->m_tdlphys;
  std::vector<double> pulseShapeDerivativeYPhysHG = m_pulseShapes->m_ydhphys;
  std::vector<double> pulseShapeDerivativeTPhysHG = m_pulseShapes->m_tdhphys;

  if (msgLvl(MSG::DEBUG)) {

    msg(MSG::DEBUG) << " pulseShapeYCisLG.size()=" << pulseShapeYCisLG.size()
                    << " pulseShapeYCisHG.size()=" << pulseShapeYCisHG.size()
                    << " pulseShapeYPhysLG.size()=" << pulseShapeYPhysLG.size()
                    << " pulseShapeYPhysHG.size()=" << pulseShapeYPhysHG.size()
                    << " pulseShapeDerivativeYCisLG.size()=" << pulseShapeDerivativeYCisLG.size()
                    << " pulseShapeDerivativeYCisHG.size()=" << pulseShapeDerivativeYCisHG.size()
                    << " pulseShapeDerivativeYPhysLG.size()=" << pulseShapeDerivativeYPhysLG.size()
                    << " pulseShapeDerivativeYPhysHG.size()=" << pulseShapeDerivativeYPhysHG.size() << endmsg;


    int N = 0;
    for (unsigned int i = 0; i < pulseShapeYCisLG.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << pulseShapeTCisLG[i] << std::setw(8) << std::setprecision(4)
                      << pulseShapeYCisLG[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < pulseShapeYCisHG.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << pulseShapeTCisHG[i] << std::setw(8) << std::setprecision(4)
                      << pulseShapeYCisHG[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < pulseShapeYPhysLG.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << pulseShapeTPhysLG[i] << std::setw(8) << std::setprecision(4)
                      << pulseShapeYPhysLG[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < pulseShapeYPhysHG.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << pulseShapeTPhysHG[i] << std::setw(8) << std::setprecision(4)
                      << pulseShapeYPhysHG[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < pulseShapeDerivativeYCisLG.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                    << pulseShapeDerivativeTCisLG[i] << std::setw(8) << std::setprecision(4)
                    << pulseShapeDerivativeYCisLG[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < pulseShapeDerivativeYCisHG.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << pulseShapeDerivativeTCisHG[i] << std::setw(8) << std::setprecision(4)
                      << pulseShapeDerivativeYCisHG[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < pulseShapeDerivativeYPhysLG.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << pulseShapeDerivativeTPhysLG[i] << std::setw(8) << std::setprecision(4)
                      << pulseShapeDerivativeYPhysLG[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < pulseShapeDerivativeYPhysHG.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << pulseShapeDerivativeTPhysHG[i] << std::setw(8) << std::setprecision(4)
                      << pulseShapeDerivativeYPhysHG[i] << endmsg;
  }

  //Build Pulse Shape (use the one in TileCalibAlgs?)
  buildPulseShape(m_pulseShapeCisLG, pulseShapeYCisLG, pulseShapeTCisLG, 9);
  buildPulseShape(m_pulseShapeCisHG, pulseShapeYCisHG, pulseShapeTCisHG, 9);
  buildPulseShape(m_pulseShapePhysLG, pulseShapeYPhysLG, pulseShapeTPhysLG, 9);
  buildPulseShape(m_pulseShapePhysHG, pulseShapeYPhysHG, pulseShapeTPhysHG, 9);
  buildPulseShape(m_pulseShapeDerivativeCisLG, pulseShapeDerivativeYCisLG, pulseShapeDerivativeTCisLG, 9);
  buildPulseShape(m_pulseShapeDerivativeCisHG, pulseShapeDerivativeYCisHG, pulseShapeDerivativeTCisHG, 9);
  buildPulseShape(m_pulseShapeDerivativePhysLG, pulseShapeDerivativeYPhysLG, pulseShapeDerivativeTPhysLG, 9);
  buildPulseShape(m_pulseShapeDerivativePhysHG, pulseShapeDerivativeYPhysHG, pulseShapeDerivativeTPhysHG, 9);

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "m_pulseShapeCisLG.size()=" << m_pulseShapeCisLG.size() << endmsg;
    for (unsigned int i = 0; i < m_pulseShapeCisLG.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_pulseShapeCisLG[i] << endmsg;

    msg(MSG::DEBUG) << "m_pulseShapeCisHG.size()=" << m_pulseShapeCisHG.size() << endmsg;
    for (unsigned int i = 0; i < m_pulseShapeCisHG.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_pulseShapeCisHG[i] << endmsg;

    msg(MSG::DEBUG) << "m_pulseShapePhysLG.size()=" << m_pulseShapePhysLG.size() << endmsg;
    for (unsigned int i = 0; i < m_pulseShapePhysLG.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_pulseShapePhysLG[i] << endmsg;

    msg(MSG::DEBUG) << "m_pulseShapePhysHG.size()=" << m_pulseShapePhysHG.size() << endmsg;
    for (unsigned int i = 0; i < m_pulseShapePhysHG.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_pulseShapePhysHG[i] << endmsg;

    msg(MSG::DEBUG) << "m_pulseShapeDerivativeCisLG.size()=" << m_pulseShapeDerivativeCisLG.size() << endmsg;
    for (unsigned int i = 0; i < m_pulseShapeDerivativeCisLG.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_pulseShapeDerivativeCisLG[i] << endmsg;

    msg(MSG::DEBUG) << "m_pulseShapeDerivativeCisHG.size()=" << m_pulseShapeDerivativeCisHG.size() << endmsg;
    for (unsigned int i = 0; i < m_pulseShapeDerivativeCisHG.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_pulseShapeDerivativeCisHG[i] << endmsg;

    msg(MSG::DEBUG) << "m_pulseShapeDerivativePhysLG.size()=" << m_pulseShapeDerivativePhysLG.size() << endmsg;
    for (unsigned int i = 0; i < m_pulseShapeDerivativePhysLG.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_pulseShapeDerivativePhysLG[i] << endmsg;

    msg(MSG::DEBUG) << "m_pulseShapeDerivativePhysHG.size()=" << m_pulseShapeDerivativePhysHG.size() << endmsg;
    for (unsigned int i = 0; i < m_pulseShapeDerivativePhysHG.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_pulseShapeDerivativePhysHG[i] << endmsg;
  }

  //get pointer to weights (loaded by the InfoLoader in TileInfo, from TileOptFilterWeights)
  m_weights = m_tileInfo->getOptFilterWeights();
  ATH_MSG_DEBUG("Weights loaded" );

  m_nSignal = 0;
  m_nNegative = 0;
  m_nCenter = 0;

  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelBuilderOptFilter::finalize() {

  ATH_MSG_VERBOSE( "Counters: Signal=" << m_nSignal
                  << " Negat=" << m_nNegative
                  << " Center=" << m_nCenter );

  ATH_MSG_DEBUG( "Finalizing" );

  return StatusCode::SUCCESS;
}


TileRawChannel * TileRawChannelBuilderOptFilter::rawChannel(const TileDigits* digits) {
  ++m_chCounter;

  double pedestal = 0.;
  double amplitude = 0.;
  double time = 0.;
  double chi2 = 0.;
  m_digits = digits->samples();
  const HWIdentifier adcId = digits->adc_HWID();
  int gain = m_tileHWID->adc(adcId);

  ATH_MSG_VERBOSE( "Building Raw Channel, with OptFilter, HWID:" << m_tileHWID->to_string(adcId)
                  << " gain=" << gain );

  if (m_confTB) {  // Using old weights for CTB
    int ros = m_tileHWID->ros(adcId);
    int drawer = m_tileHWID->drawer(adcId);
    int channel = m_tileHWID->channel(adcId);
    chi2 = filter(ros, drawer, channel, gain, pedestal, amplitude, time);

  } else {  // Using weights obtained with delta correlation
    chi2 = filter(1000, 1000, 1000, gain, pedestal, amplitude, time);
  }

  if (m_calibrateEnergy) {
    amplitude = m_tileInfo->CisCalib(adcId, amplitude);
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Creating OptFilter RawChannel a=" << amplitude
                      << " t=" << time
                      << " ped=" << pedestal
                      << " q=" << chi2 << endmsg;

    msg(MSG::VERBOSE) << "digits:";
    for (unsigned int i = 0; i < m_digits.size(); ++i)
      msg(MSG::VERBOSE) << " " << m_digits[i];

    msg(MSG::VERBOSE) << " " << endmsg;
  }

  // return new TileRawChannel
  // TileRawChannel *rawCh = new TileRawChannel(adcId,OptFilterEne,OptFilterTime,OptFilterChi2,OptFilterPed);

  DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  rawCh->assign (adcId,
                 amplitude,
                 time,
                 chi2,
                 pedestal);

  if (m_correctTime
      && (time != 0 && time < m_maxTime && time > m_minTime)) {

    rawCh->insertTime(m_tileInfo->TimeCalib(adcId, time));
    ATH_MSG_VERBOSE( "Correcting time, new time=" << rawCh->time() );

  }

  if (TileID::HIGHGAIN == gain) {
    ++m_nChH;
    m_RChSumH += amplitude;
  } else {
    ++m_nChL;
    m_RChSumL += amplitude;
  }

  return rawCh;
}


int TileRawChannelBuilderOptFilter::findMaxDigitPosition() {

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
    for (float digit : m_digits) {
      msg(MSG::VERBOSE) << " " << digit;
    }

    msg(MSG::VERBOSE) << "; Max: digit[" << iMaxDigit << "]=" << maxDigit << endmsg;

    if (saturated) msg(MSG::VERBOSE) << " Samples saturated" << endmsg;
  }

  return iMaxDigit;
}

float TileRawChannelBuilderOptFilter::findMaxDigitsDifference() {

  auto minMaxDigits = std::minmax_element(m_digits.begin(), m_digits.end());

  ATH_MSG_VERBOSE("  findMaxDigitsDifference(): maxDigit-leastDigit=" << *minMaxDigits.second - *minMaxDigits.first);

  return *minMaxDigits.second - *minMaxDigits.first;

}


float TileRawChannelBuilderOptFilter::findMaxDigit() {

  auto maxDigit = std::max_element(m_digits.begin(), m_digits.end());

  ATH_MSG_VERBOSE("findMaxDigit(): maxDigit=" << *maxDigit);

  return *maxDigit;

}


bool TileRawChannelBuilderOptFilter::are3FF() {
  bool allSaturated = true;

  for (float digit : m_digits) {
    allSaturated = allSaturated && (digit > 1022.99);
  }

  ATH_MSG_VERBOSE("are3FF(): allSaturated=" << allSaturated);

  return allSaturated;
}


float TileRawChannelBuilderOptFilter::getPedestal() {
  float pedestal = 0.;

  switch (m_pedestalMode) {
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
    default:
      pedestal = m_digits[0];
      break;
  }

  ATH_MSG_VERBOSE("getPedestal(): pedestal=" << pedestal);

  return pedestal;
}


double TileRawChannelBuilderOptFilter::filter(int ros, int drawer, int channel
    , int &gain, double &pedestal, double &amplitude, double &time) {

  ATH_MSG_VERBOSE( "Filter" );

  pedestal = getPedestal();
  amplitude = 0.;
  time = 0.;
  int phase = 0;
  double chi2 = 0.;
  int /* imaxdig=FindMaxDigit(), */nIterations = 0;
  int digits_size = m_digits.size();
  float digDiff = findMaxDigitsDifference();
  float maxDigit = findMaxDigit();
  float minDigit = maxDigit - digDiff;

  // values used for pedestal-like events when iterations are performed
  int sigma;
  int sigma_hi = 5;
  int sigma_lo = 3;

  if (gain == 0) sigma = sigma_lo;
  else sigma = sigma_hi;

  if (are3FF()) {

    gain = 0;
    pedestal = 0.;
    amplitude = 0.;
    time = 0.;
    if (digits_size > 0 && m_digits[0] > 2046.99)
      chi2 = 9999.;
    else
      chi2 = 0.;

  } else {

    if (m_maxIterations == 1) {  // Without iterations
      chi2 = compute(ros, drawer, channel, gain, pedestal, amplitude, time, phase);

      // If weights for tau=0 are used, deviations are seen in the amplitude =>
      // function to correct the amplitude
      if (m_correctAmplitude
          && amplitude > m_ampMinThresh
          && time > m_timeMinThresh
          && time < m_timeMaxThresh) {

        amplitude *= correctAmp(time, m_of2);
        ATH_MSG_VERBOSE( "Amlitude corrected by " << correctAmp(time, m_of2)
                        << " new amplitude is " << amplitude );
      }

      if (m_correctTimeNI ) {
        ATH_MSG_VERBOSE( "Time " << time 
                         << " is corrected by " << correctTime(time, m_of2)
                         << ", new time is " << time + correctTime(time, m_of2));

        time += correctTime(time, m_of2);
      }


      if (time > m_maxTime) time = m_maxTime;
      if (time < m_minTime) time = m_minTime;

    } else { // With iterations => 3 cases defined for correct pedestal treatment

      // Signal events: OF with iterations
      if ((maxDigit - m_digits[0] > sigma)
          || (m_digits[0] - m_digits[digits_size - 1] > 4 * sigma)) {

        ATH_MSG_VERBOSE( "CASE Signal: maxdig-OptFilterDigits[0]="
                        << maxDigit - m_digits[0]
                        << "   OptFilterDigits[0]-OptFilterDigits[ digits_size-1]="
                        << m_digits[0] - m_digits[digits_size - 1] );

        nIterations = iterate(ros, drawer, channel, gain, pedestal, amplitude, time, chi2);

        ATH_MSG_VERBOSE( "number of iterations= " << nIterations );

        m_nSignal++;
      } else if (m_digits[0] - minDigit > sigma) { //Pedestal events: OF with negative iterations

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "CASE NEGATIVE: maxdig-OptFilterDigits[0]="
                            << maxDigit - m_digits[0]
                            << "   OptFilterDigits[0]-OptFilterDigits[digits_size-1]="
                            << m_digits[0] - m_digits[digits_size - 1] << endmsg;

          msg(MSG::VERBOSE) << "OptFilterDigits[0]-mindig="
                            << m_digits[0] - minDigit << endmsg;
        }

        for (int i = 0; i < digits_size; i++)  // Mirror around pedestal
          m_digits[i] = pedestal - (m_digits[i] - pedestal);

        nIterations = iterate(ros, drawer, channel, gain, pedestal, amplitude, time, chi2);

        amplitude = -amplitude;

        m_nNegative++;

      } else { // Gaussian center: no iterations and phase=0

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "CASE CENTER: maxdig-OptFilterDigits[0]="
                            << maxDigit - m_digits[0]
                            << "   OptFilterDigits[0]-OptFilterDigits[digits_size-1]="
                            << m_digits[0] - m_digits[digits_size - 1] << endmsg;

          msg(MSG::VERBOSE) << "OptFilterDigits[0]-mindig="
                            << m_digits[0] - minDigit << endmsg;
        }
        //	OptFilterTime=-100.;

        chi2 = compute(ros, drawer, channel, gain, pedestal, amplitude, time, phase);

        time = 0.;
        ++m_nCenter;

      }

    }

  }

  return chi2;
}


int TileRawChannelBuilderOptFilter::iterate(int ros, int drawer, int channel, int gain,
    double &pedestal, double &amplitude, double &time, double &chi2) {

  ATH_MSG_VERBOSE( "iterate()" );

  int nIterations = 0;
  double savePhase = 0.0;
  double phase = 0.0;
  time = -1000.;

  // Mythic -1 iteration
  if (m_minus1Iter) phase = 25 * (m_t0SamplePosition - findMaxDigitPosition());

  while ((time > m_timeForConvergence
          || time < (-1.) * m_timeForConvergence)
        && nIterations < m_maxIterations) {

    chi2 = compute(ros, drawer, channel, gain, pedestal, amplitude, time, (int) phase);

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << " OptFilter computed with phase=" << phase
                        << " Time=" << time;
    }

    savePhase = phase;

    phase -= round(time);
    if (phase > m_maxTime) phase = m_maxTime;
    if (phase < m_minTime) phase = m_minTime;

    ++nIterations;
    if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << " END ITER=" << nIterations
                            << " new phase=" << phase
                            << " chi2=" << chi2
                            << "  Amp=" << amplitude << endmsg;
    }
  }

  time -= savePhase;
  if (time > m_maxTime) time = m_maxTime;
  if (time < m_minTime) time = m_minTime;

  ATH_MSG_VERBOSE( "OptFilterEne=" << amplitude
                  << " Phase=" << savePhase
                  << " Absolute Time=" << time );

  return nIterations;
}


double TileRawChannelBuilderOptFilter::compute(int ros, int drawer, int channel, int gain,
    double &pedestal, double &amplitude, double &time, int phase) {

  ATH_MSG_VERBOSE( "compute(); ros=" << ros
                  << " drawer=" << drawer
                  << " channel=" << channel
                  << " gain=" << gain );

  int i = 0;
  int digitsSize = m_digits.size();
  double chi2 = 0.;
  double a[9];
  double b[9];
  double c[9];
  double g[9];

  amplitude = 0.;
  time = 0.;

  if (m_idocis) {
    for (i = 0; i < 7; ++i) { // 7 samples only
      a[i] = m_weights->a_cis_simp[gain][i][phase + 100];
      b[i] = m_weights->b_cis_simp[gain][i][phase + 100];
      if (m_of2) c[i] = m_weights->c_cis_simp[gain][i][phase + 100];
    }
  } else {
    for (i = 0; i < digitsSize; ++i) { // 7 or 9 samples
      a[i] = m_weights->a_phys_simp[gain][i][phase + 100];
      b[i] = m_weights->b_phys_simp[gain][i][phase + 100];
      if (m_of2) c[i] = m_weights->c_phys_simp[gain][i][phase + 100];
    }
  }

  if (m_idocis) {
    if (gain == 0) {
      for (i = 0; i < digitsSize; ++i) {
        g[i] = m_pulseShapeCisLG[(i - (digitsSize - 9) / 2) * 25 + 100 + phase];
      }
    } else {
      for (i = 0; i < digitsSize; ++i) {
        g[i] = m_pulseShapeCisHG[(i - (digitsSize - 9) / 2) * 25 + 100 + phase];
      }
    }
  } else if (gain == 0) {
    for (i = 0; i < digitsSize; ++i) {
      g[i] = m_pulseShapePhysLG[(i - (digitsSize - 9) / 2) * 25 + 100 + phase];
    }
  } else {
    for (i = 0; i < digitsSize; ++i) {
      g[i] = m_pulseShapePhysHG[(i - (digitsSize - 9) / 2) * 25 + 100 + phase];
    }
  }

  ATH_MSG_VERBOSE( "OptFilterPha=" << phase << ((m_idocis) ? " cis" : " ") );

  if (m_of2) {
    pedestal = 0.;
    for (i = 0; i < digitsSize; ++i) {
      amplitude += a[i] * m_digits[i];
      time += b[i] * m_digits[i];
      pedestal += c[i] * m_digits[i];
    }
  } else {
    for (i = 0; i < digitsSize; ++i) {
      amplitude += a[i] * (m_digits[i] - pedestal);
      time += b[i] * (m_digits[i] - pedestal);
    }
  }

  bool goodEne = (fabs(amplitude) > 1.0e-04);
  if (goodEne) {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterEne=" << amplitude << endmsg;
      msg(MSG::VERBOSE) << "OptFilterTime*OptFilterEne=" << time << endmsg;
    }
    time /= amplitude;
  } else {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterEne=" << amplitude
                        << "   ... assuming 0.0" << endmsg;
      msg(MSG::VERBOSE) << "OptFilterTime*OptFilterEne=" << time
                        << "   ... assuming 0.0" << endmsg;
    }
    time = amplitude = 0.0;
  }

  for (i = 0; i < digitsSize; ++i) {
    if (m_digits[i] > 0)
      chi2 += 50 * fabs(m_digits[i] - (amplitude * g[i] + pedestal)) / m_digits[i];
    else
      chi2 += 50 * fabs(m_digits[i] - (amplitude * g[i] + pedestal)) / 1024.;
  }

  if (fabs(chi2) > 1.0e-04 || goodEne) {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterTime=" << time << endmsg;
      msg(MSG::VERBOSE) << "OptFilterPed=" << pedestal << endmsg;
      msg(MSG::VERBOSE) << "OptFilterChi2=" << chi2 << endmsg;
    }
  } else {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterTime=" << time << endmsg;
      msg(MSG::VERBOSE) << "OptFilterPed=" << pedestal << endmsg;
      msg(MSG::VERBOSE) << "OptFilterChi2=" << chi2
                        << "   ... assuming 0.0" << endmsg;
    }
    chi2 = 0.0;
  }

  return chi2;
}


void TileRawChannelBuilderOptFilter::buildPulseShape(std::vector<double> &pulseShape,
    std::vector<double> &pulseShapeY, std::vector<double> &pulseShapeT, int dignum) {

  ATH_MSG_DEBUG( "buildPulseShape");

//1: set m_pulseShape
  int shape_size = (dignum - 1) * 25 + 200;
  pulseShape.resize(shape_size);
  ATH_MSG_DEBUG( "Set dimension of m_pulseShape to shape_sie=" << shape_size);

//2: scan m_pulseShapeT for: tmin, tmax, nt0 and size: m_pulseShapeX[nt0]=1.0;
  int nt0 = 0;
  int size = pulseShapeT.size();;
  double tmin = 10000.;
  double tmax = -10000.;

  for (int i = 0; i < size; i++) {
    if (pulseShapeT[i] < tmin) tmin = pulseShapeT[i];
    if (pulseShapeT[i] > tmax) tmax = pulseShapeT[i];
    if (pulseShapeT[i] == 0) nt0 = i;
  }

  ATH_MSG_DEBUG( "m_pulseShapeY & m_pulseShapeT size =" << size
                << ", tmin=" << tmin
                << ", tmax=" << tmax
                << "  nt0=" << nt0
                << " m_pulseShapeT[nt0]=" << pulseShapeT[nt0]
                << " m_pulseShapeY[nt0]=" << pulseShapeY[nt0] );

//3: fill m_pulseShape
  bool exact;
  int nminn, nminp;
  double minn, minp, tdist;
  pulseShape[(shape_size) / 2] = pulseShapeY[nt0];
//  for (int i=1;i<(shape_size)/2+1;i++){
  for (int i = 1; i < (shape_size) / 2; i++) {
    // negative times: 0->(shape_size-1)/2
    if (-i < tmin)
      pulseShape[(shape_size) / 2 - i] = 0.;
    else {
      exact = false;
      minn = -10000.;
      minp = 10000.;
      nminn = 0;
      nminp = size - 1;
      for (int j = 0; j < nt0 + 1 && !exact; j++) {
        if (pulseShapeT[j] == double(-i)) {
          pulseShape[(shape_size) / 2 - i] = pulseShapeY[j];
          exact = true;
        } else {
          tdist = pulseShapeT[j] - double(-i);
          if (tdist < 0. && tdist > minn) {
            minn = tdist;
            nminn = j;
          }
          if (tdist > 0. && tdist < minp) {
            minp = tdist;
            nminp = j;
          }
        }
      }

      if (exact) {
        ATH_MSG_VERBOSE( "exact value found for time=" << -i
                        << " m_pulseShape=" << pulseShape[(shape_size) / 2 - i]);
      } else {
        ATH_MSG_VERBOSE( "exact value NOT found for time=" << -i
                        << " nminn=" << nminn
                        << " m_pulseShapeT="
                        << pulseShapeT[nminn]
                        << " m_pulseShapeY=" << pulseShapeY[nminn] << std::endl
                        << " nminp=" << nminp
                        << " m_pulseShapeT="
                        << pulseShapeT[nminp]
                        << " m_pulseShapeY=" << pulseShapeY[nminp]);

        pulseShape[(shape_size) / 2 - i] = pulseShapeY[nminn]
            + (pulseShapeY[nminp] - pulseShapeY[nminn])
                / (pulseShapeT[nminp] - pulseShapeT[nminn]) * (-i - pulseShapeT[nminn]);
      }

    }

    // positive times: (shape_size-1)/2->shape_size
    if (i > tmax)
      pulseShape[(shape_size) / 2 + i] = 0.;
    else {
      exact = false;
      minn = -10000.;
      minp = 10000.;
      nminn = 0;
      nminp = size;
      for (int j = nt0; j < size && !exact; j++) {
        if (pulseShapeT[j] == double(i)) {
          pulseShape[(shape_size) / 2 + i] = pulseShapeY[j];
          exact = true;
        } else {
          tdist = pulseShapeT[j] - double(i);
          if (tdist < 0) if (tdist > minn) {
            minn = tdist;
            nminn = j;
          }
          if (tdist > 0) if (tdist < minp) {
            minp = tdist;
            nminp = j;
          }
        }
      }
      if (exact) {
        ATH_MSG_VERBOSE( "exact value found for time=" << i
                        << " m_pulseShape=" << pulseShape[(shape_size) / 2 + i]);
      } else {
        ATH_MSG_VERBOSE( "exact value NOT found for time=" << i
                        << " nminn=" << nminn
                        << " m_pulseShapeT=" << pulseShapeT[nminn]
                        << " m_pulseShapeY=" << pulseShapeY[nminn] << std::endl
                        << " nminp=" << nminp
                        << " m_pulseShapeT=" << pulseShapeT[nminp]
                        << " m_pulseShapeY=" << pulseShapeY[nminp] );

        pulseShape[(shape_size) / 2 + i] = pulseShapeY[nminn]
            + (pulseShapeY[nminp] - pulseShapeY[nminn])
                / (pulseShapeT[nminp] - pulseShapeT[nminn]) * (i - pulseShapeT[nminn]);
      }
    }
  }

//   if (msgLvl(MSG::VERBOSE)) {
//    for (int i = 1; i < shape_size; i++) {
//      msg(MSG::VERBOSE) << "shape[" << i << "]=" << m_pulseShape[i] << endmsg;
//    }
//  }
}

