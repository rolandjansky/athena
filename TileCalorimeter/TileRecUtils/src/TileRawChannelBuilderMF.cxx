/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
#include "TileRecUtils/TileRawChannelBuilderMF.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "TileRecUtils/TileFilterManager.h"
#include "TileRecUtils/TileFilterTester.h"
#include "TileRecUtils/TileRawChannelBuilderOpt2Filter.h"
#include "TileConditions/TileOptFilterWeights.h"
#include "TileConditions/TilePulseShapes.h"
#include "CLHEP/Matrix/Matrix.h"

// lang include
#include <algorithm>
#include <cmath>

// C++ STL includes
#include <vector>

// for matrix treatment
#include "CLHEP/Matrix/Matrix.h"


/**
 * Standard constructor
 */
TileRawChannelBuilderMF::TileRawChannelBuilderMF(const std::string& type
    , const std::string& name, const IInterface *parent)
    : TileRawChannelBuilder(type, name, parent)
    , m_tileToolTiming("TileCondToolTiming")
    , m_tileCondToolOfcCool("TileCondToolOfcCool")
    , m_tileToolNoiseSample("TileCondToolNoiseSample")
    , m_NSamp(0)
    , m_t0Samp(0)
    , m_maxTime(0.0)
    , m_minTime(0.0)
{
  //declare interfaces
  declareInterface< TileRawChannelBuilder >( this );
  declareInterface< TileRawChannelBuilderMF >(this);
    
  m_TileRawChannelContainerID = "TileRawChannelMF";

  //declare properties
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolOfcCool", m_tileCondToolOfcCool  ,"TileCondToolOfcCool");
  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample);
  declareProperty("AmplitudeCorrection", m_correctAmplitude = false);
  declareProperty("PedestalMode", m_pedestalMode = 1);
  declareProperty("DefaultPedestal", m_defPedestal = 0.0);
  declareProperty("MF", m_MF = 0);
}


/**
 * Destructor
 */
TileRawChannelBuilderMF::~TileRawChannelBuilderMF() {
}


/**
 * Initialize
 */
StatusCode TileRawChannelBuilderMF::initialize() {

  ATH_MSG_INFO( "TileRawChannelBuilderMF::initialize()" );

  m_rChType = TileFragHash::MF;

  // Initialize arrays for ped estimation
  memset(m_chPedCounter, 0, sizeof(m_chPedCounter));
  memset(m_chPed, 0, sizeof(m_chPed));

  // init in superclass
  CHECK( TileRawChannelBuilder::initialize() );
  
  // bits 12-15 - various options
  if (m_correctAmplitude) m_bsflags |= 0x2000;

  m_NSamp = m_tileInfo->NdigitSamples();
  m_t0Samp = m_tileInfo->ItrigSample();
  m_maxTime = 25 * (m_NSamp - m_t0Samp - 1);
  m_minTime = -25 * m_t0Samp;

  //=== get TileCondToolOfcCool
  CHECK( m_tileCondToolOfcCool.retrieve() );

  //=== get TileToolTiming
  CHECK( m_tileToolTiming.retrieve() );

   //=== get TileCondToolNoiseSample
  CHECK( m_tileToolNoiseSample.retrieve() );


  ATH_MSG_DEBUG( "TileRawChannelBuilderMF::initialize() completed successfully" );

  return StatusCode::SUCCESS;
}

/**
 *  Finalize
 */
StatusCode TileRawChannelBuilderMF::finalize() {

  ATH_MSG_DEBUG( "Finalizing" );
  
  return StatusCode::SUCCESS;
}

