/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileEvent/TileRawChannel.h"

// Gaudi includes
#include "GaudiKernel/Property.h"

// Atlas includes
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileRecUtils/TileRawChannelBuilderFlatFilter.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTrigType.h"
#include "TileConditions/TileInfo.h"

// lang include
#include <algorithm>
#include <cmath>

static const InterfaceID IID_ITileRawChannelBuilderFlatFilter
       ("TileRawChannelBuilderFlatFilter", 1, 0);

const InterfaceID& TileRawChannelBuilderFlatFilter::interfaceID( ) { 
  return IID_ITileRawChannelBuilderFlatFilter;
  //return TileRawChannelBuilder::interfaceID();
}

#define TILE_FLATFILTERBUILDERVERBOSE false

/**
 * Constructor
 */
TileRawChannelBuilderFlatFilter::TileRawChannelBuilderFlatFilter(const std::string& type,
    const std::string& name, const IInterface *parent)
    : TileRawChannelBuilder(type, name, parent)
{
  //declare interfaces
  declareInterface< TileRawChannelBuilder >( this );
  declareInterface< TileRawChannelBuilderFlatFilter >(this);
    
  m_rawChannelContainerKey = "TileRawChannelFlat";

  //declare properties
  declareProperty("PedStart",m_pedStart = 0);
  declareProperty("PedLength",m_pedLength = 1);
  declareProperty("PedOffset",m_pedOffset = 0);
  declareProperty("SignalStart",m_signalStart = 1);
  declareProperty("SignalLength",m_signalLength = 8);
  declareProperty("FilterLength",m_filterLength = 5);
  declareProperty("FrameLength",m_frameLength = 9);
  declareProperty("DeltaCutLo",m_deltaCut[0] = 4.5);
  declareProperty("DeltaCutHi",m_deltaCut[1] = 8.5);
  declareProperty("RMSCutLo",m_rmsCut[0] = 1.0);
  declareProperty("RMSCutHi",m_rmsCut[1] = 2.5);
}

/**
 * Destructor
 */
TileRawChannelBuilderFlatFilter::~TileRawChannelBuilderFlatFilter(){ 
}

/**
 * Initializer
 */
