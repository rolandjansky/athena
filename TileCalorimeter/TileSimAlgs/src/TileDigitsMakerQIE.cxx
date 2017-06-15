/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
//  Filename : TileDigitsMakerQIE.cxx
//  Author   : Zhifang
//  Created  : Feb 2006 from TileHitToDigits
//
//  DESCRIPTION:
//      Created to simulate the digits information (amplitudes of N time-slices,
//      with N about 9) received by the ROD's.)
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Gaudi includes
#include "GaudiKernel/ISvcLocator.h"

// Atlas include
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"

// Tile includes
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolNoiseSample.h"
//#include "TileConditions/TilePulseShapes.h"
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileSimAlgs/TileDigitsMakerQIE.h"

//CLHEP includes
#include <CLHEP/Random/Randomize.h>
#include <CLHEP/Units/SystemOfUnits.h>

// For the Athena-based random numbers.
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaKernel/Units.h"

// Pile up
#include "PileUpTools/PileUpMergeSvc.h"

//#include "TMatrixF.h"
//#include "TDecompChol.h"
//#include "cmath"

using CLHEP::RandGaussQ;
using CLHEP::RandBinomial;
using CLHEP::RandFlat;
using Athena::Units::MeV;

//
// Constructor
//
TileDigitsMakerQIE::TileDigitsMakerQIE(std::string name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_mergeSvc(0), 
  m_tileID(0), 
  m_tileTBID(0), 
  m_tileHWID(0), 
  m_tileInfo(0), 
  m_cabling(0), 
  m_DQstatus(0), 
  m_nSamples(0), 
  m_iTrig(0), 
  m_adcMax(0.0), 
  m_adcMaxHG(0.0), 
  m_tileNoise(false), 
  m_tileCoherNoise(false), 
  m_tileThresh(false), 
  m_tileThreshHi(0.0), 
  m_tileThreshLo(0.0), 
  m_nShapeHi(0), 
  m_nBinsPerXHi(0), 
  m_binTime0Hi(0), 
  m_timeStepHi(0.0), 
  m_nShapeLo(0), 
  m_nBinsPerXLo(0), 
  m_binTime0Lo(0), 
  m_timeStepLo(0.0), 
  m_pHRengine(0), 
  m_rndmSvc("AtRndmGenSvc", name), 
  m_tileToolEmscale("TileCondToolEmscale"), 
  m_tileToolNoiseSample("TileCondToolNoiseSample"), 
  m_tileToolPulseShape("TileCondToolPulseShape"), 
  m_tileBadChanTool("TileBadChanTool"), 
  m_beamInfo("TileBeamInfoProvider/TileBeamInfoProvider"),
  m_time_offset(-2.0F)
{
  declareProperty("TileHitContainer", m_hitContainer = "TileHitCnt");
  declareProperty("TileDigitsContainer", m_digitsContainer = "TileDigitsQIE");
  declareProperty("TileFilteredContainer", m_filteredContainer = "TileDigitsFltQIE");
  declareProperty("FilterThreshold", m_filterThreshold = 100.0 * MeV, "Threshold on filtered digits (default - 100 MeV)");
  declareProperty("FilterThresholdMBTS", m_filterThresholdMBTS = 0.0 * MeV, "Threshold on filtered digits of MBTS (default - 0 MeV)");
  declareProperty("TileInfoName", m_infoName = "TileInfo");
  declareProperty("IntegerDigits", m_integerDigits = true);
  declareProperty("CalibrationRun", m_calibRun = false);
  declareProperty("RndmSvc", m_rndmSvc, "Random Number Service used in TileDigitsMakerQIE");
  declareProperty("TileCondToolEmscale", m_tileToolEmscale, "Service to calibrate all channels");
  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample);
  declareProperty("RndmEvtOverlay", m_rndmEvtOverlay = false, "Pileup and/or noise added by overlaying random events (default=false)");
  declareProperty("TileCondToolPulseShape", m_tileToolPulseShape);
  declareProperty("UseCoolPulseShapes", m_useCoolPulseShapes = true, "Pulse shapes from database (default=true)");
  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("MaskBadChannels", m_maskBadChannels = false, "Remove channels tagged bad (default=false)");

}

TileDigitsMakerQIE::~TileDigitsMakerQIE() {
  std::vector<HWIdentifier *>::iterator itr = m_all_ids.begin();
  std::vector<HWIdentifier *>::iterator last = m_all_ids.end();
  for (; itr != last; ++itr) {
    delete[] (*itr);
  }

  std::vector<double *>::iterator itr1 = m_drawerBufferHi.begin();
  std::vector<double *>::iterator last1 = m_drawerBufferHi.end();
  for (; itr1 != last1; ++itr1) {
    delete[] (*itr1);
  }

  std::vector<double *>::iterator itr2 = m_drawerBufferLo.begin();
  std::vector<double *>::iterator last2 = m_drawerBufferLo.end();
  for (; itr2 != last2; ++itr2) {
    delete[] (*itr2);
  }
}