TileRawChannel* TileRawChannelBuilderMF::rawChannel(const TileDigits* tiledigits) {

  ++m_chCounter;
  int i = 0;
  int j = 0;
  unsigned int k = 0;
  double MFchi2 = 0.;

  const HWIdentifier adcId = tiledigits->adc_HWID();
  int gain = m_tileHWID->adc(adcId);
  int ros = m_tileHWID->ros(adcId);
  int drawer = m_tileHWID->drawer(adcId);
  int channel = m_tileHWID->channel(adcId);
  
  ATH_MSG_VERBOSE ( "Running Matched Filter for TileRawChannel with HWID "
                   << m_tileHWID->to_string(adcId) );

  /* get pedestal value assumed for reconstruction */
  //double digSigma = m_tileInfo->DigitsResolution(gain);
  // new way to get channel-dependent sigma:
  // but we lost difference between sigma used in digitization and 
  // sigma assumed in reconstruction - it's the same sigma now
  //double digSigma = m_tileInfo->DigitsPedSigma(adcId);
  /* Get vector of time-slice amplitudes. */
  digits = tiledigits->samples();
  double amp_ch = 0;      // Fitted amplitude of RC (to be returned from Filtering code).
  double cof[7] = { 0., 0., 0., 0., 0., 0., 0. };
  float ped_ch = 0;      // Ped retrieved from m_tileToolNoiseSample.
  double chisq_ch = 0.; // Chisq resulting from Filtering.
  double t_ch = 0.;     // Fitted time to be supplied by Filtering code
  double amp_norm=0.;     // Normalization factor for MF method
  double amp_mf = 0.; 
  float ped_aux = 0;
  //float rms_aux = 0;
  float phase = 0;
  int err = 0;	//error for matrix handling

  float mindig, maxdig;

  // to treat issues discussed...
  if (Are3FF(mindig, maxdig)) {
    
    ped_ch = 0.;
    if (mindig > 2046.99)
      chisq_ch = 9999.;
    else
      chisq_ch = 0.;

  } else {

    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    phase = (float) -m_tileToolTiming->getSignalPhase(drawerIdx, channel, gain);

    switch(m_pedestalMode) {

      case -1:
        // use pedestal from conditions DB
        ped_ch = m_tileToolNoiseSample->getPed(drawerIdx, channel, gain);
        break;

      case 0:
        // use fixed pedestal from jobOptions
        ped_ch = m_defPedestal;
        break;

      default:
        // use ped estimated from data based on the conditions value
        ped_aux = m_tileToolNoiseSample->getPed(drawerIdx, channel, gain);
        /*rms_aux = m_tileToolNoiseSample->getHfn(drawerIdx, channel, gain); */

        if (m_chPedCounter[ros][drawer][channel][gain] < 200) {

          if ((digits[0] < (ped_aux + 10)) && (digits[0] > (ped_aux - 10))) {

            ++m_chPedCounter[ros][drawer][channel][gain];
            m_chPed[ros][drawer][channel][gain] += digits[0];

          }
        }

        if (m_chPedCounter[ros][drawer][channel][gain] > 0) {
          ped_ch = m_chPed[ros][drawer][channel][gain] / m_chPedCounter[ros][drawer][channel][gain];
        } else {
          ped_ch = ped_aux;
        }
        break;

    }

    // apply the Matched Filter
    const TileOfcWeightsStruct* m_weights;
    m_weights = m_tileCondToolOfcCool->getOfcWeights(drawerIdx, channel, gain, phase, true);

    double g[9];
    //double dg[9];
    //double a[9];
    double b[9];
    double t_aux = 0;	//variable auxiliar to compute the MF time

    for (k = 0; k < digits.size(); ++k) {
      //a[k] = m_weights->w_a[k];
      b[k] = m_weights->w_b[k];
      g[k] = m_weights->g[k];
      amp_mf += g[k]*(digits[k]-ped_ch);  // matched filter
      amp_norm += g[k]*g[k];  // matched filter calibration for amp estimation
      t_aux += b[k] * digits[k];
      //dg[k] = m_weights->dg[k];
    }

    amp_mf = amp_mf/amp_norm;	// pure MF (for muon receiver board simulation)

    // apply deconvolution for pileup handling
    int n = 7;
    HepMatrix A(n, n, 0);

    int t = 4;
    for (int row = 0; row < n; row++) {
      t -= 1;
      for (int col = 0; col < n; col++) {
        if (((col + t) > 6) || ((col + t) < 0)) {
          A[row][col] = 0.0;
        } else {
          A[row][col] = g[col + t];
        }

      }
    }

    HepMatrix B(n, n, 0);
    err = 0;
    B = A;
    B.invert(err);

    double xDecon[7] = { 0, 0, 0, 0, 0, 0, 0 };
    for (j = 0; j < n; ++j) {
      for (i = 0; i < n; ++i) {
        xDecon[j] += (digits[i] - ped_ch) * B[i][j];
      }
    }

    // test whether pileup is present
    int thr = 0;
    if (gain == 0) {
      thr = 4;
    } else {
      thr = 9;
    }

    int pileupDet[7] = { 0, 0, 0, 1, 0, 0, 0 };
    int constraint = 1;
    for (i = 0; i < n; ++i) {
      if (i == 3) continue;
      if (xDecon[i] > thr) {
        pileupDet[i] = 1;
        constraint += 1;
      }

    }

    // apply GOF
    HepMatrix H(constraint, n, 0);

    t = 4;
    int rowAux = 0;
    //int ind4 = 0;
    for (int row = 0; row < n; row++) {
      t -= 1;
      if (pileupDet[row] == 0) continue;
      //if (row == 3) ind4 = rowAux;
      for (int col = 0; col < n; col++) {
        if (((col + t) > 6) || ((col + t) < 0)) {
          H[rowAux][col] = 0.0;
        } else {
          H[rowAux][col] = g[col + t];
        }

      }
      rowAux += 1;
    }

    HepMatrix tH(n, constraint, 0);
    HepMatrix resultH(constraint, n, 0);
    HepMatrix multH(constraint, constraint, 0);
    tH = H.T();
    multH = H * tH;
    err = 0;
    multH.invert(err);
    resultH = multH * H;
    double signalModel[7] = { 0., 0., 0., 0., 0., 0., 0. };	// signal model built from COF amplitude estimates
    int r = 0;
    if (m_MF == 1){
	cof[3] = amp_mf;
	for (j = 0; j< n; j++){ 
		signalModel[j] += cof[3] * A[3][j];
	}
    }	
    else{
    	for (i = 0; i < n; ++i) {

      	if (pileupDet[i] == 0) continue;

      	for (j = 0; j < n; j++) {
        	cof[i] += (digits[j] - ped_ch) * resultH[r][j];
      	}

      	for (j = 0; j < n; j++) {
        	signalModel[j] += cof[i] * A[i][j];
      	}

      	r++;

    	}
    }
    amp_ch = cof[3];	// with COF, no need for the amp_ch variable anymore
        
    // ped=0 means channel disconnect for physics runs/ work only for CIS runs
    // digits =1023, drawer off
    if ((ped_ch == 0) || (digits[0] == 1023)) {
      amp_ch = 0;
      for (i = 0; i < n; ++i) {
        cof[i] = 0;
      }
    }

    //bool goodEne = (fabs(amp_ch) > 1.0e-04);
    bool goodEne = (fabs(cof[3]) > 1.0e-04);
    if (goodEne) {
      //t_ch = t_aux/amp_ch;
      t_ch = t_aux / cof[3];
    } else {
      t_ch = amp_ch = 0.0;
      for (i = 0; i < n; ++i) {
        cof[i] = 0.0;
      }
    }

    // If weights for tau=0 are used, deviations are seen in the amplitude =>
    // function to correct the amplitude
    if (m_correctAmplitude && amp_ch > m_ampMinThresh && t_ch > m_timeMinThresh
        && t_ch < m_timeMaxThresh) {
      amp_ch *= correctAmp(t_ch);
      for (i = 0; i < n; ++i) {
        cof[i] *= correctAmp(t_ch);
      }

      ATH_MSG_VERBOSE ( "Amplitude corrected by " << correctAmp(t_ch)
                       << " new amplitude is " << amp_ch );
    }

    if (t_ch < m_minTime) t_ch = m_minTime;
    if (t_ch > m_maxTime) t_ch = m_maxTime;

    if (m_calibrateEnergy) {
      amp_ch = m_tileInfo->CisCalib(adcId, amp_ch);
      for (i = 0; i < n; ++i) {
        cof[i] = m_tileInfo->CisCalib(adcId, cof[i]);
      }
    }

    // we know that time is zero here, put negagive chi^2 to indicate that
    for (k = 0; k < digits.size(); ++k) {
      double dqf = (signalModel[k] - (digits[k] - ped_ch));
      MFchi2 += dqf * dqf;
    }
    chisq_ch = sqrt(MFchi2 / 7);

    if (fabs(chisq_ch) > 1.0e-04 || goodEne) {
      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "MFtime=" << t_ch << endmsg;
        msg(MSG::VERBOSE) << "MFped=" << ped_ch << endmsg;
        msg(MSG::VERBOSE) << "MFchi2=" << chisq_ch << endmsg;
      }
    } else {
      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "MFtime=" << t_ch << endmsg;
        msg(MSG::VERBOSE) << "MFped=" << ped_ch << endmsg;
        msg(MSG::VERBOSE) << "MFchi2=" << chisq_ch << "   ... assuming 0.0" << endmsg;
      }
      chisq_ch = 0.0;
    }

  }

  //  TileRawChannel *rawCh = new TileRawChannel(adcId,amp_ch,t_ch,chisq_ch);
  static DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  rawCh->m_adc_hwid = adcId;
  rawCh->m_amplitude.resize(7);
  rawCh->m_amplitude[0] = cof[3];
  rawCh->m_amplitude[1] = cof[0];
  rawCh->m_amplitude[2] = cof[1];
  rawCh->m_amplitude[3] = cof[2];
  rawCh->m_amplitude[4] = cof[4];
  rawCh->m_amplitude[5] = cof[5];
  rawCh->m_amplitude[6] = cof[6];
  rawCh->m_time.resize(7);
  rawCh->m_time[0] = t_ch;
  rawCh->m_time[1] = -75.;
  rawCh->m_time[2] = -50.;
  rawCh->m_time[3] = -25.;
  rawCh->m_time[4] = 25.;
  rawCh->m_time[5] = 50.;
  rawCh->m_time[6] = 75.;
  rawCh->m_quality.resize(1);
  rawCh->m_quality[0] = chisq_ch;
  rawCh->m_pedestal = ped_ch; // default value in TileRawChannel constructor

  ATH_MSG_VERBOSE ( "Creating RawChannel"
                   << " a=" << amp_ch
                   << " t=" << t_ch
                   << " q=" << chisq_ch );
  
  if (m_correctTime && (t_ch != 0 && t_ch < m_maxTime && t_ch > m_minTime)) {
    rawCh->insertTime(m_tileInfo->TimeCalib(adcId, t_ch));
    ATH_MSG_VERBOSE ( "Correcting time, new time=" << rawCh->time() );

  }

  if (TileID::HIGHGAIN == gain) {
    ++m_nChH;
    m_RChSumH += amp_ch;
  } else {
    ++m_nChL;
    m_RChSumL += amp_ch;
  }

  return rawCh;
}

