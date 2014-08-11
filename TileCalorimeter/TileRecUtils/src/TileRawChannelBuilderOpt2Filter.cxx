/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// small hack to enable datapool usage
#define private public
#define protected public
#include "TileEvent/TileRawChannel.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#undef private
#undef protected

// Gaudi includes
#include "GaudiKernel/Property.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

// Atlas includes
#include "DataModel/DataPool.h"
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileRecUtils/TileRawChannelBuilderOpt2Filter.h"
#include "TileRecUtils/TileRawChannelBuilderOpt2FilterLookup.h"
#include "TileConditions/TileOptFilterWeights.h"
#include "TileConditions/TilePulseShapes.h"
#include "CLHEP/Matrix/Matrix.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileOptFilterWeights.h"

using namespace std;

//interface stuff
static const InterfaceID IID_ITileRawChannelBuilderOpt2Filter("TileRawChannelBuilderOpt2Filter", 1, 0);


const InterfaceID& TileRawChannelBuilderOpt2Filter::interfaceID() {
  return IID_ITileRawChannelBuilderOpt2Filter;
}


#define TILE_Opt2FilterBUILDERVERBOSE false

TileRawChannelBuilderOpt2Filter::TileRawChannelBuilderOpt2Filter(const std::string& type,
    const std::string& name, const IInterface *parent)
  : TileRawChannelBuilder(type, name, parent)
  , m_tileToolTiming("TileCondToolTiming")
  , m_tileCondToolOfc("TileCondToolOfc")
  , m_tileCondToolOfcCool("TileCondToolOfcCool")
  , m_tileToolNoiseSample("TileCondToolNoiseSample")
  , c_signal(0)
  , c_negat(0)
  , c_center(0)
  , c_const(0)
  , m_NSamp(0)
  , m_t0Samp(0)
  , m_maxTime(0.0)
  , m_minTime(0.0)
{
  //declare interfaces
  declareInterface< TileRawChannelBuilder >( this );
  declareInterface< TileRawChannelBuilderOpt2Filter >(this);

  m_TileRawChannelContainerID = "TileRawChannelOpt2";
  
  //declare properties
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolOfc",    m_tileCondToolOfc  ,"TileCondToolOfc");
  declareProperty("TileCondToolOfcCool",m_tileCondToolOfcCool  ,"TileCondToolOfcCool");
  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample,"TileCondToolNoiseSample");
  declareProperty("MaxIterations",m_maxIterations = 5);
  declareProperty("PedestalMode",m_pedestalMode = 17);
  declareProperty("TimeForConvergence",m_timeForConvergence = 0.5);
  declareProperty("ConfTB",m_ConfTB = false);
  declareProperty("OF2",m_of2 = true);
  declareProperty("Minus1Iteration",m_minus1Iter = false);
  declareProperty("AmplitudeCorrection",m_correctAmplitude = false);
  declareProperty("BestPhase",m_bestphase = false);
  declareProperty("OfcfromCool",m_ofcfromcool = false);
  declareProperty("EmulateDSP",m_emulatedsp = false);
}


TileRawChannelBuilderOpt2Filter::~TileRawChannelBuilderOpt2Filter() {
}


