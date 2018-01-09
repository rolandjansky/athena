/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// small hack to enable datapool usage
#include "TileEvent/TileRawChannel.h"

// Gaudi includes
#include "GaudiKernel/Property.h"

// Atlas includes
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileRecUtils/TileRawChannelBuilderMF.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
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
TileRawChannelBuilderMF::TileRawChannelBuilderMF(const std::string& type, const std::string& name,
    const IInterface *parent)
    : TileRawChannelBuilder(type, name, parent)
    , m_tileToolTiming("TileCondToolTiming")
    , m_tileCondToolOfc("TileCondToolOfc")
    , m_tileCondToolOfcOnFly("TileCondToolOfc")
    , m_tileToolNoiseSample("TileCondToolNoiseSample")
    , m_nSamples(0)
    , m_t0SamplePosition(0)
    , m_maxTime(0.0)
    , m_minTime(0.0)
{

  //declare interfaces
  declareInterface<TileRawChannelBuilder>(this);
  declareInterface<TileRawChannelBuilderMF>(this);

  m_rawChannelContainerKey = "TileRawChannelMF";

  //declare properties
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolOfc", m_tileCondToolOfc, "TileCondToolOfc");
  declareProperty("TileCondToolOfcOnFly", m_tileCondToolOfcOnFly, "TileCondToolOfc");
  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample);
  declareProperty("AmplitudeCorrection", m_correctAmplitude = false);
  declareProperty("PedestalMode", m_pedestalMode = 1);
  declareProperty("DefaultPedestal", m_defaultPedestal = 0.0);
  declareProperty("MF", m_MF = 0);
  declareProperty("MaxIterations", m_maxIterations = 5);
  declareProperty("BestPhase", m_bestPhase = false);
  declareProperty("TimeFromCOF", m_timeFromCOF = false);
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

  ATH_MSG_INFO("TileRawChannelBuilderMF::initialize()");

  m_rChType = TileFragHash::MF;

  // Initialize arrays for ped estimation
  memset(m_chPedCounter, 0, sizeof(m_chPedCounter));
  memset(m_chPed, 0, sizeof(m_chPed));

  // init in superclass
  CHECK(TileRawChannelBuilder::initialize());

  // bits 12-15 - various options
  if (m_correctAmplitude) m_bsflags |= 0x2000;
  if (m_maxIterations > 1) m_bsflags |= 0x4000;
  if (m_bestPhase) m_bsflags |= 0x8000;

  m_nSamples = m_tileInfo->NdigitSamples();
  m_t0SamplePosition = m_tileInfo->ItrigSample();
  m_maxTime = 25 * (m_nSamples - m_t0SamplePosition - 1);
  m_minTime = -25 * m_t0SamplePosition;

  CHECK(m_tileCondToolOfcOnFly.retrieve());
  //=== get TileCondToolOfc
  CHECK(m_tileCondToolOfc.retrieve());


  if (m_bestPhase) {
    //=== get TileToolTiming
    CHECK(m_tileToolTiming.retrieve());
  }

  //=== get TileCondToolNoiseSample
  CHECK(m_tileToolNoiseSample.retrieve());

  ATH_MSG_DEBUG("TileRawChannelBuilderMF::initialize() completed successfully");

  return StatusCode::SUCCESS;
}

/**
 *  Finalize
 */
StatusCode TileRawChannelBuilderMF::finalize() {

  ATH_MSG_DEBUG("Finalizing");

  return StatusCode::SUCCESS;
}

