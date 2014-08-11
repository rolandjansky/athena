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

// small hack to enable datapool usage
#define private public
#define protected public
#include "TileEvent/TileRawChannel.h"
#undef private
#undef protected

// Gaudi includes
#include "GaudiKernel/Property.h"

// Atlas includes
#include "DataModel/DataPool.h"
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

using namespace std;

//interface stuff
static const InterfaceID IID_ITileRawChannelBuilderOptFilter("TileRawChannelBuilderOptFilter", 1, 0);

const InterfaceID& TileRawChannelBuilderOptFilter::interfaceID() {
  return IID_ITileRawChannelBuilderOptFilter;
}


#define TILE_OptFilterBUILDERVERBOSE false

TileRawChannelBuilderOptFilter::TileRawChannelBuilderOptFilter(const std::string& type,
    const std::string& name, const IInterface *parent)
  : TileRawChannelBuilder(type, name, parent)
  , c_signal(0)
  , c_negat(0)
  , c_center(0)
  , m_NSamp(0)
  , m_t0Samp(0)
  , m_maxTime(0.0)
  , m_minTime(0.0)
  , m_pulseShapes(0)
  , m_weights(0)
{
  //declare interfaces
  declareInterface< TileRawChannelBuilder >( this );
  declareInterface< TileRawChannelBuilderOptFilter >(this);

  m_TileRawChannelContainerID = "TileRawChannelOpt";

  //declare properties
  declareProperty("MaxIterations",m_maxIterations = 5);
  declareProperty("PedestalMode",m_pedestalMode = 17);
  declareProperty("TimeForConvergence",m_timeForConvergence = 0.5);
  declareProperty("ConfTB",m_ConfTB = false);
  declareProperty("OF2",m_of2 = true);
  declareProperty("Minus1Iteration",m_minus1Iter = false);
  declareProperty("AmplitudeCorrection",m_correctAmplitude = false);

}


TileRawChannelBuilderOptFilter::~TileRawChannelBuilderOptFilter() {
}