//
// Alg standard initialize function
//
StatusCode TileDigitsMakerQIE::initialize() {
  // retrieve TileID helper and TileInfo from det store

  CHECK(detStore()->retrieve(m_tileID));

  CHECK(detStore()->retrieve(m_tileTBID));

  CHECK(detStore()->retrieve(m_tileHWID));

  CHECK(detStore()->retrieve(m_tileInfo, m_infoName));

  //=== get TileCondToolEmscale
  CHECK(m_tileToolEmscale.retrieve());

  //=== get TileCondToolNoiseSample
  CHECK(m_tileToolNoiseSample.retrieve());

  m_cabling = TileCablingService::getInstance();

  /* Get needed parameters from tileInfo. */
  m_nSamples = m_tileInfo->NdigitSamples(); // number of time slices for each chan
  m_iTrig = m_tileInfo->ItrigSample(); // index of the triggering time slice
  m_adcMax = (float) m_tileInfo->ADCmax(); // adc saturation value used in assignment
  m_adcMaxHG = m_adcMax - 0.5; // value of switch from high to low gain
  m_tileNoise = m_tileInfo->TileNoise(); // (true => generate noise in TileDigits)
  m_tileCoherNoise = m_tileInfo->TileCoherNoise(); // (true => generate coherent noise in TileDigits)
  m_tileThresh = m_tileInfo->TileZeroSuppress(); // (true => apply threshold to Digits)
  m_tileThreshHi = m_tileInfo->ThresholdDigits(TileID::HIGHGAIN);
  m_tileThreshLo = m_tileInfo->ThresholdDigits(TileID::LOWGAIN);


  CHECK(m_rndmSvc.retrieve());
  m_pHRengine = m_rndmSvc->GetEngine("Tile_DigitsMakerQIE");

  ATH_MSG_DEBUG("Event Overlay: " << ((m_rndmEvtOverlay) ? "true" : "false"));
  ATH_MSG_DEBUG("Masking Channels: " << ((m_maskBadChannels) ? "true" : "false"));
  ATH_MSG_DEBUG("Using pulse shapes from COOL: " << ((m_useCoolPulseShapes) ? "true" : "false"));


  m_maskBadChannels = false; //set to false for QIE.
  if (m_maskBadChannels || m_rndmEvtOverlay) {
    CHECK(m_tileBadChanTool.retrieve());
  }

  if (m_rndmEvtOverlay) {
    m_tileNoise = false;
    m_tileCoherNoise = false;
    m_tileThresh = false;
    m_calibRun = false;

    ATH_MSG_INFO("Pileup and/or noise added by overlaying digits of random events");

    // locate the PileUpMergeSvc and initialize our local ptr
    CHECK(service("PileUpMergeSvc", m_mergeSvc));

    ATH_MSG_INFO("PileUpMergeSvc successfully initialized");

    ATH_MSG_DEBUG("Retrieving TileBeamInfoProvider in TileDigitsMakerQIE");
    CHECK(m_beamInfo.retrieve());

  } else {
    if (msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << "Obtained info from TileInfo" << endmsg;
      msg(MSG::INFO) << "tileNoise=" << ((m_tileNoise) ? "true" : "false") << ", tileCoherNoise=" << ((m_tileCoherNoise) ? "true" : "false") << ", tileThresh="
          << ((m_tileThresh) ? "true" : "false");
      if (m_tileThresh)
        msg(MSG::INFO) << ", thresh(hi,lo)=" << m_tileThreshHi << "," << m_tileThreshLo << endmsg;
      else
        msg(MSG::INFO) << endmsg;
    }
  }

  if (m_calibRun)
    m_filteredContainer = "";
  if (m_filteredContainer.size() > 0) {
    ATH_MSG_INFO("Keep digits with hit energy above " << m_filterThreshold / MeV << " MeV in " << m_filteredContainer << " container");
    ATH_MSG_INFO("Keep digits from MBTS with original G4 hit energy above " << m_filterThresholdMBTS / MeV << " MeV ");
  } else {
    m_filterThreshold = HUGE_VALL;
    m_filterThresholdMBTS = HUGE_VALL;
  }

  /* ==================================*/
  // Store HWID's for all 12288 channels (48 channels in each of 64 drawers).
  IdContext drawer_context = m_tileHWID->drawer_context();
  int ndrawers = m_tileHWID->drawer_hash_max();
  const int nchMax = 48; // number of channels per drawer

  ATH_MSG_DEBUG("ndrawers=" << ndrawers << " nchMax=" << nchMax << " HIGAIN=" << TileID::HIGHGAIN << " LOWGAIN=" << TileID::LOWGAIN);

  /* Store all (12288) Identifiers for the calorimeter adc's for HIGHAIN */
  m_all_ids.reserve(ndrawers);
  for (int dr = 0; dr < ndrawers; ++dr) {
    HWIdentifier drawer_id;
    m_tileHWID->get_id(dr, drawer_id, &drawer_context);
    HWIdentifier* adc_ids = new HWIdentifier[nchMax];
    int ros = m_tileHWID->ros(drawer_id);
    if (ros > 0) {
      int drawer = m_tileHWID->drawer(drawer_id);
      IdentifierHash idhash;
      m_tileHWID->get_hash(drawer_id, idhash, &drawer_context);
      int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      if (drawerIdx != (int) idhash) {
        ATH_MSG_ERROR("drawer " << m_tileHWID->to_string(drawer_id, -2) << " hash " << idhash << " NOT EQUAL to idx " << drawerIdx);
      } else if (msgLvl(MSG::VERBOSE) && m_cabling->connected(ros, drawer)) {
        msg(MSG::VERBOSE) << "drawer " << m_tileHWID->to_string(drawer_id, -2) << " hash " << idhash << endmsg;
      }

      for (int ch = 0; ch < nchMax; ++ch) {
        adc_ids[ch] = m_tileHWID->adc_id(drawer_id, ch, TileID::HIGHGAIN);
        if (msgLvl(MSG::VERBOSE) && m_cabling->getTestBeam() && m_cabling->connected(ros, drawer) && !m_rndmEvtOverlay) {
          double pedSimHi = m_tileInfo->DigitsPedLevel(TileID::HIGHGAIN, ch, idhash);
          double pedSimLo = m_tileInfo->DigitsPedLevel(TileID::LOWGAIN, ch, idhash);
          double sigmaNoiseHi = m_tileInfo->DigitsPedSigma(TileID::HIGHGAIN, ch, idhash);
          double sigmaNoiseLo = m_tileInfo->DigitsPedSigma(TileID::LOWGAIN, ch, idhash);
          msg(MSG::VERBOSE) << "Ch " << m_tileHWID->to_string(drawer_id, -2) << "/" << ch 
                            << " pedHi=" << pedSimHi 
                            << " pedLo=" << pedSimLo 
                            << " rmsHi=" << sigmaNoiseHi
                            << " rmsLo=" << sigmaNoiseLo << endmsg;
        }
      }
    }
    m_all_ids.push_back(adc_ids);
  }

  /* ==================================*/
  /* Create buffers to contain all digits in a single drawer. */
  m_drawerBufferHi.reserve(nchMax);
  m_drawerBufferLo.reserve(nchMax);
  int nSamp = m_tileInfo->NdigitSamples();
  for (int ich = 0; ich < nchMax; ++ich) {
    double * pDigitSamplesHi = new double[nSamp];
    double * pDigitSamplesLo = new double[nSamp];
    m_drawerBufferHi.push_back(pDigitSamplesHi);
    m_drawerBufferLo.push_back(pDigitSamplesLo);
  }

  //calculate the GeV to p.e. conversion coefficients
  for (int i = 0; i < 7; ++i) {
    Identifier pmt_id;
    switch (i) { // invent pmt_id in a given part(sampling) of the calorimeter
    case 6:
      //pmt_id = m_tileTBID->channel_id(-1/*side*/, 0/*phi*/, 2/*radius*/);
      // use the same ID as for normal E-cell - to obtain the same Npe at the end
      pmt_id = m_tileID->pmt_id(3/*section*/, 1/*side*/, 0/*module*/, 11/*tower*/, 3/*sample*/, 0);
      break; // E4' cell uses special ID, similar to MBTS
    case 5:
      pmt_id = m_tileTBID->channel_id(1/*side*/, 0/*phi*/, 1/*radius*/);
      break; // MBTS outer cell
    case 4:
      pmt_id = m_tileTBID->channel_id(1/*side*/, 0/*phi*/, 0/*radius*/);
      break; // MBTS inner cell
    case 3:
      pmt_id = m_tileID->pmt_id(3/*section*/, 1/*side*/, 0/*module*/, 11/*tower*/, 3/*sample*/, 0);
      break; // gap/crack
    default:
      pmt_id = m_tileID->pmt_id(1/*section*/, 1/*side*/, 0/*module*/, 0/*tower*/, i/*sample*/, 0); // normal cell
      break;
    }
    // take number of photoelectrons per GeV divide by 1000 to go to MeV
    // and multiply by inverted sampling fraction (35.9)
    // to get number of photoelectrons per 1 MeV energy in scintillator
    m_nPhotoElectrons[i] = (m_tileInfo->NPhElec(i)) / (Gaudi::Units::GeV / Gaudi::Units::MeV) * m_tileInfo->HitCalib(pmt_id);

    //ATH_MSG_DEBUG( i << " " << m_tileInfo->NPhElec(i) << " " << m_tileInfo->HitCalib(pmt_id) );

  }

  /* ==================================*/
  ATH_MSG_INFO("... initialization completed");

  return StatusCode::SUCCESS;
}