StatusCode TileRawChannelBuilderFlatFilter::initialize() {

  ATH_MSG_INFO( "TileRawChannelBuilderFlatFilter::initialize()" );

  m_rChType = TileFragHash::FlatFilter;

  // init in superclass
  CHECK( TileRawChannelBuilder::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelBuilderFlatFilter::finalize() {

  ATH_MSG_DEBUG( "Finalizing" );
  return StatusCode::SUCCESS;
}

TileRawChannel* TileRawChannelBuilderFlatFilter::rawChannel(const TileDigits* digits) {

  ++m_chCounter;

  const HWIdentifier adcId = digits->adc_HWID();
  int gain = m_tileHWID->adc(adcId);

  ATH_MSG_VERBOSE( "Running FlatFilter for TileRawChannel with HWID "
                  << m_tileHWID->to_string(adcId) );

  // tmp variables for filtering
  double amplitude = 0.0;
  double time = 0.0;

  // use flat filter
  flatFilter(digits->samples(), gain, amplitude, time);

  // flat filter calib
  if (m_calibrateEnergy) {
    amplitude = m_tileInfo->CisCalib(adcId, amplitude);
  }

  ATH_MSG_VERBOSE( "Creating RawChannel a=" << amplitude << " t=" << time );

  // return new TileRawChannel
  //  TileRawChannel *rawCh = new TileRawChannel(adcId,amplitude,time,0.0);

  DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  rawCh->assign (adcId, amplitude, time, 0, 0);

  if (m_correctTime) {
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

bool TileRawChannelBuilderFlatFilter::isSignalInFrame(const std::vector<float> &digits,
			 double deltaCut, double rmsCut, int &max, int &min, double &mean, double &rms) const {

  int t, val;
  // First compute the values that will be used to decide
  min  = 65536;
  max  = 0;
  mean = 0.0;
  rms  = 0.0;
  
  if(TILE_FLATFILTERBUILDERVERBOSE)
    ATH_MSG_VERBOSE( "Checking for signal in frame" );
  
  for (t = 0;  t < m_frameLength;  ++t) {
    val = (int)digits.at(t);
    mean += val;
    rms  += val*val;
    if ( val < min ) min = val;
    if ( val > max ) max = val;
  }

  mean /= m_frameLength;
  rms  /= m_frameLength;
  rms   = sqrt(rms - mean*mean);
  if ( ( (max-min) >= deltaCut ) &&
       ( rms    >=  rmsCut  ) ) {
    return true;
  } 
  else return false;
}

double TileRawChannelBuilderFlatFilter::getPedestal(const std::vector<float> &digits
                                                    , int pedStart, int pedLgt) const {
  int i, t, sum = 0;
  double ped;

  if (TILE_FLATFILTERBUILDERVERBOSE) {
    ATH_MSG_VERBOSE( "Getting pedestal " << pedStart
                    << "->" << (pedStart + pedLgt) );
  }
    
  if (pedLgt == 0)
    return 0.0;
  else if (pedLgt < 0) {
    // calculate sum of ped_length samples starting from ped_start
    // and go to the left (negative direction)
    // if number of samples is not enough, start from the end of the frame */
    pedLgt *= -1;
    t = pedStart;
    for (i = 0; i < pedLgt; ++i) {
      if (t < 0) t += m_frameLength;
      sum += (int) digits.at(t--);
    }
  }
  else {
    // calculate sum of ped_length samples starting from ped_start
    // and go to the right
    // if number of samples is not enough, start from the begining of the frame */
    t = pedStart;
    for (i = 0; i < pedLgt; ++i) {
      if (t == m_frameLength) t -= m_frameLength;
      sum += (int) digits.at(t++);
    }
  }
    
  // calculate mean ped
  ped = (double) sum / (double) pedLgt;

  if (TILE_FLATFILTERBUILDERVERBOSE) ATH_MSG_VERBOSE( "Got pedestal " << ped );

  return (ped);
}


int TileRawChannelBuilderFlatFilter::getMaxAdder(const std::vector<float> &digits,
			                                            int filterLength,int signalStart, int signalLength
			                                            , int &tMax, int &tMaxFrame, int &adderFrame) const {
  int i, t, minW, maxW, adder, adderWindow;
  int tm=0, maxAdder=-1;
  
  if(TILE_FLATFILTERBUILDERVERBOSE) {
    ATH_MSG_VERBOSE( "Getting adder flgt " << filterLength
                    << " Signal " << signalStart
                    << "->" << (signalStart + signalLength) );
  }

  // left and right limits for signal window
  minW = signalStart;
  maxW = signalStart + signalLength - filterLength + 1;

  // max adder inside signal window
  for (t = minW; t < maxW; ++t) {
    adder = 0;
    // sum of all samples in window
    for (i = t; i < t+filterLength; ++i) adder += (int)digits.at(i);
    // store maximum
    if ( adder > maxAdder ) {
      tm = t;
      maxAdder = adder;
    }
  }

  // return parameters for max adder in signal window
  tMax = tm;

  adderWindow = maxAdder;

  // right limit for whole frame
  maxW = m_frameLength - filterLength + 1;
  
  // max adder after signal window
  for ( ; t < maxW; ++t) {
    adder = 0;
    for (i = t; i < t+filterLength; ++i) adder += (int)digits.at(i);
    
    if ( adder > maxAdder ) {
      tm = t;
      maxAdder = adder;
    }
  }

  // max adder before signal window
  for (t = 0; t < minW; ++t) {
    adder = 0;
    for (i = t; i < t+filterLength; ++i)
      adder += (int)digits.at(i);

    if ( adder > maxAdder ) {
      tm = t;
      maxAdder = adder;
    }
  }

  // return parameters for max adder in whole frame
  tMaxFrame  = tm;
  adderFrame = maxAdder;

  if (TILE_FLATFILTERBUILDERVERBOSE) {
    ATH_MSG_VERBOSE( "Got max adder, tMax=" << tMax
                    << " tMaxFrame=" << tMaxFrame
                    << " maxAdder=" << maxAdder
                    << " adderWindow=" << adderWindow );
  }
  
  return adderWindow;
}

int TileRawChannelBuilderFlatFilter::getMaxSample(const std::vector<float> &digits
    , int signalStart, int signalLength, int &tMax, int &tMaxFrame, int &sampleFrame) const {

  int t, minW, maxW, sampleWindow;
  int tm = 0, maxSample = -1;
    
  if (TILE_FLATFILTERBUILDERVERBOSE) {
    ATH_MSG_VERBOSE( "Getting max sample, Signal " << signalStart
                    << "->" << (signalStart + signalLength) );
  }

  // left and right limits for signal window
  minW = signalStart;
  maxW = signalStart + signalLength;

  // max sample inside signal window take leftmost maximum
  for (t = minW; t < maxW; ++t) {
    if ((int) digits.at(t) > maxSample) {
      tm = t;
      maxSample = (int) digits.at(t);
    }
  }

  // let's check if we have more than one sample with the same amplitude
  // in such a case take central sample and also
  // avoid maximum at the boundary (tm=minW)
  for (t = tm + 1; t < maxW; ++t)
    if ((int) digits.at(t) != maxSample) break;

  t -= tm; // number of identical samples
  if (t == 2 && tm == minW) ++tm; // move from the boundary
  else tm += (t - 1) / 2; // center of the window

  // return parameters for max sample in signal window
  tMax = tm;
  sampleWindow = maxSample;

  // right limit for whole frame
  maxW = m_frameLength;

  // max sample after signal window
  for ( ; t < maxW; ++t) {
    if ( (int)digits.at(t) > maxSample ) {
      tm = t;
      maxSample = (int)digits.at(t);
    }
  }

  // max sample before signal window
  for (t = 0; t < minW; ++t) {
    if ( (int)digits.at(t) > maxSample ) {
      tm = t;
      maxSample = (int)digits.at(t);
    }
  }

  // return parameters for max adder in whole frame
  tMaxFrame = tm;
  sampleFrame = maxSample;

  if (TILE_FLATFILTERBUILDERVERBOSE) {
    ATH_MSG_VERBOSE( "Got max sample " << sampleFrame
                    << " tMaxFrame=" << tMaxFrame
                    << " tMax=" << tMax );
  }

  return sampleWindow;
}

double TileRawChannelBuilderFlatFilter::calculatePeak(const std::vector<float> &digits
    , int peakPos, double ped, double &position) const {

  int y_1, y0, y1;
  double peak, tmax, A, B, C;

  if (TILE_FLATFILTERBUILDERVERBOSE) {
    ATH_MSG_VERBOSE( "Calculating peak, Pos " << peakPos
                    << " Pedestal " << ped );
  }

  if ((peakPos == 0) || (peakPos == m_frameLength - 1)) {
    // peak at the boundary
    tmax = peakPos;
    peak = digits.at(peakPos);
  } else {
    // let's say that "peak_pos" is X=0 point
    y_1 = (int) digits.at(peakPos - 1); // ordinate at X=-1
    y0 = (int) digits.at(peakPos); // ordinate at X=0
    y1 = (int) digits.at(peakPos + 1); // ordinate at X=1

    // coefficients A,B,C of Y(x) = A*x^2 + B*x + C
    C = y0;
    B = (y1 - y_1) / 2.;
    A = y1 - B - C;

    if (A < 0.0) {
      // amplitude at peak_pos is local maximim
      tmax = peakPos - B / (A * 2.);
      peak = C - B * B / (A * 4.);
    } 
    else if ( y_1 == y0 && y0 == y1 ) {
      // flat distribution, no peak at all
      tmax = peakPos;
      peak = y0;
      } 
    else {
      // there is no local maximim 
      // phase has negative sign to indicate that,
      // peak is amplitude at peak_pos
      tmax = -peakPos;
      peak = y0;
    }
  }

  // subtract pedestal
  peak -= ped;
  // convert to nanoseconds
  position = tmax * 25.;

  if (TILE_FLATFILTERBUILDERVERBOSE)
    ATH_MSG_VERBOSE( "Found Peak " << peak << " at " << position );

  return (peak);
}

double TileRawChannelBuilderFlatFilter::calculateFlatFilter(const std::vector<float> &digits,
    int filterStart, int filterLength, double ped, double &position) const {

  int t, sum = 0;
  double energy;

  if (TILE_FLATFILTERBUILDERVERBOSE) {
    ATH_MSG_VERBOSE( "Calc flat filter Filter: " << filterStart
                    << "->" << (filterStart + filterLength)
                    << " pedestal=" << ped );
  }
  
  // calculate sum of filter_length samples
  for (t = filterStart;  t < filterStart+filterLength;  ++t)
      sum += (int)digits.at(t);

  // subtract pedestal
  energy = (double)sum - ped * filterLength;

  // calculate center of the filter and convert to nanoseconds
  position = (filterStart + (filterLength-1)/2.) * 25.;

  if (TILE_FLATFILTERBUILDERVERBOSE) {
    ATH_MSG_VERBOSE( "Flat amplitude=" << energy
                    << " position=" << position );
  }

  return(energy);
}

double TileRawChannelBuilderFlatFilter::getTime(const std::vector<float> &digits
    , int signalStart, int signalLength, double ped) const {

  int t, tmax = signalStart + signalLength;
  double position, val, sig = 0.0, sum = 0.0;

  if (TILE_FLATFILTERBUILDERVERBOSE) {
    ATH_MSG_VERBOSE( "Getting time Signal: " << signalStart
                    << "->" << (signalStart + signalLength)
                    << " pedestal=" << ped );
  }

  // calculate sum of samples and sum of samples weighted with time
  for (t = signalStart; t < tmax; ++t) {
    val = (int) (digits.at(t) - ped);
    sig += val;
    sum += val * t;
  }

  if (sig > 0.0 && sum > 0) {
    position = sum / sig;
    // convert to nanoseconds
    if (position < tmax) position *= 25.0;
    else position = 0.0;

  } else {
    position = 0.0;
  }

  if (TILE_FLATFILTERBUILDERVERBOSE)
    ATH_MSG_VERBOSE( "Got time " << position );

  return (position);
}

void TileRawChannelBuilderFlatFilter::flatFilter(const std::vector<float>& samples
    , const int gain, double& amplitude, double& time) const {

  int frameMax = 0;
  int frameMin = 0;
  double frameMean = 0.0;
  double frameRMS = 0.0;
  bool signal = false;
  double pedestal = 0.0;

  //int adderMax = 0; // maximum of adder
  int tAdder = 0; // time for adder
  int tAdderFrame = 0;
  int adderMaxFrame = 0;

  //int sampleMax = 0;
  int tSample = 0;
  int tSampleFrame = 0;
  int sampleMaxFrame = 0;

  //double peakAmplitude;
  //double peakEnergy;
  //double peakTime;
  //double flatEnergy;

  double flatFilter; // non calibrated energy from flat filter
  double flatTime;

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Digits." << samples.size() << " {";
    for (unsigned i = 0; i < samples.size(); i++) {
      msg(MSG::VERBOSE) << samples[i] << " ";
    }
    msg(MSG::VERBOSE) << "}" << endmsg;
  }

  // check if signal in frame
  signal = isSignalInFrame(samples, m_deltaCut[gain % 2], m_rmsCut[gain % 2]
                           , frameMax, frameMin, frameMean, frameRMS);

  if (signal && (m_pedOffset < 0)) {
    // take pedestal before signal
    // and if number of samples before signal is not enough -
    // use samples at the end of the frame
    pedestal = getPedestal(samples, tAdder + m_pedOffset, -m_pedLength);
  } else {
    // fixed position of pedestal window for channel without signal
    pedestal = getPedestal(samples, m_pedStart, m_pedLength);
  }

  //------------------ Flat Filter ---------------
  /*adderMax =*/getMaxAdder(samples, m_filterLength, m_signalStart, m_signalLength
                            , tAdder , tAdderFrame, adderMaxFrame);

  // look for max. sample inside maxAdder
  /*sampleMax =*/getMaxSample(samples, tAdder, m_filterLength, tSample
                              , tSampleFrame, sampleMaxFrame);

  // position of the peak is always in the signal window even if
  // real maximum is outside signal window
  //peakAmplitude = calculatePeak (samples, tSample, pedestal, peakTime);
  //peakEnergy = calibrateCSL(calibratePeak(peakAmplitude, adcId), adcId);

  // flat filter method
  // position of the filter is always in the signal window even if
  // real maximum is outside signal window
  flatFilter = calculateFlatFilter(samples, tAdder, m_filterLength, pedestal, flatTime);

  //ZZZ: No calibration at all!
  //flatEnergy = calibrateCSL(calibrateFlatFilter(flatFilter,adcId),adcId);

  // calculate time of the signal as weighted sum of all samples in signal window
  time = getTime(samples, m_signalStart, m_signalLength, pedestal);
  if (time <= 0.0) time = -flatTime;

  amplitude = flatFilter;
}

void TileRawChannelBuilderFlatFilter::flatFilter(const std::vector<uint32_t> &digits,
    const int gain, double& amplitude, double& time) const {

  std::vector<float> dVec;

  if (TILE_FLATFILTERBUILDERVERBOSE)
    ATH_MSG_VERBOSE( "TileRawChannelBuilderFlatFilter - Invoked with vector<int>..");

  dVec.reserve(digits.size());
  for (uint32_t i = 0; i < digits.size(); i++) {
    dVec.push_back((float) digits[i]);
  }
  return flatFilter(dVec, gain, amplitude, time);
}

