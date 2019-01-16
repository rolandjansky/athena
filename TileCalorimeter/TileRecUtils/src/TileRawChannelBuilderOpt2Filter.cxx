/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


//////////////////////////////////////////////////////////////////////
//
//     Base on the code of Ximo Poveda.
//     Andrei.Artamonov@cern.ch, July 2008
//
//     TileRawChannelBuilderOpt2Filter.cxx
//
//     implementation of the Optimal Filtering based on Lagrange multipliers
//       for energy/time reconstruction in TileCal 
//
//////////////////////////////////////////////////////////////////////

// Tile includes
#include "TileEvent/TileRawChannel.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileRecUtils/TileRawChannelBuilderOpt2Filter.h"
#include "TileRecUtils/TileRawChannelBuilderOpt2FilterLookup.h"
#include "TileConditions/TileOptFilterWeights.h"
#include "TileConditions/TilePulseShapes.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileOptFilterWeights.h"

// Atlas includes
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"

// Gaudi includes
#include "GaudiKernel/Property.h"

#include "CLHEP/Matrix/Matrix.h"
//using namespace std;
#include <algorithm>

//interface stuff
static const InterfaceID IID_ITileRawChannelBuilderOpt2Filter("TileRawChannelBuilderOpt2Filter", 1, 0);


const InterfaceID& TileRawChannelBuilderOpt2Filter::interfaceID() {
  return IID_ITileRawChannelBuilderOpt2Filter;
}


#define TILE_Opt2FilterBUILDERVERBOSE false

TileRawChannelBuilderOpt2Filter::TileRawChannelBuilderOpt2Filter(const std::string& type,
    const std::string& name, const IInterface *parent)
  : TileRawChannelBuilder(type, name, parent)
  , m_nSignal(0)
  , m_nNegative(0)
  , m_nCenter(0)
  , m_nConst(0)
  , m_nSamples(0)
  , m_t0SamplePosition(0)
{
  //declare interfaces
  declareInterface< TileRawChannelBuilder >( this );
  declareInterface< TileRawChannelBuilderOpt2Filter >(this);

  m_rawChannelContainerKey = "TileRawChannelOpt2";
  
  //declare properties
  declareProperty("MaxIterations", m_maxIterations = 5);
  declareProperty("PedestalMode", m_pedestalMode = 17);
  declareProperty("TimeForConvergence", m_timeForConvergence = 0.5);
  declareProperty("ConfTB", m_confTB = false);
  declareProperty("OF2", m_of2 = true);
  declareProperty("Minus1Iteration", m_minus1Iter = false);
  declareProperty("AmplitudeCorrection", m_correctAmplitude = false);
  declareProperty("TimeCorrection", m_correctTimeNI = false);
  declareProperty("BestPhase", m_bestPhase = false);
  declareProperty("EmulateDSP", m_emulateDsp = false);
  declareProperty("NoiseThresholdHG", m_noiseThresholdHG = 5);
  declareProperty("NoiseThresholdLG", m_noiseThresholdLG = 3);
  declareProperty("MinTime", m_minTime =  0.0);
  declareProperty("MaxTime", m_maxTime = -1.0);
}


TileRawChannelBuilderOpt2Filter::~TileRawChannelBuilderOpt2Filter() {
}


StatusCode TileRawChannelBuilderOpt2Filter::initialize() {

  ATH_MSG_INFO( "initialize()" );

  m_rChType = TileFragHash::OptFilterOffline; // type for offline Opt Filter

  // init in superclass
  ATH_CHECK( TileRawChannelBuilder::initialize() );

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
                 << " OF2=" << m_of2
                 << " Minus1Iteration=" << m_minus1Iter
                 << " AmplitudeCorrection=" << m_correctAmplitude
                 << " TimeCorrection=" << m_correctTimeNI
                 << " Best Phase " << m_bestPhase );

  ATH_MSG_DEBUG( " NoiseThresholdHG=" << m_noiseThresholdHG
                 << " NoiseThresholdLG=" << m_noiseThresholdLG);

  m_nSamples = m_tileInfo->NdigitSamples();
  m_t0SamplePosition = m_tileInfo->ItrigSample();
  if (m_maxTime < m_minTime) { // set time window if it was not set from jobOptions
    m_maxTime = 25 * (m_nSamples - m_t0SamplePosition - 1);
    m_minTime = -25 * m_t0SamplePosition;
  }
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

  //=== get TileCondToolOfc
  ATH_CHECK( m_tileCondToolOfc.retrieve() );
  
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

  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelBuilderOpt2Filter::finalize() {

  if (msgLvl(MSG::VERBOSE)) {
    if (m_maxIterations == 1) { // Without iterations
      msg(MSG::VERBOSE) << "Counters: Signal=" << m_nSignal
                        << " Constant=" << m_nConst
                        << " Total=" << m_nSignal + m_nConst << endmsg;
    } else {
      msg(MSG::VERBOSE) << "Counters: Signal=" << m_nSignal
                        << " Negat=" << m_nNegative
                        << " Center=" << m_nCenter
                        << " Constant=" << m_nConst
                        << " Total=" << m_nSignal + m_nNegative + m_nConst + m_nCenter << endmsg;
    }
  }

  ATH_MSG_DEBUG( "Finalizing" );

  return StatusCode::SUCCESS;
}