bool TileRawChannelBuilderMF::Are3FF(float &dmin, float &dmax) {
  bool allSaturated = true;
  bool jump = false;

  unsigned int nSamp = digits.size();
  if (nSamp) {
    dmin = dmax = digits[0];

    for (unsigned int i = 1; i < nSamp; ++i) {
      float dig = digits[i];
      if (dig > dmax)
        dmax = dig;
      else if (dig < dmin) dmin = dig;
    }
    allSaturated = (dmin > 1022.99);

    // FIXME:: set these 2 parameters from JobOptions
    // FIXME:: move this method to base class 
    const float epsilon = 2.1; // allow 1 count fluctuations around const value
    const float delta = 99.9;  // consider jumps by 100 counts only
    const float level0 = 39.9; // jump from this level to zero is bad
    const float level1 = 99.9; // jump from this level to 1023 is bad 

    if (!allSaturated && (dmax - dmin) > delta) {
      float abovemin = dmax;
      float belowmax = dmin;
      unsigned int nmin = 0;
      unsigned int nmax = 0;
      //unsigned int pmin = nSamp;
      unsigned int pmax = nSamp;
      for (unsigned int i = 0; i < nSamp; ++i) {
        float smp = digits[i];
        if (smp - dmin < epsilon) {
          ++nmin;
          //pmin = i;
        }
        if (dmax - smp < epsilon) {
          ++nmax;
          pmax = i;
        }
        if (smp < abovemin && smp > dmin) {
          abovemin = smp;
        }
        if (smp > belowmax && smp < dmax) {
          belowmax = smp;
        }
      }

      if (dmin < 0.01 && dmax > 1022.99) { // jump from zero to saturation
        jump = true;
      } else if (dmin < 0.01 && abovemin > level0 && nmin > 1) { // at least two samples at zero, others - above pedestal
        jump = true;
      } else if (dmax > 1022.99 && belowmax < level1 && nmax > 1) { // at least two saturated. others - close to pedestal
        jump = true;
      } else if (nmax + nmin == nSamp) {
        if (nmax > 1 && nmin > 1) { // at least 2 samples at two distinct levels
          jump = true;
        } else if (nmax == 1) {
          if (pmax > 0 && pmax < nSamp - 1) { // jump up in one sample, but not at the edge
            jump = true;
          }
        } else if (nmin == 1) { // jump down in one sample
          jump = true;
        }
      }
    }

    ATH_MSG_VERBOSE ( "  TileRawChannelBuilderMF::Are3FF()"
                     << "  mindig= " << dmin
                     << "  maxdig= " << dmax
                     << "  allSat= " << allSaturated
                     << "  jump= " << jump );

    if (jump) {
      dmin = dmax = 4095.; // this is needed to set bad quality from Opt Filter later
    }

  } else {
    dmin = dmax = 0.0;
  }

  return (allSaturated || jump);
}

