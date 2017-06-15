/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
//
//     Author:
//       Alexander.Paramonov@cern.ch
//
//     Created:
//       05 February 2016
//
//     File Name:
//       TileRawChannelBuilderQIEFilter.h
//
//    Description:
//      The code calculates time and energy/charge of QIE pulses
//
//////////////////////////////////////////////////////////////////////

#include "TileEvent/TileRawChannel.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

// Gaudi includes
#include "GaudiKernel/Property.h"

// Atlas includes
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileRecUtils/TileRawChannelBuilderQIEFilter.h"
//#include "TileRecUtils/TileRawChannelBuilderQIEFilterLookup.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"

//using namespace std;
#include <algorithm>

//interface stuff
static const InterfaceID IID_ITileRawChannelBuilderQIEFilter("TileRawChannelBuilderQIEFilter", 1, 0);

const InterfaceID& TileRawChannelBuilderQIEFilter::interfaceID() {
  return IID_ITileRawChannelBuilderQIEFilter;
}

#define TILE_QIEFilterBUILDERVERBOSE false

TileRawChannelBuilderQIEFilter::TileRawChannelBuilderQIEFilter(const std::string& type, const std::string& name, const IInterface *parent) :
  TileRawChannelBuilder(type, name, parent),
  m_nSignal(0),
  m_nConst(0),
  m_nSamples(0),
  m_t0SamplePosition(0)
{
  //declare interfaces
  declareInterface < TileRawChannelBuilder > (this);
  declareInterface < TileRawChannelBuilderQIEFilter > (this);

  m_TileRawChannelContainerID = "TileRawChannelQIE";

  //declare properties

  declareProperty("PedestalMode", m_pedestalMode = 17);

}

TileRawChannelBuilderQIEFilter::~TileRawChannelBuilderQIEFilter() {
}