StatusCode TileDigitsMakerQIE::execute() {
  ATH_MSG_DEBUG("Executing ...");

  static bool first = (msgLvl(MSG::VERBOSE) && !m_rndmEvtOverlay && !m_cabling->getTestBeam());
  if (first) {
    ATH_MSG_VERBOSE("Dumping 2G noise parameters");
    first = false;
    IdContext drawer_context = m_tileHWID->drawer_context();
    int ndrawers = m_tileHWID->drawer_hash_max();
    const int nchMax = 48; // number of channels per drawer
    for (int dr = 0; dr < ndrawers; ++dr) {
      HWIdentifier drawer_id;
      m_tileHWID->get_id(dr, drawer_id, &drawer_context);
      int ros = m_tileHWID->ros(drawer_id);
      int drawer = m_tileHWID->drawer(drawer_id);
      if (m_cabling->connected(ros, drawer)) {
        IdentifierHash idhash;
        m_tileHWID->get_hash(drawer_id, idhash, &drawer_context);
        for (int ch = 0; ch < nchMax; ++ch) {
          double pedSimHi = m_tileToolNoiseSample->getPed(idhash, ch, TileID::HIGHGAIN);
          double sigmaHi_Hfn1 = m_tileToolNoiseSample->getHfn1(idhash, ch, TileID::HIGHGAIN);
          double sigmaHi_Hfn2 = m_tileToolNoiseSample->getHfn2(idhash, ch, TileID::HIGHGAIN);
          double sigmaHi_Norm = m_tileToolNoiseSample->getHfnNorm(idhash, ch, TileID::HIGHGAIN);
          double pedSimLo = m_tileToolNoiseSample->getPed(idhash, ch, TileID::LOWGAIN);
          double sigmaLo_Hfn1 = m_tileToolNoiseSample->getHfn1(idhash, ch, TileID::LOWGAIN);
          double sigmaLo_Hfn2 = m_tileToolNoiseSample->getHfn2(idhash, ch, TileID::LOWGAIN);
          double sigmaLo_Norm = m_tileToolNoiseSample->getHfnNorm(idhash, ch, TileID::LOWGAIN);
          ATH_MSG_VERBOSE("Channel " << m_tileHWID->to_string(drawer_id, -2) << "/" << ch 
                          << " pedHi=" << pedSimHi 
                          << " pedLo=" << pedSimLo 
                          << " rmsHi=" << sigmaHi_Hfn1 << "," << sigmaHi_Hfn2 << "," << sigmaHi_Norm 
                          << " rmsLo=" << sigmaLo_Hfn1 << "," << sigmaLo_Hfn2 << "," << sigmaLo_Norm);

        }
      }
    }
  }

  // step1: Get hit container from TES
  const TileHitContainer* hitCont;
  CHECK(evtStore()->retrieve(hitCont, m_hitContainer));

  // Zero sums for monitoring.
  int nChSum = 0;
  int nChHiSum = 0;
  int nChLoSum = 0;
  int nChHiAcc = 0;
  int nChLoAcc = 0;
  int nChHiFlt = 0;
  int nChLoFlt = 0;
  int nChHiCut = 0;
  int nChLoCut = 0;
  double echtot_Acc = 0.;
  double echint_Acc = 0.;
  double echtot_Cut = 0.;
  double echint_Cut = 0.;
  double HitSum = 0.;
  double EneSum = 0.;
  double RChSum = 0.;

  /* step2: Set up  Digits container */
  TileDigitsContainer* pDigitsContainer;
  pDigitsContainer = new TileDigitsContainer(true);

  TileDigitsContainer* pFilteredContainer = 0;
  if (m_filteredContainer.size() > 0)
    pFilteredContainer = new TileDigitsContainer(true, SG::VIEW_ELEMENTS);

  /* Set up buffers for handling information in a single collection. */
  IdentifierHash idhash;
  IdContext drawer_context = m_tileHWID->drawer_context();
  HWIdentifier * adc_ids;
  const int nchMax = 48; // number of channels per drawer
  int igain[nchMax];
  int ntot_ch[nchMax];
  double ech_tot[nchMax];
  double ech_int[nchMax];
  int over_gain[nchMax];
  memset(over_gain, -1, sizeof(over_gain));

  /* Make a vector of digits (to be filled at the end from m_drawerBuffer arrays) */
  std::vector<float> digitsBuffer(m_nSamples);
  std::vector<float> digitsBufferLo(m_nSamples); // for calib runs

  TileDigitsContainer::const_iterator collItrRndm;
  TileDigitsContainer::const_iterator lastCollRndm;
  if (m_rndmEvtOverlay) {

    typedef PileUpMergeSvc::TimedList<TileDigitsContainer>::type TimedDigitContList;
    TileDigitsContainer::const_iterator rndm_digititer_begin;
    TileDigitsContainer::const_iterator rndm_digititer_end;
    TileDigitsContainer::const_iterator rndm_digititer;

    TimedDigitContList digitContList;
    CHECK(m_mergeSvc->retrieveSubEvtsData("TileDigitsCnt", digitContList));
    ATH_MSG_DEBUG("TileDigitsCnt successfully retrieved ");

    if (digitContList.size() == 0) {
      ATH_MSG_WARNING("No overlay done ... ");
      return StatusCode::SUCCESS;
    }

    TimedDigitContList::iterator iTzeroDigitCont(digitContList.begin());
    const TileDigitsContainer * rndm_digit_container = (iTzeroDigitCont->second);
    collItrRndm = rndm_digit_container->begin();
    lastCollRndm = rndm_digit_container->end();

    typedef PileUpMergeSvc::TimedList<TileRawChannelContainer>::type TimedRawChanContList;
    TimedRawChanContList rawchanContList;
    if (!(m_mergeSvc->retrieveSubEvtsData("TileRawChannelCnt", rawchanContList).isSuccess())) {
      ATH_MSG_ERROR("Cannot retrieve TileRawChannelContainer for DQ check");
    } else {
      ATH_MSG_DEBUG("TileRawChannelContainer for DQ check retrieved");
    }
    TimedRawChanContList::iterator iTzeroRawChanCont(rawchanContList.begin());
    const TileRawChannelContainer * rndm_rawchan_container = (iTzeroRawChanCont->second);
    ATH_MSG_DEBUG("... setContainer method being called");
    m_beamInfo->setContainers(rndm_digit_container, rndm_rawchan_container);
    ATH_MSG_DEBUG("Containers successfully set in TileBeamInfoProvider");
    m_DQstatus = m_beamInfo->getDQstatus();
  }

  // iterate over all collections in a container
  TileHitContainer::const_iterator collItr = hitCont->begin();
  TileHitContainer::const_iterator lastColl = hitCont->end();

  /* ----------------------------------------------------------------- */
  /* Begin loop over the Hit collections.  All collections are defined */
  /* (even if they have no hits), and all the digit information        */
  /* including pileup events are contained in the collection.          */
  /*-------------------------------------------------------------------*/

  for (; collItr != lastColl; ++collItr) {

    /* Get array of HWID's for this drawer (stored locally). */
    HWIdentifier drawer_id = m_tileHWID->drawer_id((*collItr)->identify());
    int ros = m_tileHWID->ros(drawer_id);
    int drawer = m_tileHWID->drawer(drawer_id);
    int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    if (m_cabling->connected(ros, drawer)) {
      ATH_MSG_VERBOSE("ROS " << ros << " drawer " << drawer << " is connected");
    } else {
      if (m_rndmEvtOverlay && collItrRndm != lastCollRndm) {
        ++collItrRndm; // skip also one drawer in digi overlay container
      }
      continue;
    }

    m_tileHWID->get_hash(drawer_id, idhash, &drawer_context);
    adc_ids = m_all_ids[idhash];

    /* Initialize gain settings.  If noise is requested, all channels are */
    /* set to be active.  If not, set them all to be inactive (gain=-1).  */
    /* Only those which contain actual hits will be set active when the   */
    /* hits are read in.                                                  */
    int igainch = -1;
    if (m_tileNoise || m_tileCoherNoise || m_rndmEvtOverlay) {
      igainch = TileID::HIGHGAIN;
      if (m_rndmEvtOverlay)
        memset(over_gain, -1, sizeof(over_gain));
    }
    for (int ich = 0; ich < nchMax; ++ich) {
      igain[ich] = igainch;
      ech_tot[ich] = 0.;
      ech_int[ich] = 0.;
      ntot_ch[ich] = 0;
      double * pDigitSamplesHi = m_drawerBufferHi[ich];
      double * pDigitSamplesLo = m_drawerBufferLo[ich];
      for (int js = 0; js < m_nSamples; ++js) {
        pDigitSamplesHi[js] = 0.;
        pDigitSamplesLo[js] = 0.;
      }
    }

    if (m_rndmEvtOverlay && collItrRndm != lastCollRndm) {

      if ((*collItr)->identify() != (*collItrRndm)->identify()) {
        ATH_MSG_ERROR( "Frag IDs for hit collection and digits overlay collection do not match " << MSG::hex << (*collItr)->identify() 
                       << " != " << (*collItrRndm)->identify() << MSG::dec);
        return StatusCode::FAILURE;
      }

      // iterate over all digits in a collection
      TileDigitsCollection::const_iterator digitItr = (*collItrRndm)->begin();
      TileDigitsCollection::const_iterator lastDigit = (*collItrRndm)->end();

      for (; digitItr != lastDigit; ++digitItr) {

        /* Get digit HWIdentifier (= adc_id) */
        HWIdentifier adcId = (*digitItr)->adc_HWID();
        int channel = m_tileHWID->channel(adcId);
        int gain = m_tileHWID->adc(adcId);

        igain[channel] = gain;

        // get channel status
        bool good_dq = m_DQstatus->isAdcDQgood(ros, drawer, channel, gain);
        bool good_ch = (!m_tileBadChanTool->getAdcStatus(drawerIdx, channel, gain).isBad());

        // get digits
        std::vector<float> digits = (*digitItr)->samples();
        // get number of time samples & compare with nSamp
        int nSamp2 = digits.size();
        int goodDigits = nSamp2;

        float dig(2047.), digmin(65536.), digmax(-65536.);
        if (good_dq) {
          for (int js = 0; js < nSamp2; ++js) {
            dig = digits[js];
            if (dig < digmin)
              digmin = dig;
            if (dig > digmax)
              digmax = dig;
          }
        } else if (goodDigits > 0) {
          goodDigits = 0;
          dig = digmin = digmax = digits[0];
          for (int js = 1; js < nSamp2; ++js) {
            dig = digits[js];
            if (dig < digmin)
              digmin = dig;
            else if (dig > digmax)
              digmax = dig;
          }
        }

        if (goodDigits > 0) {
          over_gain[channel] = gain;
          if (nSamp2 != m_nSamples) {
            digits.resize(m_nSamples);
            for (int js = nSamp2; js < m_nSamples; ++js)
              digits[js] = digits[js - 1]; // repeat last value in vector (nSamp-nSamp2) times
          }

          double * buffer;
          double * bufferLG = 0;
          if (gain == TileID::HIGHGAIN) {

            if ( good_ch) { // to ignore pure noise in HG (less than 0.1 count effect in LG)
              float ratio = m_tileToolEmscale->doCalibCisOnl(drawerIdx, channel, TileID::HIGHGAIN, 1.) 
                / m_tileToolEmscale->doCalibCisOnl(drawerIdx, channel, TileID::LOWGAIN, 1.); // ratio between low and high gain

              bufferLG = m_drawerBufferLo[channel];
              for (int js = 0; js < m_nSamples; ++js) // in case low gain background is needed later
                bufferLG[js] = digits[js] * ratio; // put in low gain buffer signal divided by 64 (LG/HG ratio)
            }

            buffer = m_drawerBufferHi[channel];
          } else {
            buffer = m_drawerBufferLo[channel];
          }
          for (int js = 0; js < m_nSamples; ++js)
            buffer[js] = digits[js];

          if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "RNDM BG ADC " << m_tileHWID->to_string(adcId) << " samples=";
            for (int js = 0; js < m_nSamples; ++js)
              msg(MSG::VERBOSE) << " " << buffer[js];
            if (!good_ch)
              msg(MSG::VERBOSE) << " BCH";
            if (!good_dq) {
              msg(MSG::VERBOSE) << " BDQ";
            } else if (bufferLG) {
              msg(MSG::VERBOSE) << "  LG=";
              for (int js = 0; js < m_nSamples; ++js)
                msg(MSG::VERBOSE) << " " << int(bufferLG[js] * 100) / 100.;
            }
            msg(MSG::VERBOSE) << endmsg;
          }

        } else if (nSamp2 > 0) {
          over_gain[channel] = 10 + gain; // flag problematic channel

          double * buffer;
          if (gain == TileID::HIGHGAIN) {
            buffer = m_drawerBufferHi[channel];
          } else {
            buffer = m_drawerBufferLo[channel];
          }

          if (digmin != digmax || (dig != 0. && dig != 1023.))
            dig = 2047.; // keep only 0 or 1023 as it is
          for (int js = 0; js < m_nSamples; ++js)
            buffer[js] = dig;

          if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "BAD BG  ADC " << m_tileHWID->to_string(adcId) << " samples=";
            for (int js = 0; js < nSamp2; ++js)
              msg(MSG::VERBOSE) << " " << digits[js];
            msg(MSG::VERBOSE) << ((good_ch) ? "" : " BCH") << ((good_dq) ? "" : " BDQ") << endmsg;
          }

        } else {
          ATH_MSG_VERBOSE("NO BG   ADC " << m_tileHWID->to_string(adcId) 
                          << " samples= 0 0 0 0 0 0 0" << ((good_ch) ? "" : " BCH") << ((good_dq) ? "" : " BDQ"));
        }
      }
      ++collItrRndm; // skip to next digi collection
    }

    // iterate over all hits in a collection
    TileHitCollection::const_iterator hitItr = (*collItr)->begin();
    TileHitCollection::const_iterator lastHit = (*collItr)->end();
    bool signal_in_channel[nchMax];
    memset(signal_in_channel, 0, sizeof(signal_in_channel));

    for (; hitItr != lastHit; ++hitItr) {

      /* Get hit Identifier (= pmt_ID) and needed parameters for this channel */
      Identifier pmt_id = (*hitItr)->pmt_ID();
      double mbts_extra_factor = (m_tileTBID->is_tiletb(pmt_id)) ? -1.0 : 1.0;
      HWIdentifier channel_id = (*hitItr)->pmt_HWID();
      int ich = m_tileHWID->channel(channel_id);
      signal_in_channel[ich] = true;

      if (over_gain[ich] > 9) {
        if (msgLvl(MSG::DEBUG)) {
          int n_hits = (*hitItr)->size();
          double e_hit(0.);
          for (int ihit = 0; ihit < n_hits; ++ihit) {
            e_hit += (*hitItr)->energy(ihit);
          }
          HitSum += e_hit;
          e_hit *= m_tileInfo->HitCalib(pmt_id);
          EneSum += e_hit;
          ech_tot[ich] += e_hit;
          ntot_ch[ich] += n_hits;
          ATH_MSG_VERBOSE("BAD Overlay digits - skip hit in channel " << m_tileHWID->to_string(channel_id, -1));
        }
        continue;
      } else {
        ATH_MSG_VERBOSE("new hit in channel " << m_tileHWID->to_string(channel_id, -1));
      }


      /* Set gain=high and get digitSamples and calibration for this channel. */
      if (igain[ich] < 0)
        igain[ich] = TileID::HIGHGAIN;
      // conversion from scintillator energy to total cell energy (sampling fraction)
      double hit_calib = m_tileInfo->HitCalib(pmt_id);

      // conversion to ADC counts for high gain
      double efactorHi = hit_calib / m_tileToolEmscale->channelCalib(drawerIdx, ich, 
                                                                     TileID::HIGHGAIN, 1., 
                                                                     TileRawChannelUnit::ADCcounts, 
                                                                     TileRawChannelUnit::MegaElectronVolts);

      double* pDigitSamplesHi = m_drawerBufferHi[ich];

      /* Loop over the subhits for this channel.  For each one,
       convolute with shaping function and add to digitSamples.                  */
      int n_hits = (*hitItr)->size();
      for (int ihit = 0; ihit < n_hits; ++ihit) {
        /* Get hit energy and convert to amplitude of high-gain and low-gain channel */
        double e_hit = (*hitItr)->energy(ihit);
        double amp_ch = e_hit * efactorHi;
        double ech_sub = e_hit * hit_calib;
        double t_hit = (*hitItr)->time(ihit);

        ech_tot[ich] += ech_sub;
        //Sasha: QIE sees charge in 25 ns buckets
        if (fabs(t_hit) < 50.0) // ene within +/- 50 ns, used for filtered digits cut
          ech_int[ich] += ech_sub * mbts_extra_factor;
        ntot_ch[ich] += 1;
        nChSum += 1;
        HitSum += e_hit;
        EneSum += ech_sub;
        RChSum += amp_ch;
        
        //Sasha: QIE integrates in 25 ns buckets
        //Sasha: WLS decay time is not simulated for 3-in-1's
        //Sasha: Photostatitics (total #p.e. per a PMT pulse was done in TileHitVecToCnt
        //Sasha: Tail of the PMT distribution needs to have binomial statistics?
 

        int js0, js1;
        //m_time_offset [ns] offset of the integration window to the in-time hits with slightly negative time into the sample
        if ((t_hit - m_time_offset) >= 0) {
          js0 = m_iTrig + (int) ((t_hit - m_time_offset) / 25.); //QIE integrates charge every 25 ns.
          js1 = js0 + 1;
        } else {
          js0 = m_iTrig - 1 + (int) ((t_hit - m_time_offset) / 25.); //QIE integrates charge every 25 ns.
          js1 = js0 + 1;
        }


        if ((js0 >= 0 && js0 < m_nSamples) || (js1 >= 0 && js1 < m_nSamples)) {
          int pmt_sample = (m_tileTBID->is_tiletb(pmt_id)) ? TileID::SAMP_X + m_tileTBID->channel(pmt_id) : m_tileID->sample(pmt_id);

          double pe = e_hit * m_nPhotoElectrons[pmt_sample]; //number of photo-electrons
          int int_pt = (int) pe;

          double fraction_2nd_samle = 0.; //random number: leakage into the following sample
          if (int_pt > 0) {
            fraction_2nd_samle = RandBinomial::shoot(m_pHRengine, int_pt, 0.16); //16% of total charge is in the 2nd sample
            fraction_2nd_samle = fraction_2nd_samle / (double) int_pt;
          }

          if (js0 >= 0 && js0 < m_nSamples) {
            pDigitSamplesHi[js0] += amp_ch * (1. - fraction_2nd_samle);
          }

          if (js1 >= 0 && js1 < m_nSamples) {
            pDigitSamplesHi[js1] += amp_ch * fraction_2nd_samle;
          }

        }


        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "subHit:  ch=" << ich 
                            << " e_hit=" << e_hit 
                            << " t_hit=" << t_hit 
                            << " SamplesHi[" << m_iTrig << "]=" << pDigitSamplesHi[m_iTrig] << endmsg;
        }
      } /* end loop over sub-hits */

    } /* end loop over hits for this collection. */

    /* Now all signals for this collection are stored in m_drawerBuffer,
     accessed with pDigitSamplesHi and pDigitSampleLo. */
    if (msgLvl(MSG::VERBOSE)) {
      for (int ich = 0; ich < nchMax; ++ich) {
        if (igain[ich] > -1) {
          double * pDigitSamplesHi = m_drawerBufferHi[ich];
          msg(MSG::VERBOSE) << "total:  ADC " << m_tileHWID->to_string(adc_ids[ich], -1) << "/" << igain[ich] 
                            << " nhit=" << ntot_ch[ich] 
                            << " e_ch=" << ech_tot[ich] 
                            << " AinTHi=" << pDigitSamplesHi[m_iTrig] << endmsg;
        }
      }
    }

    /* ---------------------------------------------------------------      */
    /* Now all signals for this drawer are stored in m_drawerBuffer arrays, */
    /* and we are finished with TileHits for this collection.  Loop over    */
    /* channels to add noise and pedestal.  Check for saturation for        */
    /* each channel, and in case of saturation convert to low gain.         */
    /* -------------------------------------------------------------- */

    // looping over channels
    for (int ich = 0; ich < nchMax; ++ich) {
      /* If igain<0, channel is inactive => skip it.                    */
      if (igain[ich] < 0)
        continue;

      bool chanLoIsBad = false;
      bool chanHiIsBad = false;

      /* Generate the nSamp Digits for high gain. Check each for saturation. */
      ++nChHiSum;
      HWIdentifier adc_id = adc_ids[ich];
      HWIdentifier adc_id_lo; // for calib runs
      Identifier pmt_id = m_cabling->h2s_pmt_id(adc_id);
      ATH_MSG_DEBUG("Ch " << m_tileHWID->to_string(adc_id, -1) 
                    << " PMT " << (pmt_id.is_valid() ? m_tileID->to_string(pmt_id, -1) : (signal_in_channel[ich] ? "fake gap" : "not connected"))
                    << " gain=" << igain[ich]);
      if (m_calibRun) {
        ++nChLoSum;
        adc_id_lo = m_tileHWID->adc_id(drawer_id, ich, TileID::LOWGAIN);
      }
      /* Get pedestal and noise values */
      double pedSimHi(0.), pedSimLo(0.);
      bool good_ch = (over_gain[ich] < 9);

      double * pDigitSamplesHi = m_drawerBufferHi[ich];

      ATH_MSG_DEBUG(" Channel " << ros << '/' << drawer << '/' << ich 
                    << " sampHi=" << pDigitSamplesHi[m_iTrig] 
                    << " pedHi=" << pedSimHi);

      // looping over samples
      for (int js = 0; js < m_nSamples; ++js) {
        digitsBuffer[js] = pDigitSamplesHi[js];
        //igain[ich] = TileID::HIGHGAIN;
      }

      if (m_calibRun) { // calib run - keep both low and high gain

        if (chanHiIsBad) {
          for (int js = 0; js < m_nSamples; ++js) {
            digitsBuffer[js] = 2047;
          }
          ATH_MSG_DEBUG("Masking Channel " << ros << '/' << drawer << '/' << ich << "/1 HG");
        }

        TileDigits* pDigits = new TileDigits(adc_id, digitsBuffer);
        pDigitsContainer->push_back(pDigits);

        if (chanLoIsBad) {
          for (int js = 0; js < m_nSamples; ++js) {
            digitsBufferLo[js] = 2047;
          }
          ATH_MSG_DEBUG("Masking Channel " << ros << '/' << drawer << '/' << ich << "/0 LG");
        }

      } else { //normal run

        bool hiGain = (igain[ich] == TileID::HIGHGAIN);

        // If tileThresh, apply threshold cut to the in-time Digits signal
        bool Good = true;

        // If channel is good, create TileDigits object and store in container.
        if (Good) {
          echtot_Acc += ech_tot[ich];
          echint_Acc += fabs(ech_int[ich]);
          if (hiGain) {
            ++nChHiAcc;
          } else {
            ++nChLoAcc;
          }

          if (hiGain) {
            //if (m_rndmEvtOverlay // not needed, because DQstatus have been already checked before
            //    && !(theDQstatus->isAdcDQgood(ros, drawer, ich, TileID::HIGHGAIN))) {
            //  chanHiIsBad = true;
            //  ATH_MSG_DEBUG( "BAD DQ Channel " << ros << '/' << drawer << '/' << ich << "/1 HG");
            //}
            if (chanHiIsBad) {
              if (pmt_id.is_valid()) {
                for (int js = 0; js < m_nSamples; ++js) {
                  digitsBuffer[js] = 2047;
                }
              } else if (good_ch) {
                ATH_MSG_DEBUG("Disconnected Channel " << ros << '/' << drawer << '/' << ich);
                for (int js = 0; js < m_nSamples; ++js) {
                  digitsBuffer[js] = 0;
                }
              }
              ATH_MSG_DEBUG("Masking Channel " << ros << '/' << drawer << '/' << ich << "/1 HG");
            }
          } else {

            //if (m_rndmEvtOverlay // not needed, because DQstatus have been already checked before
            //    && !(theDQstatus->isAdcDQgood(ros, drawer, ich, TileID::LOWGAIN))) {
            //  chanLoIsBad = true;
            //  ATH_MSG_DEBUG( "BAD DQ Channel " << ros << '/' << drawer << '/' << ich << "/0 LG");
            //}

            if (chanLoIsBad) {
              if (pmt_id.is_valid()) {
                for (int js = 0; js < m_nSamples; ++js) {
                  digitsBuffer[js] = 2047;
                }
              } else if (good_ch) {
                ATH_MSG_DEBUG("Disconnected Channel " << ros << '/' << drawer << '/' << ich);
                for (int js = 0; js < m_nSamples; ++js) {
                  digitsBuffer[js] = 0;
                }
              }
              ATH_MSG_DEBUG("Masking Channel " << ros << '/' << drawer << '/' << ich << "/0 LG");
            }
          }

          TileDigits* pDigits = new TileDigits(adc_id, digitsBuffer);
          pDigitsContainer->push_back(pDigits);

          //if (ech_int[ich] > m_filterThreshold || ech_int[ich] < -m_filterThresholdMBTS) {
          //  pFilteredContainer->push_back(pDigits);
          //  if (hiGain) {
          //    ++nChHiFlt;
          //  } else {
          //    ++nChLoFlt;
          //  }
          //}

          if (msgLvl(MSG::VERBOSE)) {
            double pedSim = ((hiGain) ? pedSimHi : pedSimLo);
            double ampInTime = digitsBuffer[m_iTrig] - pedSim;
            if (m_integerDigits)
              ampInTime = round(ampInTime);
            msg(MSG::VERBOSE) << ((ech_int[ich] > m_filterThreshold || ech_int[ich] < -m_filterThresholdMBTS) ? "AccFlt" : "Accept") 
                              << " ADC " << m_tileHWID->to_string(adc_id)
                              << " AinT=" << ampInTime 
                              << " ped=" << pedSim 
                              << " Ech=" << ech_tot[ich] 
                              << " EinT=" << ech_int[ich] << endmsg;

            msg(MSG::VERBOSE) << "digits";
            for (unsigned int i = 0; i < digitsBuffer.size(); ++i)
              msg(MSG::VERBOSE) << " " << digitsBuffer[i];
            msg(MSG::VERBOSE) << endmsg;
          }
        } else {
          echtot_Cut += ech_tot[ich];
          echint_Cut += ech_int[ich];
          if (hiGain) {
            ++nChHiCut;
          } else {
            ++nChLoCut;
          }

          if (msgLvl(MSG::VERBOSE)) {
            double pedSim = ((hiGain) ? pedSimHi : pedSimLo);
            double ampInTime = digitsBuffer[m_iTrig] - pedSim;
            if (m_integerDigits)
              ampInTime = round(ampInTime);
            msg(MSG::VERBOSE) << "Reject. ADC " << m_tileHWID->to_string(adc_id) 
                              << " AinT=" << ampInTime 
                              << " ped=" << pedSim 
                              << " Ech=" << ech_tot[ich] 
                              << " EinT=" << ech_int[ich] << endmsg;
          }
        }
      }
    }
  }


  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "... execution completed." << endmsg;
    msg(MSG::DEBUG) << " nCh=" << nChSum 
                    << " nChH/L=" << nChHiSum << "/" << nChLoSum 
                    << " nFltH/L=" << nChHiFlt << "/" << nChLoFlt 
                    << " Hit=" << HitSum 
                    << " Ene=" << EneSum
                    << " RChSum=" << RChSum << endmsg;

    if (m_tileThresh) {
      msg(MSG::DEBUG) << " Accepted:  nChLo/Hi=" << nChLoAcc << "/" << nChHiAcc 
                      << " eTot=" << echtot_Acc 
                      << " eInT=" << echint_Acc << endmsg;
      msg(MSG::DEBUG) << " Rejected: nChLo/Hi=" << nChLoCut << "/" << nChHiCut 
                      << " eTot=" << echtot_Cut 
                      << " eInT=" << echint_Cut << endmsg;
    }
  }

  // step3: register the Digit container in the TES
  CHECK(evtStore()->record(pDigitsContainer, m_digitsContainer, false));

  if (m_filteredContainer.size() > 0) {
    CHECK(evtStore()->record(pFilteredContainer, m_filteredContainer, false));
  }

  return StatusCode::SUCCESS;
}

StatusCode TileDigitsMakerQIE::finalize() {
  ATH_MSG_INFO("... finalized successfully");

  return StatusCode::SUCCESS;
}