TileRawChannel * TileRawChannelBuilderOpt2Filter::rawChannel(const TileDigits* digits) {

  ++m_chCounter;

  double pedestal = 0.;
  double energy = 0.;
  double time = 0.;
  double chi2 = 0.;
  m_digits = digits->samples();
  const HWIdentifier adcId = digits->adc_HWID();
  int gain = m_tileHWID->adc(adcId);
  
  ATH_MSG_VERBOSE( "Building Raw Channel, with OptFilter, HWID:" << m_tileHWID->to_string(adcId)
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
    msg(MSG::VERBOSE) << "Creating OptFilter RawChannel"
                      << " a=" << energy
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


int TileRawChannelBuilderOpt2Filter::findMaxDigitPosition() {

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


float TileRawChannelBuilderOpt2Filter::getPedestal(int ros, int drawer, int channel, int gain) {
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


double TileRawChannelBuilderOpt2Filter::filter(int ros, int drawer, int channel
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

    pedestal = getPedestal(ros, drawer, channel, gain);
    double phase = 0.;
    int nIterations = 0;

    if (m_maxIterations == 1) {  // Without iterations
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      // AA 3.10.08 --- take best phase from COOL
      if (m_bestPhase) {
        // AS 19.11.09 - note minus sign here - time in DB is opposite to best phase 
        phase = -m_tileToolTiming->getSignalPhase(drawerIdx, channel, gain);
        ATH_MSG_VERBOSE( "Best phase: " << phase
                        << " drawerIdx " << drawerIdx
                        << " channel " << channel );
      }
      
      double ofcPhase(phase);
      chi2 = compute(ros, drawer, channel, gain, pedestal, amplitude, time, ofcPhase);

      // If weights for tau=0 are used, deviations are seen in the amplitude =>
      // function to correct the amplitude
      if (m_correctAmplitude
          && amplitude > m_ampMinThresh
          && time > m_timeMinThresh
          && time < m_timeMaxThresh) {

        amplitude *= correctAmp(time, m_of2);
        ATH_MSG_VERBOSE( "Amplitude corrected by " << correctAmp(time, m_of2)
                         << " new amplitude is " << amplitude );
      }

      if (m_correctTimeNI ) {
        ATH_MSG_VERBOSE( "Time " << time 
                         << " is corrected by " << correctTime(time, m_of2)
                         << ", new time is " << time + correctTime(time, m_of2));

        time += correctTime(time, m_of2);
      }

      time += (phase - ofcPhase); // correct time if actual phase used in the calculation is different from required

      if (time > m_maxTime) time = m_maxTime;
      if (time < m_minTime) time = m_minTime;

      m_nSignal++;

    } else { // With iterations => 3 cases defined for correct pedestal treatment

      // values used for pedestal-like events when iterations are performed
      //const int sigma_hi = 5;
      //const int sigma_lo = 3;
      int sigma = (gain) ? m_noiseThresholdHG : m_noiseThresholdLG;
      int digits_size_1 = m_digits.size() - 1;

      // Signal events: OF with iterations
      if ((maxDigit - m_digits[0] > sigma)
          || (m_digits[0] - m_digits[digits_size_1] > 4 * sigma)) {

        ATH_MSG_VERBOSE( "CASE Signal: maxdig-OptFilterDigits[0]="
                        << maxDigit - m_digits[0]
                        << "   OptFilterDigits[0]-OptFilterDigits[ digits_size_1]="
                        << m_digits[0] - m_digits[digits_size_1] );

        nIterations = iterate(ros, drawer, channel, gain, pedestal, amplitude, time, chi2);

        ATH_MSG_VERBOSE( "number of iterations= " << nIterations );

        m_nSignal++;
      } else if (m_digits[0] - minDigit > sigma) { //Pedestal events: OF with negative iterations

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "CASE NEGATIVE: maxdig-OptFilterDigits[0]="
                            << maxDigit - m_digits[0]
                            << "   OptFilterDigits[0]-OptFilterDigits[digits_size_1]="
                            << m_digits[0] - m_digits[digits_size_1] << endmsg;
          msg(MSG::VERBOSE) << "OptFilterDigits[0]-mindig="
                            << m_digits[0] - minDigit << endmsg;
        }

        for (int i = 0; i <= digits_size_1; i++)  // Mirror around pedestal
          m_digits[i] = pedestal - (m_digits[i] - pedestal);

        nIterations = iterate(ros, drawer, channel, gain, pedestal, amplitude, time, chi2);

        amplitude = -amplitude;

        ++m_nNegative;

      } else { // Gaussian center: no iterations and phase=0

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "CASE CENTER: maxdig-OptFilterDigits[0]="
                            << maxDigit - m_digits[0]
                            << "   OptFilterDigits[0]-OptFilterDigits[digits_size_1]="
                            << m_digits[0] - m_digits[digits_size_1] << endmsg;
          msg(MSG::VERBOSE) << "OptFilterDigits[0]-mindig="
                            << m_digits[0] - minDigit
                            << endmsg;
        }

        if (m_bestPhase) {
          unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
          // AS 19.11.09 - note minus sign here - time in DB is opposite to best phase 
          phase = -m_tileToolTiming->getSignalPhase(drawerIdx, channel, gain);
          ATH_MSG_VERBOSE( "Best phase: " << phase
                           << " drawerIdx " << drawerIdx
                           << " channel " << channel );
        }
      
        chi2 = compute(ros, drawer, channel, gain, pedestal, amplitude, time, phase);

        // If weights for tau=0 are used, deviations are seen in the amplitude =>
        // function to correct the amplitude
        if (m_correctAmplitude
            && amplitude > m_ampMinThresh
            && time > m_timeMinThresh
            && time < m_timeMaxThresh) {

          amplitude *= correctAmp(time, m_of2);
          ATH_MSG_VERBOSE( "Amplitude corrected by " << correctAmp(time, m_of2)
                           << " new amplitude is " << amplitude );
        }

        if (m_bestPhase) {
          time = -phase;
          chi2 = -chi2;
        } else {
          time = 0.;
        }

        m_nCenter++;

      }
      
    }

  }

  return chi2;
}


int TileRawChannelBuilderOpt2Filter::iterate(int ros, int drawer, int channel, int gain,
    double &pedestal, double &amplitude, double &time, double &chi2) {

  ATH_MSG_VERBOSE( "iterate()" );

  int nIterations = 0;
  double savePhase = 0.0;
  double phase = 0.0;
  time = -1000.;
  
  // Mythic -1 iteration or DSP emulation case
  if (m_minus1Iter || (m_emulateDsp && (m_maxIterations > 1)))
    phase = 25 * (m_t0SamplePosition - findMaxDigitPosition());

  while ((time > m_timeForConvergence
          || time < (-1.) * m_timeForConvergence
          || m_emulateDsp)
         && nIterations < m_maxIterations) {

    chi2 = compute(ros, drawer, channel, gain, pedestal, amplitude, time, phase);

    savePhase = phase;

    if (m_emulateDsp)
      phase -= round(time); // rounding phase to integer like in DSP
    else
      phase -= time; // no rounding at all for OFC on the fly

    if (phase > m_maxTime) phase = m_maxTime;
    if (phase < m_minTime) phase = m_minTime;

    ++nIterations;
    ATH_MSG_VERBOSE( " OptFilter computed with phase=" << savePhase
                    << " Time=" << time
                    << " END ITER=" << nIterations
                    << " new phase=" << phase
                    << " chi2=" << chi2
                    << "  Amp=" << amplitude );
  }
  
  time -= savePhase;
  if (time > m_maxTime) time = m_maxTime;
  if (time < m_minTime) time = m_minTime;
  
  ATH_MSG_VERBOSE( "OptFilterEne=" << amplitude
                  << " Phase=" << savePhase
                  << " Absolute Time=" << time );

  return nIterations;
}


double TileRawChannelBuilderOpt2Filter::compute(int ros, int drawer, int channel, int gain,
    double &pedestal, double &amplitude, double &time, double& phase) {

 ATH_MSG_VERBOSE( "compute();"
                 << " ros=" << ros
                 << " drawer="  << drawer
                 << " channel=" << channel
                 << " gain=" << gain );

  int i = 0, digits_size = m_digits.size();
  double chi2 = 0.;
  double a[9];
  double b[9];
  double c[9];
  double g[9];
  double dg[9];
  
  amplitude = 0.;
  time = 0.;
  float ofcPhase = (float) phase;

  unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
  TileOfcWeightsStruct weights;
  if (m_tileCondToolOfc->getOfcWeights(drawerIdx, channel, gain, ofcPhase, m_of2, weights).isFailure())
  {
    ATH_MSG_ERROR( "getOfcWeights fails" );
    return 0;
  }

  phase = ofcPhase;

  for (i = 0; i < digits_size; ++i) {
    a[i] = weights.w_a[i];
    b[i] = weights.w_b[i];
    g[i] = weights.g[i];
    dg[i] = weights.dg[i];
    if (m_of2) c[i] = weights.w_c[i]; // [OptFilterPha+100];
  }

  // for DSP emulation
  short a_int[9], ascale = 0, calib = 0, calib_offset = 0;
  short b_int[9], bscale = 0;
  unsigned short scale = 0, round = 0, lut = 0;
  int slope_scale = 0;
  int dspEnergy = 0, dspTime = 0;
  if (m_emulateDsp) {
    dspEnergy = 0;
    dspTime = 0;
    slope_scale = (int) truncf(log(std::pow(2., 15) - 1.) / log(2.));
    calib_offset = (short) roundf(std::pow(2., slope_scale));
    ofc2int(digits_size, a, a_int, ascale);
    ofc2int(digits_size, b, b_int, bscale);
    calib = ascale + slope_scale;
  }
  
  ATH_MSG_VERBOSE( "OptFilterPha=" << phase );

  if (m_of2) {
    if (m_emulateDsp) {
      pedestal = m_digits[0];
      for (i = 0; i < digits_size; ++i) {
        dspEnergy += a_int[i] * ((int) m_digits[i]);
        dspTime += b_int[i] * ((int) m_digits[i]);
        amplitude += a[i] * m_digits[i]; //aa temp
        time += b[i] * m_digits[i]; //aa temp
      }
    } else {
      pedestal = 0.;
      for (i = 0; i < digits_size; ++i) {
        amplitude += a[i] * m_digits[i];
        time += b[i] * m_digits[i];
        pedestal += c[i] * m_digits[i];
      }
    }
  } else {
    if (m_emulateDsp) pedestal = m_digits[0];
    for (i = 0; i < digits_size; ++i) {
      amplitude += a[i] * (m_digits[i] - pedestal);
      time += b[i] * (m_digits[i] - pedestal);
    }
  }

  if (m_emulateDsp) {
    round = 1 << (ascale - 2);
    short e2Dsp = (unsigned short) ((dspEnergy + round) >> (ascale - 1));
    size_t OptInd = abs(e2Dsp);
    if (OptInd >= (sizeof(lookup) / sizeof(short))) OptInd = 0;
    lut = lookup[OptInd];
    scale = bscale - 4 + lookup[0] - 9;
    round = 1 << (scale - 1);
    //    int told=dspTime;
    dspTime = (((dspTime + 0x100) >> 9) * lut + round) >> scale;
    //        printf(" 1 OptFilterTime %f OptFilterTimeDSP %d e2 %d round %d lut %d bscale %d scale %d told %d\n",OptFilterTime/OptFilterEne,OptFilterTimeDSP,OptFilterE2DSP,round,lut,bscale,scale,told);
    time = dspTime / 16.0;

    //  printf(" 1 OptFilterEneDSP %d calib_offset  %d calib %d \n",OptFilterEneDSP,calib_offset,calib);

    dspEnergy = (dspEnergy + 1024) >> 11;
    //  printf(" 2 OptFilterEneDSP %d  \n",OptFilterEneDSP);
    dspEnergy = (dspEnergy * calib_offset + (1 << (calib - 15 - 1))) >> (calib - 15);
    //  printf(" 3 OptFilterEneDSP %d  \n",OptFilterEneDSP);
    double goffset = (gain == 0) ? 512 : 2048;
    dspEnergy = (int) (dspEnergy + goffset);
    amplitude = (dspEnergy - goffset) / 16.;
  }

  bool goodEnergy = (fabs(amplitude) > 1.0e-04);
  if (goodEnergy) {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterEne=" << amplitude << endmsg;
      msg(MSG::VERBOSE) << "OptFilterTime*OptFilterEne=" << time << endmsg;
    }
    if (!m_emulateDsp) time /= amplitude;
  } else {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterEne=" << amplitude
                        << "   ... assuming 0.0" << endmsg;
      msg(MSG::VERBOSE) << "OptFilterTime*OptFilterEne=" << time
                        << "   ... assuming 0.0" << endmsg;
    }
    time = amplitude = 0.0;
  }
  
  /* new QF in both cases now 02.2010 AA
   if(!m_emulatedsp) {
   for (i=0; i<digits_size; ++i){
   if(OptFilterDigits[i]>0)
   OptFilterChi2 += 50*fabs(OptFilterDigits[i]-(OptFilterEne*g[i]+OptFilterPed))/OptFilterDigits[i];
   else
   OptFilterChi2 += 50*fabs(OptFilterDigits[i]-(OptFilterEne*g[i]+OptFilterPed))/1024.;
   }
   } else {
   */
  for (i = 0; i < digits_size; ++i) {
    double dqf = m_digits[i] - amplitude * g[i] + amplitude * time * dg[i] - pedestal;
    chi2 += dqf * dqf;
  }
  chi2 = sqrt(chi2);
  // new QF  }
  
  //  std::cout << "emulate " << m_emulatedsp << " OptFilterEne " << OptFilterEne << " OptFilterDigits[3]" << OptFilterDigits[3] << " OptFilterTime="<<OptFilterTime<<" OptFilterPed="<<OptFilterPed<<" OptFilterChi2="<<OptFilterChi2<<" g 3 " << g[3] << " dg 1 3 5 " << dg[1] << " " << dg[3] << " " << dg[5] <<std::endl;
  if (fabs(chi2) > 1.0e-04 || goodEnergy) {
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


void TileRawChannelBuilderOpt2Filter::ofc2int(int nDigits, double* w_off, short* w_int, short &scale) {
  // Number of bits of the integer word (signed -1 )
  int numberBits = 16;
  numberBits = numberBits - 1;
  
  // Get Absolute Maximum
  double max = -10000.;
  double sum = 0.;
  for (int i = 0; i < nDigits; i++) {
    sum += w_off[i];
    if (fabs(w_off[i]) > max) max = fabs(w_off[i]);
    //    printf("idig = %d weight  = %f \n",i, w_off[i]);
  }
  if (fabs(sum) > max) max = fabs(sum);
  
  // Get Scale at Maximum
  scale = 0;
  if (max != 0) scale = (int) truncf(log((std::pow(2., numberBits) - 1.) / max) / log(2.));
  
  // Convert to Integer the weights and the sum
  for (int i = 0; i < nDigits; i++)
    w_int[i] = (short) roundf(w_off[i] * std::pow(2., scale));
  //aa  w_sum_dsp = (short) roundf(sum*std::pow(2.,scale));
  

/* 
  if (msgLvl(MSG::VERBOSE)) {
    printf("\nAbsolute Max value = %15.8f -> Scale = %3d\n",max,scale);

    for (int i=0; i<ndigits; i++)
      {
	if ( i == 0){
	  printf("\n        Offline              Off*Scale             Dsp/scale    Dsp      Scale  \n");
	  printf("----------------------------------------------------------------------------------\n");
	  printf("  %17.10f     %17.10f    %17.10f  0x%04X    %3d   \n",
		 w_off[i],w_off[i]*std::pow(2.,scale), w_int[i]/std::pow(2.,scale),(unsigned int) w_int[i] ,scale);
	} else {
	  printf("  %17.10f     %17.10f    %17.10f  0x%04X   \n",
		 w_off[i],w_off[i]*std::pow(2.,scale), w_int[i]/std::pow(2.,scale),(unsigned int)w_int[i]);
	}
      }
    //aa    printf("  %17.10f     %17.10f    %17.10f  0x%04X     <- SUM\n",
    //aa	   sum,sum*std::pow(2.,scale),w_sum_dsp/std::pow(2.,scale),(unsigned int)w_sum_dsp);
  }
*/  
  return;
}