StatusCode TileRawChannelBuilderOpt2Filter::initialize() {

  ATH_MSG_INFO( "TileRawChannelBuilderOpt2Filter::initialize()" );

  m_rChType = TileFragHash::OptFilterOffline; // type for offline Opt Filter

  // init in superclass
  CHECK( TileRawChannelBuilder::initialize() );

  
  // bits 12-15 - various options
  if (m_correctAmplitude) m_bsflags |= 0x2000;
  if (m_maxIterations > 1) m_bsflags |= 0x4000;
  if (m_bestphase) m_bsflags |= 0x8000;

  ATH_MSG_DEBUG( " MaxIterations=" << m_maxIterations
                << " PedestalMode=" << m_pedestalMode
                << " TimeForConvergence=" << m_timeForConvergence
                << " ConfTB=" << m_ConfTB
                << " OF2=" << m_of2
                << " Minus1Iteration=" << m_minus1Iter
                << " AmplitudeCorrection=" << m_correctAmplitude
                << " Best Phase " << m_bestphase );

  m_NSamp = m_tileInfo->NdigitSamples();
  m_t0Samp = m_tileInfo->ItrigSample();
  m_maxTime = 25 * (m_NSamp - m_t0Samp - 1);
  m_minTime = -25 * m_t0Samp;
  ATH_MSG_DEBUG(" NSamples=" << m_NSamp
                << " T0Sample=" << m_t0Samp
                << " minTime=" << m_minTime
                << " maxTime=" << m_maxTime );

  if (m_pedestalMode % 10 > 2 && m_NSamp != m_pedestalMode % 10) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Changing PedestalMode from " << m_pedestalMode;
    m_pedestalMode = (m_pedestalMode / 10) * 10 + m_NSamp;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " to " << m_pedestalMode << endmsg;
  }


  if (m_NSamp != 7 && (m_pedestalMode == 71 || m_pedestalMode == 7621)) {
    ATH_MSG_ERROR( "Incompatable pedestal mode [" << m_pedestalMode
		   << "] and number of samples [" << m_NSamp << "]" );
    return StatusCode::FAILURE;
  }

  c_signal = 0;
  c_negat = 0;
  c_center = 0;
  c_const = 0;

  const IGeoModelSvc *geoModel = 0;
  CHECK( service("GeoModelSvc", geoModel) );
  
  // dummy parameters for the callback:
  int dummyInt = 0;
  std::list<std::string> dummyList;
  
  if (geoModel->geoInitialized()) {
    return geoInit(dummyInt, dummyList);
  } else {
    CHECK( detStore()->regFcn(&IGeoModelSvc::geoInit, geoModel,
        &TileRawChannelBuilderOpt2Filter::geoInit, this) );
  }

  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelBuilderOpt2Filter::geoInit(IOVSVC_CALLBACK_ARGS) {
  
  if (m_ofcfromcool) {
    //=== get TileCondToolOfcCool
    CHECK( m_tileCondToolOfcCool.retrieve() );
  } else {
    //=== get TileCondToolOfc
    CHECK( m_tileCondToolOfc.retrieve() );
  }
  
  //=== get TileCondToolNoiseSample
  CHECK( m_tileToolNoiseSample.retrieve() );


  if (m_bestphase) {
    //=== get TileToolTiming
    CHECK( m_tileToolTiming.retrieve() );
  }
  
  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelBuilderOpt2Filter::finalize() {

  if (msgLvl(MSG::VERBOSE)) {
    if (m_maxIterations == 1) { // Without iterations
      msg(MSG::VERBOSE) << "Counters: Signal=" << c_signal
                        << " Constant=" << c_const
                        << " Total=" << c_signal + c_const << endmsg;
    } else {
      msg(MSG::VERBOSE) << "Counters: Signal=" << c_signal
                        << " Negat=" << c_negat
                        << " Center=" << c_center
                        << " Constant=" << c_const
                        << " Total=" << c_signal + c_negat + c_const + c_center << endmsg;
    }
  }

  ATH_MSG_DEBUG( "Finalizing" );

  return StatusCode::SUCCESS;
}


TileRawChannel * TileRawChannelBuilderOpt2Filter::rawChannel(const TileDigits* digits) {

  ++m_chCounter;

  double OptFilterPed = 0., OptFilterEne = 0., OptFilterTime = 0., OptFilterChi2 = 0.;
  OptFilterDigits = digits->samples();
  const HWIdentifier adcId = digits->adc_HWID();
  int OptFilterGain = m_tileHWID->adc(adcId);
  
  ATH_MSG_VERBOSE( "Building Raw Channel, with OptFilter, HWID:" << m_tileHWID->to_string(adcId)
                  << " gain=" << OptFilterGain );

  int ros = m_tileHWID->ros(adcId);
  int drawer = m_tileHWID->drawer(adcId);
  int channel = m_tileHWID->channel(adcId);
  OptFilterChi2 = Filter(ros, drawer, channel, OptFilterGain, OptFilterPed, OptFilterEne,
      OptFilterTime);
  
  if (m_calibrateEnergy) {
    OptFilterEne = m_tileInfo->CisCalib(adcId, OptFilterEne);
  }
  
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Creating OptFilter RawChannel"
                      << " a=" << OptFilterEne
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
      && (OptFilterTime != 0
          && OptFilterTime < m_maxTime
          && OptFilterTime > m_minTime)) {

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


float TileRawChannelBuilderOpt2Filter::FindMaxDigit() {

  ATH_MSG_VERBOSE( "  TileRawChannelBuilderOpt2Filter::FindMaxDigit()" );

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

    if (saturated)  msg(MSG::VERBOSE) << " Samples saturated" << endmsg;
  }
  
  return imaxdig;
}


float TileRawChannelBuilderOpt2Filter::SetPedestal(int ros, int drawer, int channel, int gain) {
  float pedestal = 0.;
  
  switch (m_pedestalMode) {
    case -1:
      // use pedestal from conditions DB
      pedestal = m_tileToolNoiseSample->getPed(TileCalibUtils::getDrawerIdx(ros, drawer), channel, gain);
      break;
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
    case 71:
      pedestal = std::min(OptFilterDigits[0], OptFilterDigits[6]);
      break;
    case 7621:
      pedestal = 0.5 * std::min(OptFilterDigits[0] + OptFilterDigits[1], OptFilterDigits[5] + OptFilterDigits[6]); 
      break;
    default:
      pedestal = OptFilterDigits[0];
      break;
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "  TileRawChannelBuilderOpt2Filter::SetPedestal()" << endmsg;
    msg(MSG::VERBOSE) << "      pedestal=" << pedestal << endmsg;
  }
  
  return pedestal;
}


double TileRawChannelBuilderOpt2Filter::Filter(int ros, int drawer, int channel
    , int &OptFilterGain, double &OptFilterPed, double &OptFilterEne, double &OptFilterTime) {

  ATH_MSG_VERBOSE( "TileRawChannelBuilderOpt2Filter::Filter" );

  OptFilterEne = 0.;
  OptFilterTime = 0.;
  double OptFilterChi2 = 0.;

  std::vector<float>::const_iterator idig = OptFilterDigits.begin();
  std::vector<float>::const_iterator iend = OptFilterDigits.end();
  float mindig = (*idig);
  float maxdig = mindig;

  for (++idig; idig != iend; ++idig) {
    float samp = (*idig);
    if (samp < mindig)
      mindig = samp;
    else if (samp > maxdig) maxdig = samp;
  }
  
  if (maxdig - mindig < 0.01) { // constant value in all samples

    OptFilterPed = mindig;
    OptFilterChi2 = 0.;
    ATH_MSG_VERBOSE( "CASE NO SIGNAL: maxdig-mindig = " << maxdig << "-" << mindig
                    << " = " << maxdig - mindig );

    c_const++;

  } else {

    OptFilterPed = SetPedestal(ros, drawer, channel, OptFilterGain);
    double OptFilterPha = 0.;
    int niter = 0;

    if (m_maxIterations == 1) {  // Without iterations
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      // AA 3.10.08 --- take best phase from COOL
      if (m_bestphase) {
        // AS 19.11.09 - note minus sign here - time in DB is opposite to best phase 
        OptFilterPha = -m_tileToolTiming->getSignalPhase(drawerIdx, channel, OptFilterGain);
        ATH_MSG_VERBOSE( "Best phase: " << OptFilterPha
                        << " drawerIdx " << drawerIdx
                        << " channel " << channel );
      }
      
      OptFilterChi2 = Compute(ros, drawer, channel, OptFilterGain, OptFilterPed
          , OptFilterEne, OptFilterTime, OptFilterPha);

      // If weights for tau=0 are used, deviations are seen in the amplitude =>
      // function to correct the amplitude
      if (m_correctAmplitude
          && OptFilterEne > m_ampMinThresh
          && OptFilterTime > m_timeMinThresh
          && OptFilterTime < m_timeMaxThresh) {

        OptFilterEne *= correctAmp(OptFilterTime);
        ATH_MSG_VERBOSE( "Amplitude corrected by " << correctAmp(OptFilterTime)
                        << " new amplitude is " << OptFilterEne );
      }

      if (OptFilterTime > m_maxTime) OptFilterTime = m_maxTime;
      if (OptFilterTime < m_minTime) OptFilterTime = m_minTime;

      c_signal++;

    } else { // With iterations => 3 cases defined for correct pedestal treatment

      // values used for pedestal-like events when iterations are performed
      const int sigma_hi = 5;
      const int sigma_lo = 3;
      int sigma = (OptFilterGain) ? sigma_hi : sigma_lo;
      int digits_size_1 = OptFilterDigits.size() - 1;

      // Signal events: OF with iterations
      if ((maxdig - OptFilterDigits[0] > sigma)
          || (OptFilterDigits[0] - OptFilterDigits[digits_size_1] > 4 * sigma)) {

        ATH_MSG_VERBOSE( "CASE Signal: maxdig-OptFilterDigits[0]="
                        << maxdig - OptFilterDigits[0]
                        << "   OptFilterDigits[0]-OptFilterDigits[ digits_size_1]="
                        << OptFilterDigits[0] - OptFilterDigits[digits_size_1] );

        niter = Iterator(ros, drawer, channel, OptFilterGain, OptFilterPed
            , OptFilterEne, OptFilterTime, OptFilterChi2);

        ATH_MSG_VERBOSE( "number of iterations= " << niter );

        c_signal++;
      } else if (OptFilterDigits[0] - mindig > sigma) { //Pedestal events: OF with negative iterations

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "CASE NEGATIVE: maxdig-OptFilterDigits[0]="
                            << maxdig - OptFilterDigits[0]
                            << "   OptFilterDigits[0]-OptFilterDigits[digits_size_1]="
                            << OptFilterDigits[0] - OptFilterDigits[digits_size_1] << endmsg;
          msg(MSG::VERBOSE) << "OptFilterDigits[0]-mindig="
                            << OptFilterDigits[0] - mindig << endmsg;
        }

        for (int i = 0; i <= digits_size_1; i++)  // Mirror around pedestal
          OptFilterDigits[i] = OptFilterPed - (OptFilterDigits[i] - OptFilterPed);

        niter = Iterator(ros, drawer, channel, OptFilterGain, OptFilterPed
            , OptFilterEne, OptFilterTime, OptFilterChi2);

        OptFilterEne = -OptFilterEne;

        c_negat++;

      } else { // Gaussian center: no iterations and phase=0

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "CASE CENTER: maxdig-OptFilterDigits[0]="
                            << maxdig - OptFilterDigits[0]
                            << "   OptFilterDigits[0]-OptFilterDigits[digits_size_1]="
                            << OptFilterDigits[0] - OptFilterDigits[digits_size_1] << endmsg;
          msg(MSG::VERBOSE) << "OptFilterDigits[0]-mindig="
                            << OptFilterDigits[0] - mindig
                            << endmsg;
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


int TileRawChannelBuilderOpt2Filter::Iterator(int ros, int drawer, int channel, int OptFilterGain,
    double &OptFilterPed, double &OptFilterEne, double &OptFilterTime, double &OptFilterChi2) {

  ATH_MSG_VERBOSE( "TileRawChannelBuilderOpt2Filter::Iterator()" );

  int niter = 0;
  double save_phase = 0.0;
  double OptFilterPha = 0.0;
  OptFilterTime = -1000.;
  
  // Mythic -1 iteration or DSP emulation case
  if (m_minus1Iter || (m_emulatedsp && (m_maxIterations > 1)))
    OptFilterPha = 25 * (m_t0Samp - FindMaxDigit());

  while ((OptFilterTime > m_timeForConvergence
          || OptFilterTime < (-1.) * m_timeForConvergence
          || m_emulatedsp)
         && niter < m_maxIterations) {

    OptFilterChi2 = Compute(ros, drawer, channel, OptFilterGain, OptFilterPed
                            , OptFilterEne, OptFilterTime, OptFilterPha);

    save_phase = OptFilterPha;

    if (m_emulatedsp)
      OptFilterPha -= round(OptFilterTime); // rounding phase to integer like in DSP
    else if (m_ofcfromcool)
      OptFilterPha -= round(OptFilterTime * 10.) / 10.; // rounding phase to 0.1 - OFC in DB are stored with 0.1ns steps
    else
      OptFilterPha -= OptFilterTime; // no rounding at all for OFC on the fly

    if (OptFilterPha > m_maxTime) OptFilterPha = m_maxTime;
    if (OptFilterPha < m_minTime) OptFilterPha = m_minTime;

    ++niter;
    ATH_MSG_VERBOSE( " OptFilter computed with phase=" << save_phase
                    << " Time=" << OptFilterTime
                    << " END ITER=" << niter
                    << " new phase=" << OptFilterPha
                    << " chi2=" << OptFilterChi2
                    << "  Amp=" << OptFilterEne );
  }
  
  OptFilterTime -= save_phase;
  if (OptFilterTime > m_maxTime) OptFilterTime = m_maxTime;
  if (OptFilterTime < m_minTime) OptFilterTime = m_minTime;
  
  ATH_MSG_VERBOSE( "OptFilterEne=" << OptFilterEne
                  << " Phase=" << save_phase
                  << " Absolute Time=" << OptFilterTime );

  return niter;
}


double TileRawChannelBuilderOpt2Filter::Compute(int ros, int drawer, int channel, int OptFilterGain,
    double &OptFilterPed, double &OptFilterEne, double &OptFilterTime, double OptFilterPha) {

 ATH_MSG_VERBOSE( "TileRawChannelBuilderOpt2Filter::Compute();"
                 << " ros=" << ros
                 << " drawer="  << drawer
                 << " channel=" << channel
                 << " gain=" << OptFilterGain );

  int i = 0, digits_size = OptFilterDigits.size();
  double OptFilterChi2 = 0.;
  double a[9];
  double b[9];
  double c[9];
  double g[9];
  double dg[9];
  
  OptFilterEne = 0.;
  OptFilterTime = 0.;
  float phase = (float) OptFilterPha;

  unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
  const TileOfcWeightsStruct* m_weights;
  if (m_ofcfromcool) {
    m_weights = m_tileCondToolOfcCool->getOfcWeights(drawerIdx, channel, OptFilterGain, phase, m_of2);
  } else {
    m_weights = m_tileCondToolOfc->getOfcWeights(drawerIdx, channel, OptFilterGain, phase, m_of2);
  }

  for (i = 0; i < digits_size; ++i) {
    a[i] = m_weights->w_a[i];
    b[i] = m_weights->w_b[i];
    g[i] = m_weights->g[i];
    dg[i] = m_weights->dg[i];
    if (m_of2) c[i] = m_weights->w_c[i]; // [OptFilterPha+100];
  }

  // for DSP emulation
  short a_int[9], ascale = 0, calib = 0, calib_offset = 0;
  short b_int[9], bscale = 0;
  unsigned short scale = 0, round = 0, lut = 0;
  int slope_scale = 0;
  int OptFilterEneDSP = 0, OptFilterTimeDSP = 0;
  if (m_emulatedsp) {
    OptFilterEneDSP = 0;
    OptFilterTimeDSP = 0;
    slope_scale = (int) truncf(log(pow(2., 15) - 1.) / log(2.));
    calib_offset = (short) roundf(pow(2., slope_scale));
    ofc2int(digits_size, a, a_int, ascale);
    ofc2int(digits_size, b, b_int, bscale);
    calib = ascale + slope_scale;
  }
  
  ATH_MSG_VERBOSE( "OptFilterPha=" << OptFilterPha );

  if (m_of2) {
    if (m_emulatedsp) {
      OptFilterPed = OptFilterDigits[0];
      for (i = 0; i < digits_size; ++i) {
        OptFilterEneDSP += a_int[i] * ((int) OptFilterDigits[i]);
        OptFilterTimeDSP += b_int[i] * ((int) OptFilterDigits[i]);
        OptFilterEne += a[i] * OptFilterDigits[i]; //aa temp
        OptFilterTime += b[i] * OptFilterDigits[i]; //aa temp
      }
    } else {
      OptFilterPed = 0.;
      for (i = 0; i < digits_size; ++i) {
        OptFilterEne += a[i] * OptFilterDigits[i];
        OptFilterTime += b[i] * OptFilterDigits[i];
        OptFilterPed += c[i] * OptFilterDigits[i];
      }
    }
  } else {
    if (m_emulatedsp) OptFilterPed = OptFilterDigits[0];
    for (i = 0; i < digits_size; ++i) {
      OptFilterEne += a[i] * (OptFilterDigits[i] - OptFilterPed);
      OptFilterTime += b[i] * (OptFilterDigits[i] - OptFilterPed);
    }
  }

  if (m_emulatedsp) {
    round = 1 << (ascale - 2);
    short OptFilterE2DSP = (unsigned short) ((OptFilterEneDSP + round) >> (ascale - 1));
    size_t OptInd = abs(OptFilterE2DSP);
    if (OptInd >= (sizeof(lookup) / sizeof(short))) OptInd = 0;
    lut = lookup[OptInd];
    scale = bscale - 4 + lookup[0] - 9;
    round = 1 << (scale - 1);
    //    int told=OptFilterTimeDSP;
    OptFilterTimeDSP = (((OptFilterTimeDSP + 0x100) >> 9) * lut + round) >> scale;
    //        printf(" 1 OptFilterTime %f OptFilterTimeDSP %d e2 %d round %d lut %d bscale %d scale %d told %d\n",OptFilterTime/OptFilterEne,OptFilterTimeDSP,OptFilterE2DSP,round,lut,bscale,scale,told);
    OptFilterTime = OptFilterTimeDSP / 16.0;

    //  printf(" 1 OptFilterEneDSP %d calib_offset  %d calib %d \n",OptFilterEneDSP,calib_offset,calib);

    OptFilterEneDSP = (OptFilterEneDSP + 1024) >> 11;
    //  printf(" 2 OptFilterEneDSP %d  \n",OptFilterEneDSP);
    OptFilterEneDSP = (OptFilterEneDSP * calib_offset + (1 << (calib - 15 - 1))) >> (calib - 15);
    //  printf(" 3 OptFilterEneDSP %d  \n",OptFilterEneDSP);
    double goffset = (OptFilterGain == 0) ? 512 : 2048;
    OptFilterEneDSP = (int) (OptFilterEneDSP + goffset);
    OptFilterEne = (OptFilterEneDSP - goffset) / 16.;
  }

  bool goodEne = (fabs(OptFilterEne) > 1.0e-04);
  if (goodEne) {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterEne=" << OptFilterEne << endmsg;
      msg(MSG::VERBOSE) << "OptFilterTime*OptFilterEne=" << OptFilterTime << endmsg;
    }
    if (!m_emulatedsp) OptFilterTime /= OptFilterEne;
  } else {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "OptFilterEne=" << OptFilterEne
                        << "   ... assuming 0.0" << endmsg;
      msg(MSG::VERBOSE) << "OptFilterTime*OptFilterEne=" << OptFilterTime
                        << "   ... assuming 0.0" << endmsg;
    }
    OptFilterTime = OptFilterEne = 0.0;
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
    double dqf = OptFilterDigits[i] - OptFilterEne * g[i] + OptFilterEne * OptFilterTime * dg[i]
        - OptFilterPed;
    OptFilterChi2 += dqf * dqf;
  }
  OptFilterChi2 = sqrt(OptFilterChi2);
  // new QF  }
  
  //  std::cout << "emulate " << m_emulatedsp << " OptFilterEne " << OptFilterEne << " OptFilterDigits[3]" << OptFilterDigits[3] << " OptFilterTime="<<OptFilterTime<<" OptFilterPed="<<OptFilterPed<<" OptFilterChi2="<<OptFilterChi2<<" g 3 " << g[3] << " dg 1 3 5 " << dg[1] << " " << dg[3] << " " << dg[5] <<std::endl;
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


void TileRawChannelBuilderOpt2Filter::ofc2int(int ndigits, double* w_off, short* w_int,
    short &scale) {
  // Number of bits of the integer word (signed -1 )
  int NumberBits = 16;
  NumberBits = NumberBits - 1;
  
  // Get Absolute Maximum
  double max = -10000.;
  double sum = 0.;
  for (int i = 0; i < ndigits; i++) {
    sum += w_off[i];
    if (fabs(w_off[i]) > max) max = fabs(w_off[i]);
    //    printf("idig = %d weight  = %f \n",i, w_off[i]);
  }
  if (fabs(sum) > max) max = fabs(sum);
  
  // Get Scale at Maximum
  scale = 0;
  if (max != 0) scale = (int) truncf(log((pow(2., NumberBits) - 1.) / max) / log(2.));
  
  // Convert to Integer the weights and the sum
  for (int i = 0; i < ndigits; i++)
    w_int[i] = (short) roundf(w_off[i] * pow(2., scale));
  //aa  w_sum_dsp = (short) roundf(sum*pow(2.,scale));
  

/* 
  if (msgLvl(MSG::VERBOSE)) {
    printf("\nAbsolute Max value = %15.8f -> Scale = %3d\n",max,scale);

    for (int i=0; i<ndigits; i++)
      {
	if ( i == 0){
	  printf("\n        Offline              Off*Scale             Dsp/scale    Dsp      Scale  \n");
	  printf("----------------------------------------------------------------------------------\n");
	  printf("  %17.10f     %17.10f    %17.10f  0x%04X    %3d   \n",
		 w_off[i],w_off[i]*pow(2.,scale), w_int[i]/pow(2.,scale),(unsigned int) w_int[i] ,scale);
	} else {
	  printf("  %17.10f     %17.10f    %17.10f  0x%04X   \n",
		 w_off[i],w_off[i]*pow(2.,scale), w_int[i]/pow(2.,scale),(unsigned int)w_int[i]);
	}
      }
    //aa    printf("  %17.10f     %17.10f    %17.10f  0x%04X     <- SUM\n",
    //aa	   sum,sum*pow(2.,scale),w_sum_dsp/pow(2.,scale),(unsigned int)w_sum_dsp);
  }
*/  
  return;
}