StatusCode TileRawChannelBuilderQIEFilter::initialize() {

  ATH_MSG_INFO("initialize()");

  m_rChType = TileFragHash::OptFilterOffline; // type for offline Opt Filter

  // init in superclass
  CHECK(TileRawChannelBuilder::initialize());
  m_nSamples = m_tileInfo->NdigitSamples();
  m_t0SamplePosition = m_tileInfo->ItrigSample();

  if (m_nSamples < 3) { 
    ATH_MSG_ERROR("Incompatable number of samples [" << m_nSamples << "]");
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelBuilderQIEFilter::finalize() {

  ATH_MSG_DEBUG("Finalizing");

  return StatusCode::SUCCESS;
}

TileRawChannel * TileRawChannelBuilderQIEFilter::rawChannel(const TileDigits* digits) {

  ++m_chCounter;

  float pedestal = 10.; // A non-zero default is used to avoid the data quality cuts.
  float energy = 0.;
  float time = 0.;
  float chi2 = 0.;
  m_digits = digits->samples(); //the 7 samples; currently charge is expected. It the future these will be digitized
  const HWIdentifier adcId = digits->adc_HWID();
  int gain = m_tileHWID->adc(adcId); //this is set to 1 for QIE FEB by default

  ATH_MSG_VERBOSE("Building Raw Channel, with QIEFilter, HWID:" << m_tileHWID->to_string(adcId) << " gain=" << gain);

  int ros = m_tileHWID->ros(adcId);
  int drawer = m_tileHWID->drawer(adcId);
  int channel = m_tileHWID->channel(adcId);
  chi2 = filter(ros, drawer, channel, energy, time);


  //std::cout << "\nros: " << ros << ", drawer:" << drawer << ", channel:" << channel << "\n";
  //std::cout << "energy: " << energy << "\n";
  //std::cout << "digits: " << m_digits[0] << ", " << m_digits[1] << ", " << m_digits[2] << ", " << m_digits[3] << ", " << m_digits[4] << ", " << m_digits[5] << ", " << m_digits[6] << "\n";


  //TODO: Energy calibration needs to be implemented
  if (m_calibrateEnergy) {
    energy = m_tileInfo->CisCalib(adcId, energy);
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Creating QIEFilter RawChannel" << " a=" << energy << " t=" << time << " q=" << chi2 << endmsg;

    msg(MSG::VERBOSE) << "digits:";

    for (unsigned int i = 0; i < m_digits.size(); ++i)
      msg(MSG::VERBOSE) << " " << m_digits[i];

    msg(MSG::VERBOSE) << " " << endmsg;
  }

  // return new TileRawChannel
  // TileRawChannel *rawCh = new TileRawChannel(adcId,OptFilterEne,OptFilterTime,OptFilterChi2,OptFilterPed);
  DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  rawCh->assign(adcId, energy, time, chi2, pedestal);


  if (TileID::HIGHGAIN == gain) {
    ++m_nChH;
    m_RChSumH += energy;
  } else {
    ++m_nChL;
    m_RChSumL += energy;
  }

  return rawCh;
}

int TileRawChannelBuilderQIEFilter::findMaxDigitPosition() {

  ATH_MSG_VERBOSE("  findMaxDigitPosition()");

  int iMaxDigit = 0;
  float maxDigit = 0.;
  //bool saturated = false;

  for (unsigned int i = 0; i < m_digits.size(); i++) {
    //TODO: Enable when digitization is introduced
    //if (m_digits[i] > 510.99) saturated = true; //QIE returns range *128 + adc codes from 0 511
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

    //if (saturated)
    //  msg(MSG::VERBOSE) << " Samples saturated" << endmsg;
  }

  return iMaxDigit;
}



float TileRawChannelBuilderQIEFilter::filter(int ros, int drawer, int channel, float &amplitude, float &time) {

  ATH_MSG_VERBOSE("filter()");

  amplitude = 0.;
  time = 0.;
  float chi2 = -1.;

  chi2 = compute(ros, drawer, channel, amplitude, time);
  m_nSignal++;

  return chi2;
}

//charge per 25 ns is calculated here
float TileRawChannelBuilderQIEFilter::compute(int ros, int drawer, int channel, float &amplitude, float &time) {

  ATH_MSG_VERBOSE("compute();" << " ros=" << ros << " drawer=" << drawer << " channel=" << channel);

  //int i = 0, digits_size = m_digits.size();
  float chi2 = 1.; //the chi2 is not calculated by default
  float fraction_mean = 0;
  //float expected_fraction_RMS = 0;


  amplitude = 0.;
  time = 0.; //TODO: Time needs to be implemented using the QIE TDC

  //TODO; The code needs to be able to handle QIE counts instead of fC/3-in-1 ADC counts
  //TODO: Timing information needs to be added

  //TODO: These numbers may be placed in a database
  const float avg_fraction = 0.16;
  //February 2016: The transfer constants are taken from the Oct 2015 testbeam results; they are not optimal:
  //0.16/(1-0.16) = 0.19

  const float Q_1pe = 85.932 / 72.; //85.952/72 high-gain ADC counts per a photo-electron
  //16 pC  per a photo-electron in fC
  //1GeV = 1.05 pC

  //m_digits[m_t0SamplePosition] charge in the central interaction
  //m_digits[m_t0SamplePosition+1] charge in the following interaction;


  if ((m_t0SamplePosition - 1) < 0 || (m_t0SamplePosition + 1) >= m_nSamples) { //check if the filter is configures properly to run the algorithm
    return -1.;
  }

  if (m_digits[m_t0SamplePosition - 1] > 0) //check if there is pileup in the preceding BC.
    amplitude = m_digits[m_t0SamplePosition] - avg_fraction * m_digits[m_t0SamplePosition - 1] / (1. - avg_fraction);
  else
    amplitude = m_digits[m_t0SamplePosition];

  if (amplitude < 0)
    amplitude = 0;


  if (m_digits[m_t0SamplePosition + 1] > 0) {
    amplitude += m_digits[m_t0SamplePosition + 1];

    fraction_mean = m_digits[m_t0SamplePosition + 1] / amplitude;
    chi2 = fraction_mean;

    if (fraction_mean > avg_fraction ) { // Pileup is suspected in the 2nd integration window

      //translate charge into photo-electrons
      int n = (int) (amplitude / Q_1pe);
      int k = (int) (m_digits[m_t0SamplePosition + 1] / Q_1pe);

      //binomial probability distribution for photo-statistical fluctuations is assumed
      //one std diviation for binomial distribution is  sqrt(avg_fraction * (1. - avg_fraction) * n)

      //check if there is too much pileup in the 2nd integration window.
      if (((float) k - avg_fraction * (float) n) > 2. * sqrt(avg_fraction * (1. - avg_fraction) * (float) n)) {
        amplitude -= m_digits[m_t0SamplePosition + 1]; //ignore the charge collected in the 2nd window
        amplitude *= 1. / (1. - avg_fraction); //extrapolate charge collected in the 1st window
      }
    }

  }


  if (msgLvl(MSG::VERBOSE)) {

    bool goodEnergy = (fabs(amplitude) > 1.0e-04);
    if (goodEnergy) {
      msg(MSG::VERBOSE) << "QIEFilterEne=" << amplitude << endmsg;
      msg(MSG::VERBOSE) << "QIEFilterTimee=" << time << endmsg;
    } else {
      msg(MSG::VERBOSE) << "QIEFilterEne=" << amplitude << "   ... assuming 0.0" << endmsg;
      msg(MSG::VERBOSE) << "QIEFilterTime=" << time << "   ... assuming 0.0" << endmsg;
    }

    if (fabs(chi2) > 1.0e-04 || goodEnergy) {
      msg(MSG::VERBOSE) << "QIEFilterTime=" << time << endmsg;
      msg(MSG::VERBOSE) << "QIEFilterChi2=" << chi2 << endmsg;
    } else {
      msg(MSG::VERBOSE) << "QIEFilterTime=" << time << endmsg;
      msg(MSG::VERBOSE) << "QIEFilterChi2=" << chi2 << "   ... assuming 0.0" << endmsg;
    }

  }

  return chi2;
}


// find all bad patterns in a drawer and fill internal static arrays
void TileRawChannelBuilderQIEFilter::fill_drawer_errors(const TileDigitsCollection* /*collection*/){

}