TileRawChannel* TileRawChannelBuilderMF::rawChannel(const TileDigits* tiledigits) {

  ++m_chCounter;
  int i, j, row, col;
  unsigned int k;
  double MFchi2 = 0.;

  const HWIdentifier adcId = tiledigits->adc_HWID();
  int gain = m_tileHWID->adc(adcId);
  int ros = m_tileHWID->ros(adcId);
  int drawer = m_tileHWID->drawer(adcId);
  int channel = m_tileHWID->channel(adcId);

  ATH_MSG_VERBOSE("Running Matched Filter for TileRawChannel with HWID " << m_tileHWID->to_string(adcId));

  /* get pedestal value assumed for reconstruction */
  //double digSigma = m_tileInfo->DigitsResolution(gain);
  // new way to get channel-dependent sigma:
  // but we lost difference between sigma used in digitization and 
  // sigma assumed in reconstruction - it's the same sigma now
  //double digSigma = m_tileInfo->DigitsPedSigma(adcId);
  /* Get vector of time-slice amplitudes. */
  m_digits = tiledigits->samples();
  double amp_ch = 0;      // Fitted amplitude of RC (to be returned from Filtering code).
  double cof[7] = { 0., 0., 0., 0., 0., 0., 0. };
  float ped_ch = 0;      // Ped retrieved from m_tileToolNoiseSample.
  double chisq_ch = 0.; // Chisq resulting from Filtering.
  double t_ch = 0.;     // Fitted time to be supplied by Filtering code
  double amp_norm = 0.;     // Normalization factor for MF method
  double amp_mf = 0.;
  float ped_aux = 0;
  float rms_aux = 0;
  float phase = 0;
  int err = 0;	//error for matrix handling

  float mindig, maxdig;

  // to treat issues discussed...
  if (are3FF(mindig, maxdig)) {

    ped_ch = 0.;
    if (mindig > 2046.99)
      chisq_ch = 9999.;
    else
      chisq_ch = 0.;

  } else {

    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    if (m_bestPhase) phase = (float) -m_tileToolTiming->getSignalPhase(drawerIdx, channel, gain);
    rms_aux = m_tileToolNoiseSample->getHfn(drawerIdx, channel, gain);

    switch (m_pedestalMode) {

      case -1:
        // use ped estimated from data based on the conditions value
        ped_aux = m_tileToolNoiseSample->getPed(drawerIdx, channel, gain);
        /*rms_aux = m_tileToolNoiseSample->getHfn(drawerIdx, channel, gain); */

        if (m_chPedCounter[ros][drawer][channel][gain] < 200) {

          if ((m_digits[0] < (ped_aux + 10)) && (m_digits[0] > (ped_aux - 10))) {

            ++m_chPedCounter[ros][drawer][channel][gain];
            m_chPed[ros][drawer][channel][gain] += m_digits[0];

          }
        }

        if (m_chPedCounter[ros][drawer][channel][gain] > 0) {
          ped_ch = m_chPed[ros][drawer][channel][gain] / m_chPedCounter[ros][drawer][channel][gain];
        } else {
          ped_ch = ped_aux;
        }
        break;

      case 0:
        // use fixed pedestal from jobOptions
        ped_ch = m_defaultPedestal;
        break;

      default:
        // use pedestal from conditions DB
        ped_ch = m_tileToolNoiseSample->getPed(drawerIdx, channel, gain);
        break;

    }

    // begin COF iteration for amplitude greater than m_ampMinThresh (15 ADC) and within +/- bunch spacing/2
    int n = 7;
    CLHEP::HepMatrix A(n, n, 0);
    int t;
    double signalModel[7];
    bool goodEne = true;
    t_ch = -phase;
    for (int it = 0; it < m_maxIterations; it++) {

      float ofcPhase(-t_ch);
      const TileOfcWeightsStruct* weights;
      weights = m_tileCondToolOfc->getOfcWeights(drawerIdx, channel, gain, ofcPhase, true);
      
      t_ch = -ofcPhase;

      double g[9] = {0};
      double b[9] = {0};
      double dg[9] = {0};

      for (k = 0; k < m_digits.size(); ++k) {
        b[k] = weights->w_b[k];
        g[k] = weights->g[k];
        dg[k] = weights->dg[k];
        if (it == 0) {
          amp_mf += g[k] * (m_digits[k] - ped_ch);  // matched filter
          amp_norm += g[k] * g[k];  // matched filter calibration for amp estimation
        }
      }

      if (it == 0) amp_mf = amp_mf / amp_norm;	// pure MF (for muon receiver board simulation)

      // build deconvolution matrix (keep OOT signals "in-time")
      if (it == 0) {
        t = 4;
        for (row = 0; row < n; row++) {
          t -= 1;
          for (col = 0; col < n; col++) {
            if (((col + t) > 6) || ((col + t) < 0)) {
              A[row][col] = 0.0;
            } else {
              A[row][col] = g[col + t];
            }

          }
        }
      } else {
        for (col = 0; col < n; col++) {
          A[3][col] = g[col];
        }
      }

      // apply deconvolution for pileup handling
      CLHEP::HepMatrix B(n, n, 0);
      err = 0;
      B = A;
      B.invert(err);

      double xDecon[7] = { 0, 0, 0, 0, 0, 0, 0 };
      for (j = 0; j < n; ++j) {
        for (i = 0; i < n; ++i) {
          xDecon[j] += (m_digits[i] - ped_ch) * B[i][j];
        }
      }

      // build the pile-up threshold based on deconvolution noise and test whether pileup is present
      double thr[7] = { 0, 0, 0, 0, 0, 0, 0 };
      for (j = 0; j < n; ++j) {
        for (i = 0; i < n; ++i) {
          thr[j] += (rms_aux * rms_aux) * (B[i][j] * B[i][j]);
        }
        thr[j] = sqrt(thr[j]);
      }

      // apply the pile-up threshold to detect OOT signals
      int pileupDet[7] = { 0, 0, 0, 1, 0, 0, 0 };
      int constraint = 1;
      for (i = 0; i < n; ++i) {
        if (i == 3) continue;
        if ((ros == 1) || (ros == 2)) {   // test 4*noise RMS from deconvolution matrix for barrel cells (low occupancy)
          if (xDecon[i] > 4 * thr[i]) {
            pileupDet[i] = 1;
            constraint += 1;
          }
        } else if ((ros == 3) || (ros == 4)) { // test 3*noise RMS from deconvolution matrix for extended barrel cells (higher occupancy)
          if (xDecon[i] > 3 * thr[i]) {
            pileupDet[i] = 1;
            constraint += 1;
          }
        }

      }

      // apply second step COF
      int allBCtrig = 0; // test if all BCs were triggered by decovolution, and if not, we can include the derivative for timing estimation
      if (m_timeFromCOF) {
        if (constraint < 7)
          constraint += 1;  // this line is to include the derivative in second step of COF, for the time estimation
        else
          allBCtrig = 1;
      }

      CLHEP::HepMatrix H(constraint, n, 0);

      t = 4;
      int rowAux = 0;
      for (row = 0; row < n; row++) {
        t -= 1;
        if (pileupDet[row] == 0) continue;
        for (col = 0; col < n; col++) {
          if (((col + t) > 6) || ((col + t) < 0)) {
            H[rowAux][col] = 0.0;
          } else {
            H[rowAux][col] = g[col + t];
          }

        }
        rowAux += 1;
      }

      // including the derivative to better estimate the time if we have less than 7 signals detected by DM
      if (m_timeFromCOF) {
        if (rowAux < 7) {
          for (col = 0; col < n; col++) {
            H[rowAux][col] = dg[col];
          }
        }
      }

      CLHEP::HepMatrix tH(n, constraint, 0);
      CLHEP::HepMatrix resultH(constraint, n, 0);
      CLHEP::HepMatrix multH(constraint, constraint, 0);
      tH = H.T();
      multH = H * tH;
      err = 0;
      multH.invert(err);
      resultH = multH * H;
      for (j = 0; j < n; j++) {    // initialize signal model to be built from COF amplitude estimates
        signalModel[j] = 0.0;
        cof[j] = 0.0;
      }
      int r = 0;
      int iBC3 = 0; // variable to be used in case of amplitude correction (m_correctAmplitude)
      if (m_MF == 1) {
        cof[3] = amp_mf;
        for (j = 0; j < n; j++) {
          signalModel[j] += cof[3] * A[3][j];
        }
      } else {
        for (i = 0; i < n; ++i) {
          if (pileupDet[i] == 0) continue;
          if (i == 3) iBC3 = r;
          for (j = 0; j < n; j++) {
            cof[i] += (m_digits[j] - ped_ch) * resultH[r][j];
          }
          for (j = 0; j < n; j++) {
            signalModel[j] += cof[i] * A[i][j];
          }
          r++;
        }
      }

      // computation of the timing, instead of taking the b weights from OF2, which are also not optimized for pileup
      double t_aux = 0.0;
      if (m_timeFromCOF) {
        for (j = 0; j < n; j++) {
          if (allBCtrig == 0)
            t_aux += (m_digits[j] - ped_ch) * (-resultH[rowAux][j]); // negative to be consistent
          else
            t_aux += m_digits[j] * b[j]; // use OF2 b weights (no need to subtract the pedestal), in the case all 7 BC are triggered by deconvolution
        }
      } else {
        for (j = 0; j < n; j++) {
          t_aux += m_digits[j] * b[j];  // use b weights from OF2 to calculate time
        }
      }

      amp_ch = cof[3];	// with COF, the amp_ch may be deprecated

      goodEne = (fabs(cof[3]) > 1.0e-04);
      if (goodEne) {
        t_aux /= cof[3];
        t_ch += t_aux;
      } else {
        t_ch = amp_ch = 0.0;
        for (i = 0; i < n; ++i) {
          cof[i] = 0.0;
        }
      }

      // condition for amplitude and time correction using iterative mode
      if ((m_maxIterations > 1) && ((cof[3] < m_ampMinThresh) || (t_ch < m_timeMinThresh || t_ch > m_timeMaxThresh)))
        break;

      // amplitude correction for central BC (same as parabolic correction)
      if (m_correctAmplitude && cof[3] > m_ampMinThresh && t_ch > m_timeMinThresh && t_ch < m_timeMaxThresh) {
        double correction = 0.0;
        ofcPhase = -t_ch;
        weights = m_tileCondToolOfcOnFly->getOfcWeights(drawerIdx, channel, gain, ofcPhase, true);
        for (j = 0; j < n; ++j) {
          correction += weights->g[j] * resultH[iBC3][j];
        }
        cof[3] += (1 - correction) * cof[3];
      }

    } //end of COF iteration

    t_ch += phase;

    if (t_ch < m_minTime) t_ch = m_minTime;
    if (t_ch > m_maxTime) t_ch = m_maxTime;

    if (m_calibrateEnergy) {
      amp_ch = m_tileInfo->CisCalib(adcId, amp_ch);
      for (i = 0; i < n; ++i) {
        cof[i] = m_tileInfo->CisCalib(adcId, cof[i]);
      }
    }

    // we know that time is zero here, put negagive chi^2 to indicate that
    for (k = 0; k < m_digits.size(); ++k) {
      double dqf = (signalModel[k] - (m_digits[k] - ped_ch));
      MFchi2 += dqf * dqf;
    }
    chisq_ch = sqrt(MFchi2);

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
  DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  float times[7] = { (float)t_ch, -75, -50, -25, 25, 50, 75};
  rawCh->assign (adcId,
                 std::begin(cof), std::end(cof),
                 std::begin(times), std::end(times),
                 &chisq_ch, &chisq_ch+1,
                 ped_ch);

  // correct order of amplitudes
  rawCh->setAmplitude(cof[3], 0);
  rawCh->setAmplitude(cof[0], 1);
  rawCh->setAmplitude(cof[1], 2);
  rawCh->setAmplitude(cof[2], 3);

  ATH_MSG_VERBOSE("Creating RawChannel" << " a=" << amp_ch << " t=" << t_ch << " q=" << chisq_ch);

  if (m_correctTime && (t_ch != 0 && t_ch < m_maxTime && t_ch > m_minTime)) {
    rawCh->insertTime(m_tileInfo->TimeCalib(adcId, t_ch));
    ATH_MSG_VERBOSE("Correcting time, new time=" << rawCh->time());

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

bool TileRawChannelBuilderMF::are3FF(float &dmin, float &dmax) {
  bool allSaturated = true;
  bool jump = false;

  unsigned int nSamp = m_digits.size();
  if (nSamp) {
    dmin = dmax = m_digits[0];

    for (unsigned int i = 1; i < nSamp; ++i) {
      float dig = m_digits[i];
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
        float smp = m_digits[i];
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

    ATH_MSG_VERBOSE( "  TileRawChannelBuilderMF::Are3FF()"
                    << "  mindig= " << dmin
                    << "  maxdig= " << dmax
                    << "  allSat= " << allSaturated
                    << "  jump= " << jump);

    if (jump) {
      dmin = dmax = 4095.; // this is needed to set bad quality from Opt Filter later
    }

  } else {
    dmin = dmax = 0.0;
  }

  return (allSaturated || jump);
}