StatusCode TileRawChannelBuilderOptFilter::initialize() {

  ATH_MSG_INFO( "TileRawChannelBuilderOptFilter::initialize()" );

  m_rChType = TileFragHash::OptFilterOffline; // type for offline Opt Filter

  // init in superclass
  CHECK( TileRawChannelBuilder::initialize() );

  // bits 12-15 - various options
  if (m_correctAmplitude) m_bsflags |= 0x2000;
  if (m_maxIterations > 1) m_bsflags |= 0x4000;

  ATH_MSG_DEBUG( " MaxIterations=" << m_maxIterations
                << " PedestalMode=" << m_pedestalMode
                << " TimeForConvergence=" << m_timeForConvergence
                << " ConfTB=" << m_ConfTB
                << " OF2=" << m_of2
                << " Minus1Iteration=" << m_minus1Iter
                << " AmplitudeCorrection=" << m_correctAmplitude );

  m_NSamp = m_tileInfo->NdigitSamples();
  m_t0Samp = m_tileInfo->ItrigSample();
  m_maxTime = 25 * (m_NSamp - m_t0Samp - 1);
  m_minTime = -25 * m_t0Samp;
  ATH_MSG_DEBUG( " NSamples=" << m_NSamp
                << " T0Sample=" << m_t0Samp
                << " minTime=" << m_minTime
                << " maxTime=" << m_maxTime );

  if (m_pedestalMode % 10 > 2 && m_NSamp != m_pedestalMode % 10) {
    int oldPedestalMode(m_pedestalMode);
    m_pedestalMode = (m_pedestalMode / 10) * 10 + m_NSamp;
    ATH_MSG_DEBUG( "Changing PedestalMode from " << oldPedestalMode
                  << " to " << m_pedestalMode );
  }

  //get pointer to pulse shapes build pulse shapes)
  m_pulseShapes = m_tileInfo->getPulseShapes();

  vector<double> m_LpulseShapeX_cis, m_LpulseShapeT_cis;
  vector<double> m_HpulseShapeX_cis, m_HpulseShapeT_cis;
  vector<double> m_LpulseShapeX_phys, m_LpulseShapeT_phys;
  vector<double> m_HpulseShapeX_phys, m_HpulseShapeT_phys;

  vector<double> m_LdpulseShapeX_cis, m_LdpulseShapeT_cis;
  vector<double> m_HdpulseShapeX_cis, m_HdpulseShapeT_cis;
  vector<double> m_LdpulseShapeX_phys, m_LdpulseShapeT_phys;
  vector<double> m_HdpulseShapeX_phys, m_HdpulseShapeT_phys;

  m_LpulseShapeX_cis = m_pulseShapes->m_ylcis;
  m_LpulseShapeT_cis = m_pulseShapes->m_tlcis;
  m_HpulseShapeX_cis = m_pulseShapes->m_yhcis;
  m_HpulseShapeT_cis = m_pulseShapes->m_thcis;

  m_LpulseShapeX_phys = m_pulseShapes->m_ylphys;
  m_LpulseShapeT_phys = m_pulseShapes->m_tlphys;
  m_HpulseShapeX_phys = m_pulseShapes->m_yhphys;
  m_HpulseShapeT_phys = m_pulseShapes->m_thphys;

  m_LdpulseShapeX_cis = m_pulseShapes->m_ydlcis;
  m_LdpulseShapeT_cis = m_pulseShapes->m_tdlcis;
  m_HdpulseShapeX_cis = m_pulseShapes->m_ydhcis;
  m_HdpulseShapeT_cis = m_pulseShapes->m_tdhcis;

  m_LdpulseShapeX_phys = m_pulseShapes->m_ydlphys;
  m_LdpulseShapeT_phys = m_pulseShapes->m_tdlphys;
  m_HdpulseShapeX_phys = m_pulseShapes->m_ydhphys;
  m_HdpulseShapeT_phys = m_pulseShapes->m_tdhphys;

  if (msgLvl(MSG::DEBUG)) {

    msg(MSG::DEBUG) << " m_LpulseShapeX_cis.size()=" << m_LpulseShapeX_cis.size()
                    << " m_HpulseShapeX_cis.size()=" << m_HpulseShapeX_cis.size()
                    << " m_LpulseShapeX_phys.size()=" << m_LpulseShapeX_phys.size()
                    << " m_HpulseShapeX_phys.size()=" << m_HpulseShapeX_phys.size()
                    << " m_LdpulseShapeX_cis.size()=" << m_LdpulseShapeX_cis.size()
                    << " m_HdpulseShapeX_cis.size()=" << m_HdpulseShapeX_cis.size()
                    << " m_LdpulseShapeX_phys.size()=" << m_LdpulseShapeX_phys.size()
                    << " m_HdpulseShapeX_phys.size()=" << m_HdpulseShapeX_phys.size() << endmsg;


    int N = 0;
    for (unsigned int i = 0; i < m_LpulseShapeX_cis.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << m_LpulseShapeT_cis[i] << std::setw(8) << std::setprecision(4)
                      << m_LpulseShapeX_cis[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < m_HpulseShapeX_cis.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << m_HpulseShapeT_cis[i] << std::setw(8) << std::setprecision(4)
                      << m_HpulseShapeX_cis[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < m_LpulseShapeX_phys.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << m_LpulseShapeT_phys[i] << std::setw(8) << std::setprecision(4)
                      << m_LpulseShapeX_phys[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < m_HpulseShapeX_phys.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << m_HpulseShapeT_phys[i] << std::setw(8) << std::setprecision(4)
                      << m_HpulseShapeX_phys[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < m_LdpulseShapeX_cis.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                    << m_LdpulseShapeT_cis[i] << std::setw(8) << std::setprecision(4)
                    << m_LdpulseShapeX_cis[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < m_HdpulseShapeX_cis.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << m_HdpulseShapeT_cis[i] << std::setw(8) << std::setprecision(4)
                      << m_HdpulseShapeX_cis[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < m_LdpulseShapeX_phys.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << m_LdpulseShapeT_phys[i] << std::setw(8) << std::setprecision(4)
                      << m_LdpulseShapeX_phys[i] << endmsg;

    N = 0;
    for (unsigned int i = 0; i < m_HdpulseShapeX_phys.size(); i++)
      msg(MSG::DEBUG) << "N=" << ++N << std::setw(8) << std::setprecision(4)
                      << m_HdpulseShapeT_phys[i] << std::setw(8) << std::setprecision(4)
                      << m_HdpulseShapeX_phys[i] << endmsg;
  }

  //Build Pulse Shape (use the one in TileCalibAlgs?)
  BuildPulseShape(m_LpulseShape_cis, m_LpulseShapeX_cis, m_LpulseShapeT_cis, 9);
  BuildPulseShape(m_HpulseShape_cis, m_HpulseShapeX_cis, m_HpulseShapeT_cis, 9);
  BuildPulseShape(m_LpulseShape_phys, m_LpulseShapeX_phys, m_LpulseShapeT_phys, 9);
  BuildPulseShape(m_HpulseShape_phys, m_HpulseShapeX_phys, m_HpulseShapeT_phys, 9);
  BuildPulseShape(m_LdpulseShape_cis, m_LdpulseShapeX_cis, m_LdpulseShapeT_cis, 9);
  BuildPulseShape(m_HdpulseShape_cis, m_HdpulseShapeX_cis, m_HdpulseShapeT_cis, 9);
  BuildPulseShape(m_LdpulseShape_phys, m_LdpulseShapeX_phys, m_LdpulseShapeT_phys, 9);
  BuildPulseShape(m_HdpulseShape_phys, m_HdpulseShapeX_phys, m_HdpulseShapeT_phys, 9);

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "m_LpulseShape_cis.size()=" << m_LpulseShape_cis.size() << endmsg;
    for (unsigned int i = 0; i < m_LpulseShape_cis.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_LpulseShape_cis[i] << endmsg;

    msg(MSG::DEBUG) << "m_HpulseShape_cis.size()=" << m_HpulseShape_cis.size() << endmsg;
    for (unsigned int i = 0; i < m_HpulseShape_cis.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_HpulseShape_cis[i] << endmsg;

    msg(MSG::DEBUG) << "m_LpulseShape_phys.size()=" << m_LpulseShape_phys.size() << endmsg;
    for (unsigned int i = 0; i < m_LpulseShape_phys.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_LpulseShape_phys[i] << endmsg;

    msg(MSG::DEBUG) << "m_HpulseShape_phys.size()=" << m_HpulseShape_phys.size() << endmsg;
    for (unsigned int i = 0; i < m_HpulseShape_phys.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_HpulseShape_phys[i] << endmsg;

    msg(MSG::DEBUG) << "m_LdpulseShape_cis.size()=" << m_LdpulseShape_cis.size() << endmsg;
    for (unsigned int i = 0; i < m_LdpulseShape_cis.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_LdpulseShape_cis[i] << endmsg;

    msg(MSG::DEBUG) << "m_HdpulseShape_cis.size()=" << m_HdpulseShape_cis.size() << endmsg;
    for (unsigned int i = 0; i < m_HdpulseShape_cis.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_HdpulseShape_cis[i] << endmsg;

    msg(MSG::DEBUG) << "m_LdpulseShape_phys.size()=" << m_LdpulseShape_phys.size() << endmsg;
    for (unsigned int i = 0; i < m_LdpulseShape_phys.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_LdpulseShape_phys[i] << endmsg;

    msg(MSG::DEBUG) << "m_HdpulseShape_phys.size()=" << m_HdpulseShape_phys.size() << endmsg;
    for (unsigned int i = 0; i < m_HdpulseShape_phys.size(); i++)
      msg(MSG::DEBUG) << i << " " << m_HdpulseShape_phys[i] << endmsg;
  }

  //get pointer to weights (loaded by the InfoLoader in TileInfo, from TileOptFilterWeights)
  m_weights = m_tileInfo->getOptFilterWeights();
  ATH_MSG_DEBUG("Weights loaded" );

  c_signal = 0;
  c_negat = 0;
  c_center = 0;

  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelBuilderOptFilter::finalize() {

  ATH_MSG_VERBOSE( "Counters: Signal=" << c_signal
                  << " Negat=" << c_negat
                  << " Center=" << c_center );

  ATH_MSG_DEBUG( "Finalizing" );

  return StatusCode::SUCCESS;
}


TileRawChannel * TileRawChannelBuilderOptFilter::rawChannel(const TileDigits* digits) {
  ++m_chCounter;

  double OptFilterPed = 0., OptFilterEne = 0., OptFilterTime = 0., OptFilterChi2 = 0.;
  OptFilterDigits = digits->samples();
  const HWIdentifier adcId = digits->adc_HWID();
  int OptFilterGain = m_tileHWID->adc(adcId);

  ATH_MSG_VERBOSE( "Building Raw Channel, with OptFilter, HWID:" << m_tileHWID->to_string(adcId)
                  << " gain=" << OptFilterGain );

  if (m_ConfTB) {  // Using old weights for CTB
    int ros = m_tileHWID->ros(adcId);
    int drawer = m_tileHWID->drawer(adcId);
    int channel = m_tileHWID->channel(adcId);
    OptFilterChi2 = Filter(ros, drawer, channel, OptFilterGain, OptFilterPed, OptFilterEne,
        OptFilterTime);

  } else {  // Using weights obtained with delta correlation
    OptFilterChi2 = Filter(1000, 1000, 1000, OptFilterGain, OptFilterPed, OptFilterEne,
        OptFilterTime);
  }

  if (m_calibrateEnergy) {
    OptFilterEne = m_tileInfo->CisCalib(adcId, OptFilterEne);
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Creating OptFilter RawChannel a=" << OptFilterEne
                      << " t=" << OptFilterTime
                      << " ped=" << OptFilterPed
                      << " q=" << OptFilterChi2 << endmsg;

    msg(MSG::VERBOSE) << "digits:";
    for (unsigned int i = 0; i < OptFilterDigits.size(); ++i)
      msg(MSG::VERBOSE) << " " << OptFilterDigits[i];

    msg(MSG::VERBOSE) << " " << endmsg;
  }

  // return new TileRawChannel
  // TileRawChannel *rawCh = new TileRawChannel(adcId,OptFilterEne,OptFilterTime,OptFilterChi2,OptFilterPed);

  static DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  rawCh->m_adc_hwid = adcId;
  rawCh->m_amplitude.resize(1);
  rawCh->m_amplitude[0] = OptFilterEne;
  rawCh->m_time.resize(1);
  rawCh->m_time[0] = OptFilterTime;
  rawCh->m_quality.resize(1);
  rawCh->m_quality[0] = OptFilterChi2;
  rawCh->m_pedestal = OptFilterPed;

  if (m_correctTime
      && (OptFilterTime != 0 && OptFilterTime < m_maxTime && OptFilterTime > m_minTime)) {

    rawCh->insertTime(m_tileInfo->TimeCalib(adcId, OptFilterTime));
    ATH_MSG_VERBOSE( "Correcting time, new time=" << rawCh->time() );

  }

  if (TileID::HIGHGAIN == OptFilterGain) {
    ++m_nChH;
    m_RChSumH += OptFilterEne;
  } else {
    ++m_nChL;
    m_RChSumL += OptFilterEne;
  }

  return rawCh;
}


float TileRawChannelBuilderOptFilter::FindMaxDigit() {

  ATH_MSG_VERBOSE( "  TileRawChannelBuilderOptFilter::FindMaxDigit()" );

  int imaxdig = 0;
  float maxdig = 0.;
  bool saturated = false;

  for (unsigned int i = 0; i < OptFilterDigits.size(); i++) {
    if (OptFilterDigits[i] > 1022.99) saturated = true;
    if (maxdig < OptFilterDigits[i]) {
      maxdig = OptFilterDigits[i];
      imaxdig = i;
    }
  }

  if (msgLvl(MSG::VERBOSE)) {
    for (unsigned int i = 0; i < OptFilterDigits.size(); i++) {
      msg(MSG::VERBOSE) << " " << OptFilterDigits[i];
    }

    msg(MSG::VERBOSE) << "; Max: digit[" << imaxdig << "]=" << maxdig << endmsg;

    if (saturated) msg(MSG::VERBOSE) << " Samples saturated" << endmsg;
  }

  return imaxdig;
}

float TileRawChannelBuilderOptFilter::MaxDigDiff() {
  float leastdig = 1024.;
  float maxdig = 0.;

  for (unsigned int i = 0; i < OptFilterDigits.size(); i++) {
    if (leastdig > OptFilterDigits[i]) leastdig = OptFilterDigits[i];
    if (maxdig < OptFilterDigits[i]) maxdig = OptFilterDigits[i];
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "  TileRawChannelBuilderOptFilter::MaxDigDiff()" << endmsg;
    msg(MSG::VERBOSE) << "      maxdig-leastdig=" << maxdig - leastdig << endmsg;
  }

  return maxdig - leastdig;
}


float TileRawChannelBuilderOptFilter::MaxDigit() {
  float maxdig = 0.;

  for (unsigned int i = 0; i < OptFilterDigits.size(); i++)
    if (maxdig < OptFilterDigits[i]) maxdig = OptFilterDigits[i];

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "  TileRawChannelBuilderOptFilter::MaxDigit()" << endmsg;
    msg(MSG::VERBOSE) << "      maxdig=" << maxdig << endmsg;
  }

  return maxdig;
}


bool TileRawChannelBuilderOptFilter::Are3FF() {
  bool allSaturated = true;

  for (unsigned int i = 0; i < OptFilterDigits.size(); i++) {
    allSaturated = allSaturated && (OptFilterDigits[i] > 1022.99);
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "  TileRawChannelBuilderOptFilter::Are3FF()" << endmsg;
    msg(MSG::VERBOSE) << "      allSat=" << allSaturated << endmsg;
  }

  return allSaturated;
}


float TileRawChannelBuilderOptFilter::SetPedestal() {
  float pedestal = 0.;

  switch (m_pedestalMode) {
    case 7:
      pedestal = OptFilterDigits[6];
      break;
    case 9:
      pedestal = OptFilterDigits[8];
      break;
    case 12:
      pedestal = .5 * (OptFilterDigits[0] + OptFilterDigits[1]);
      break;
    case 17:
      pedestal = .5 * (OptFilterDigits[0] + OptFilterDigits[6]);
      break;
     case 19:
      pedestal = .5 * (OptFilterDigits[0] + OptFilterDigits[8]);
      break;
    default:
      pedestal = OptFilterDigits[0];
      break;
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "  TileRawChannelBuilderOptFilter::SetPedestal()" << endmsg;
    msg(MSG::VERBOSE) << "      pedestal=" << pedestal << endmsg;
  }

  return pedestal;
}


double TileRawChannelBuilderOptFilter::Filter(int ros, int drawer, int channel
    , int &OptFilterGain, double &OptFilterPed, double &OptFilterEne, double &OptFilterTime) {

  ATH_MSG_VERBOSE( "TileRawChannelBuilderOptFilter::Filter" );

  OptFilterPed = SetPedestal();
  OptFilterEne = 0.;
  OptFilterTime = 0.;
  int OptFilterPha = 0;
  double OptFilterChi2 = 0.;
  int /* imaxdig=FindMaxDigit(), */niter = 0, digits_size = OptFilterDigits.size();
  float digdiff = MaxDigDiff();
  float maxdig = MaxDigit();
  float mindig = maxdig - digdiff;

  // values used for pedestal-like events when iterations are performed
  int sigma;
  int sigma_hi = 5;
  int sigma_lo = 3;

  if (OptFilterGain == 0) sigma = sigma_lo;
  else sigma = sigma_hi;

  if (Are3FF()) {

    OptFilterGain = 0;
    OptFilterPed = 0.;
    OptFilterEne = 0.;
    OptFilterTime = 0.;
    if (digits_size > 0 && OptFilterDigits[0] > 2046.99)
      OptFilterChi2 = 9999.;
    else
      OptFilterChi2 = 0.;

  } else {

    if (m_maxIterations == 1) {  // Without iterations
      OptFilterChi2 = Compute(ros, drawer, channel, OptFilterGain, OptFilterPed
                              , OptFilterEne, OptFilterTime, OptFilterPha);

      // If weights for tau=0 are used, deviations are seen in the amplitude =>
      // function to correct the amplitude
      if (m_correctAmplitude
          && OptFilterEne > m_ampMinThresh
          && OptFilterTime > m_timeMinThresh
          && OptFilterTime < m_timeMaxThresh) {

        OptFilterEne *= correctAmp(OptFilterTime);
        ATH_MSG_VERBOSE( "Amlitude corrected by " << correctAmp(OptFilterTime)
                        << " new amplitude is " << OptFilterEne );
      }

      if (OptFilterTime > m_maxTime) OptFilterTime = m_maxTime;
      if (OptFilterTime < m_minTime) OptFilterTime = m_minTime;

    } else { // With iterations => 3 cases defined for correct pedestal treatment

      // Signal events: OF with iterations
      if ((maxdig - OptFilterDigits[0] > sigma)
          || (OptFilterDigits[0] - OptFilterDigits[digits_size - 1] > 4 * sigma)) {

        ATH_MSG_VERBOSE( "CASE Signal: maxdig-OptFilterDigits[0]="
                        << maxdig - OptFilterDigits[0]
                        << "   OptFilterDigits[0]-OptFilterDigits[ digits_size-1]="
                        << OptFilterDigits[0] - OptFilterDigits[digits_size - 1] );

        niter = Iterator(ros, drawer, channel, OptFilterGain, OptFilterPed, OptFilterEne,
            OptFilterTime, OptFilterChi2);

        ATH_MSG_VERBOSE( "number of iterations= " << niter );

        c_signal++;
      } else if (OptFilterDigits[0] - mindig > sigma) { //Pedestal events: OF with negative iterations

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "CASE NEGATIVE: maxdig-OptFilterDigits[0]="
                            << maxdig - OptFilterDigits[0]
                            << "   OptFilterDigits[0]-OptFilterDigits[digits_size-1]="
                            << OptFilterDigits[0] - OptFilterDigits[digits_size - 1] << endmsg;

          msg(MSG::VERBOSE) << "OptFilterDigits[0]-mindig="
                            << OptFilterDigits[0] - mindig << endmsg;
        }

        for (int i = 0; i < digits_size; i++)  // Mirror around pedestal
          OptFilterDigits[i] = OptFilterPed - (OptFilterDigits[i] - OptFilterPed);

        niter = Iterator(ros, drawer, channel, OptFilterGain, OptFilterPed
                        , OptFilterEne, OptFilterTime, OptFilterChi2);

        OptFilterEne = -OptFilterEne;

        c_negat++;

      } else { // Gaussian center: no iterations and phase=0

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "CASE CENTER: maxdig-OptFilterDigits[0]="
                            << maxdig - OptFilterDigits[0]
                            << "   OptFilterDigits[0]-OptFilterDigits[digits_size-1]="
                            << OptFilterDigits[0] - OptFilterDigits[digits_size - 1] << endmsg;

          msg(MSG::VERBOSE) << "OptFilterDigits[0]-mindig="
                            << OptFilterDigits[0] - mindig << endmsg;
        }
        //	OptFilterTime=-100.;

        OptFilterChi2 = Compute(ros, drawer, channel, OptFilterGain, OptFilterPed, OptFilterEne,
            OptFilterTime, OptFilterPha);

        OptFilterTime = 0.;
        c_center++;

      }

    }

  }

  return OptFilterChi2;
}


int TileRawChannelBuilderOptFilter::Iterator(int ros, int drawer, int channel, int OptFilterGain,
    double &OptFilterPed, double &OptFilterEne, double &OptFilterTime, double &OptFilterChi2) {

  ATH_MSG_VERBOSE( "TileRawChannelBuilderOptFilter::Iterator()" );

  int niter = 0;
  double save_phase = 0.0;
  double OptFilterPha = 0.0;
  OptFilterTime = -1000.;

  // Mythic -1 iteration
  if (m_minus1Iter) OptFilterPha = 25 * (m_t0Samp - FindMaxDigit());

  while ((OptFilterTime > m_timeForConvergence
          || OptFilterTime < (-1.) * m_timeForConvergence)
        && niter < m_maxIterations) {

    OptFilterChi2 = Compute(ros, drawer, channel, OptFilterGain, OptFilterPed
                            , OptFilterEne, OptFilterTime, (int) OptFilterPha);

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << " OptFilter computed with phase=" << OptFilterPha
                        << " Time=" << OptFilterTime;
    }

    save_phase = OptFilterPha;

    OptFilterPha -= round(OptFilterTime);
    if (OptFilterPha > m_maxTime) OptFilterPha = m_maxTime;
    if (OptFilterPha < m_minTime) OptFilterPha = m_minTime;

    niter++;
    if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << " END ITER=" << niter
                            << " new phase=" << OptFilterPha
                            << " chi2=" << OptFilterChi2
                            << "  Amp=" << OptFilterEne << endmsg;
    }
  }

  OptFilterTime -= save_phase;
  if (OptFilterTime > m_maxTime) OptFilterTime = m_maxTime;
  if (OptFilterTime < m_minTime) OptFilterTime = m_minTime;

  ATH_MSG_VERBOSE( "OptFilterEne=" << OptFilterEne
                  << " Phase=" << save_phase
                  << " Absolute Time=" << OptFilterTime );

  return niter;
}


double TileRawChannelBuilderOptFilter::Compute(int ros, int drawer, int channel, int OptFilterGain,
    double &OptFilterPed, double &OptFilterEne, double &OptFilterTime, int OptFilterPha) {

  ATH_MSG_VERBOSE( "TileRawChannelBuilderOptFilter::Compute(); ros=" << ros
                  << " drawer=" << drawer
                  << " channel=" << channel
                  << " gain=" << OptFilterGain );

  int i = 0, digits_size = OptFilterDigits.size();
  double OptFilterChi2 = 0.;
  double a[9];
  double b[9];
  double c[9];
  double g[9];

  OptFilterEne = 0.;
  OptFilterTime = 0.;

  if (m_idocis) {
    for (i = 0; i < 7; ++i) { // 7 samples only
      a[i] = m_weights->a_cis_simp[OptFilterGain][i][OptFilterPha + 100];
      b[i] = m_weights->b_cis_simp[OptFilterGain][i][OptFilterPha + 100];
      if (m_of2) c[i] = m_weights->c_cis_simp[OptFilterGain][i][OptFilterPha + 100];
    }
  } else {
    for (i = 0; i < digits_size; ++i) { // 7 or 9 samples
      a[i] = m_weights->a_phys_simp[OptFilterGain][i][OptFilterPha + 100];
      b[i] = m_weights->b_phys_simp[OptFilterGain][i][OptFilterPha + 100];
      if (m_of2) c[i] = m_weights->c_phys_simp[OptFilterGain][i][OptFilterPha + 100];
    }
  }

  if (m_idocis)
    if (OptFilterGain == 0)
      for (i = 0; i < digits_size; i++)
        g[i] = m_LpulseShape_cis[(i - (digits_size - 9) / 2) * 25 + 100 + OptFilterPha];
    else
      for (i = 0; i < digits_size; i++)
        g[i] = m_HpulseShape_cis[(i - (digits_size - 9) / 2) * 25 + 100 + OptFilterPha];
  else if (OptFilterGain == 0)
    for (i = 0; i < digits_size; i++)
      g[i] = m_LpulseShape_phys[(i - (digits_size - 9) / 2) * 25 + 100 + OptFilterPha];
  else
    for (i = 0; i < digits_size; i++)
      g[i] = m_HpulseShape_phys[(i - (digits_size - 9) / 2) * 25 + 100 + OptFilterPha];

  ATH_MSG_VERBOSE( "OptFilterPha=" << OptFilterPha << ((m_idocis) ? " cis" : " ") );

  if (m_of2) {
    OptFilterPed = 0.;
    for (i = 0; i < digits_size; i++) {
      OptFilterEne += a[i] * OptFilterDigits[i];
      OptFilterTime += b[i] * OptFilterDigits[i];
      OptFilterPed += c[i] * OptFilterDigits[i];
    }
  } else {
    for (i = 0; i < digits_size; i++) {
      OptFilterEne += a[i] * (OptFilterDigits[i] - OptFilterPed);
      OptFilterTime += b[i] * (OptFilterDigits[i] - OptFilterPed);
    }
  }

  bool goodEne = (fabs(OptFilterEne) > 1.0e-04);
  if (goodEne) {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterEne=" << OptFilterEne << endmsg;
      msg(MSG::VERBOSE) << "OptFilterTime*OptFilterEne=" << OptFilterTime << endmsg;
    }
    OptFilterTime /= OptFilterEne;
  } else {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterEne=" << OptFilterEne
                        << "   ... assuming 0.0" << endmsg;
      msg(MSG::VERBOSE) << "OptFilterTime*OptFilterEne=" << OptFilterTime
                        << "   ... assuming 0.0" << endmsg;
    }
    OptFilterTime = OptFilterEne = 0.0;
  }

  for (i = 0; i < digits_size; i++) {
    if (OptFilterDigits[i] > 0)
      OptFilterChi2 += 50 * fabs(OptFilterDigits[i] - (OptFilterEne * g[i] + OptFilterPed))
          / OptFilterDigits[i];
    else
      OptFilterChi2 += 50 * fabs(OptFilterDigits[i] - (OptFilterEne * g[i] + OptFilterPed)) / 1024.;
  }

  if (fabs(OptFilterChi2) > 1.0e-04 || goodEne) {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterTime=" << OptFilterTime << endmsg;
      msg(MSG::VERBOSE) << "OptFilterPed=" << OptFilterPed << endmsg;
      msg(MSG::VERBOSE) << "OptFilterChi2=" << OptFilterChi2 << endmsg;
    }
  } else {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterTime=" << OptFilterTime << endmsg;
      msg(MSG::VERBOSE) << "OptFilterPed=" << OptFilterPed << endmsg;
      msg(MSG::VERBOSE) << "OptFilterChi2=" << OptFilterChi2
                        << "   ... assuming 0.0" << endmsg;
    }
    OptFilterChi2 = 0.0;
  }

  return OptFilterChi2;
}


void TileRawChannelBuilderOptFilter::BuildPulseShape(vector<double> &m_pulseShape,
    vector<double> &m_pulseShapeX, vector<double> &m_pulseShapeT, int dignum) {

  ATH_MSG_DEBUG( "TileRawChannelBuilderOptFilter::BuildPulseShape");

//1: set m_pulseShape
  int shape_size = (dignum - 1) * 25 + 200;
  m_pulseShape.resize(shape_size);
  ATH_MSG_DEBUG( "Set dimension of m_pulseShape to shape_sie=" << shape_size);

//2: scan m_pulseShapeT for: tmin, tmax, nt0 and size: m_pulseShapeX[nt0]=1.0;
  int nt0 = 0, size;
  double tmin = 10000., tmax = -10000.;
  size = m_pulseShapeT.size();
  for (int i = 0; i < size; i++) {
    if (m_pulseShapeT[i] < tmin) tmin = m_pulseShapeT[i];
    if (m_pulseShapeT[i] > tmax) tmax = m_pulseShapeT[i];
    if (m_pulseShapeT[i] == 0) nt0 = i;
  }

  ATH_MSG_DEBUG( "m_pulseShapeX & m_pulseShapeT size =" << size
                << ", tmin=" << tmin
                << ", tmax=" << tmax
                << "  nt0=" << nt0
                << " m_pulseShapeT[nt0]=" << m_pulseShapeT[nt0]
                << " m_pulseShapeX[nt0]=" << m_pulseShapeX[nt0] );

//3: fill m_pulseShape
  bool exact;
  int nminn, nminp;
  double minn, minp, tdist;
  m_pulseShape[(shape_size) / 2] = m_pulseShapeX[nt0];
//  for (int i=1;i<(shape_size)/2+1;i++){
  for (int i = 1; i < (shape_size) / 2; i++) {
    // negative times: 0->(shape_size-1)/2
    if (-i < tmin)
      m_pulseShape[(shape_size) / 2 - i] = 0.;
    else {
      exact = false;
      minn = -10000.;
      minp = 10000.;
      nminn = 0;
      nminp = size - 1;
      for (int j = 0; j < nt0 + 1 && !exact; j++) {
        if (m_pulseShapeT[j] == double(-i)) {
          m_pulseShape[(shape_size) / 2 - i] = m_pulseShapeX[j];
          exact = true;
        } else {
          tdist = m_pulseShapeT[j] - double(-i);
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
                        << " m_pulseShape=" << m_pulseShape[(shape_size) / 2 - i]);
      } else {
        ATH_MSG_VERBOSE( "exact value NOT found for time=" << -i
                        << " nminn=" << nminn
                        << " m_pulseShapeT="
                        << m_pulseShapeT[nminn]
                        << " m_pulseShapeX=" << m_pulseShapeX[nminn] << std::endl
                        << " nminp=" << nminp
                        << " m_pulseShapeT="
                        << m_pulseShapeT[nminp]
                        << " m_pulseShapeX=" << m_pulseShapeX[nminp]);

        m_pulseShape[(shape_size) / 2 - i] = m_pulseShapeX[nminn]
            + (m_pulseShapeX[nminp] - m_pulseShapeX[nminn])
                / (m_pulseShapeT[nminp] - m_pulseShapeT[nminn]) * (-i - m_pulseShapeT[nminn]);
      }

    }

    // positive times: (shape_size-1)/2->shape_size
    if (i > tmax)
      m_pulseShape[(shape_size) / 2 + i] = 0.;
    else {
      exact = false;
      minn = -10000.;
      minp = 10000.;
      nminn = 0;
      nminp = size;
      for (int j = nt0; j < size && !exact; j++) {
        if (m_pulseShapeT[j] == double(i)) {
          m_pulseShape[(shape_size) / 2 + i] = m_pulseShapeX[j];
          exact = true;
        } else {
          tdist = m_pulseShapeT[j] - double(i);
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
                        << " m_pulseShape=" << m_pulseShape[(shape_size) / 2 + i]);
      } else {
        ATH_MSG_VERBOSE( "exact value NOT found for time=" << i
                        << " nminn=" << nminn
                        << " m_pulseShapeT=" << m_pulseShapeT[nminn]
                        << " m_pulseShapeX=" << m_pulseShapeX[nminn] << std::endl
                        << " nminp=" << nminp
                        << " m_pulseShapeT=" << m_pulseShapeT[nminp]
                        << " m_pulseShapeX=" << m_pulseShapeX[nminp] );

        m_pulseShape[(shape_size) / 2 + i] = m_pulseShapeX[nminn]
            + (m_pulseShapeX[nminp] - m_pulseShapeX[nminn])
                / (m_pulseShapeT[nminp] - m_pulseShapeT[nminn]) * (i - m_pulseShapeT[nminn]);
      }
    }
  }

//   if (msgLvl(MSG::VERBOSE)) {
//    for (int i = 1; i < shape_size; i++) {
//      msg(MSG::VERBOSE) << "shape[" << i << "]=" << m_pulseShape[i] << endmsg;
//    }
//  }
}

