/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
//  Filename : TileDigitsMaker.cxx
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
#include "TileConditions/TilePulseShapes.h"
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileSimAlgs/TileDigitsMaker.h"

//CLHEP includes
#include <CLHEP/Random/Randomize.h>
#include <CLHEP/Units/SystemOfUnits.h>

// For the Athena-based random numbers.
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaKernel/Units.h"

// Pile up
#include "PileUpTools/PileUpMergeSvc.h"

#include "TMatrixF.h"
#include "TDecompChol.h"
#include "cmath"


using CLHEP::RandGaussQ;
using CLHEP::RandFlat;
using Athena::Units::MeV;


//
// Constructor
//
TileDigitsMaker::TileDigitsMaker(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
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
    m_rndmSvc ("AtRndmGenSvc", name),
    m_tileToolEmscale("TileCondToolEmscale"),
    m_tileToolNoiseSample("TileCondToolNoiseSample"),
    m_tileToolPulseShape("TileCondToolPulseShape"),
    m_tileBadChanTool("TileBadChanTool"),
    m_beamInfo("TileBeamInfoProvider/TileBeamInfoProvider")
{
  declareProperty("TileHitContainer",      m_hitContainer = "TileHitCnt");
  declareProperty("TileHitContainer_DigiHSTruth",      m_hitContainer_DigiHSTruth = "TileHitCnt_DigiHSTruth");
  declareProperty("TileDigitsContainer",   m_digitsContainer = "TileDigitsCnt");
  declareProperty("TileDigitsContainer_DigiHSTruth",   m_digitsContainer_DigiHSTruth = "TileDigitsCnt_DigiHSTruth");
  declareProperty("TileFilteredContainer", m_filteredContainer = "TileDigitsFlt");
  declareProperty("FilterThreshold",       m_filterThreshold = 100.0 * MeV, "Threshold on filtered digits (default - 100 MeV)");
  declareProperty("FilterThresholdMBTS",   m_filterThresholdMBTS = 0.0 * MeV, "Threshold on filtered digits of MBTS (default - 0 MeV)");
  declareProperty("TileInfoName",   m_infoName = "TileInfo");
  declareProperty("IntegerDigits",  m_integerDigits = true);
  declareProperty("CalibrationRun", m_calibRun = false);
  declareProperty("RndmSvc", m_rndmSvc, "Random Number Service used in TileDigitsMaker");
  declareProperty("TileCondToolEmscale", m_tileToolEmscale, "Service to calibrate all channels");
  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample);
  declareProperty("RndmEvtOverlay",m_rndmEvtOverlay = false,"Pileup and/or noise added by overlaying random events (default=false)");
  declareProperty("TileCondToolPulseShape",m_tileToolPulseShape);
  declareProperty("UseCoolPulseShapes",m_useCoolPulseShapes = true,"Pulse shapes from database (default=true)");
  declareProperty("TileBadChanTool",m_tileBadChanTool);
  declareProperty("MaskBadChannels",m_maskBadChannels = false,"Remove channels tagged bad (default=false)");
  declareProperty("DoHSTruthReconstruction",m_doDigiTruth = true);
  declareProperty("AllChannels", m_allChannels = -1, "Create all channels, use 0 or 1 or 2 (default=-1 - unset)");
}

TileDigitsMaker::~TileDigitsMaker() {
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

  if(m_doDigiTruth){
    std::vector<double *>::iterator itr3 = m_drawerBufferHi_DigiHSTruth.begin();
    std::vector<double *>::iterator last3 = m_drawerBufferHi_DigiHSTruth.end();
    for (; itr3 != last3; ++itr3) {
      delete[] (*itr3);
    }

    std::vector<double *>::iterator itr4 = m_drawerBufferLo_DigiHSTruth.begin();
    std::vector<double *>::iterator last4 = m_drawerBufferLo_DigiHSTruth.end();
    for (; itr4 != last4; ++itr4) {
      delete[] (*itr4);
    }
  }
}

//
// Alg standard initialize function
//
StatusCode TileDigitsMaker::initialize() {
  // retrieve TileID helper and TileInfo from det store

  ATH_CHECK( detStore()->retrieve(m_tileID) );

  ATH_CHECK( detStore()->retrieve(m_tileTBID) );

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );

  //=== get TileCondToolEmscale
  ATH_CHECK( m_tileToolEmscale.retrieve() );

  //=== get TileCondToolNoiseSample
  ATH_CHECK( m_tileToolNoiseSample.retrieve() );

  m_cabling = TileCablingService::getInstance();

  /* Get needed parameters from tileInfo. */
  m_nSamples = m_tileInfo->NdigitSamples(); // number of time slices for each chan
  m_iTrig = m_tileInfo->ItrigSample();   // index of the triggering time slice
  m_adcMax = (float)m_tileInfo->ADCmax();// adc saturation value used in assignment
  m_adcMaxHG = m_adcMax-0.5;               // value of switch from high to low gain
  m_tileNoise = m_tileInfo->TileNoise(); // (true => generate noise in TileDigits)
  m_tileCoherNoise = m_tileInfo->TileCoherNoise(); // (true => generate coherent noise in TileDigits)
  m_tileThresh = m_tileInfo->TileZeroSuppress(); // (true => apply threshold to Digits)
  m_tileThreshHi = m_tileInfo->ThresholdDigits(TileID::HIGHGAIN);
  m_tileThreshLo = m_tileInfo->ThresholdDigits(TileID::LOWGAIN);

  if (m_tileNoise || m_tileCoherNoise || m_rndmEvtOverlay) {
    ATH_CHECK( m_rndmSvc.retrieve());
    m_pHRengine = m_rndmSvc->GetEngine("Tile_DigitsMaker");
  }

  ATH_MSG_DEBUG( "Event Overlay: " << ((m_rndmEvtOverlay)?"true":"false"));
  ATH_MSG_DEBUG( "Masking Channels: " << ((m_maskBadChannels)?"true":"false"));
  ATH_MSG_DEBUG( "Using pulse shapes from COOL: " << ((m_useCoolPulseShapes)?"true":"false"));

  if (m_useCoolPulseShapes) {
    //=== get TileCondToolPulseShape
    ATH_MSG_DEBUG( "Retrieving pulse shape tool");
    ATH_CHECK( m_tileToolPulseShape.retrieve() );
  } else {
    /* Get fine-grained shaping profile (0.5-ns bins) for both gains*/
    m_digitShapeHi = m_tileInfo->digitsFullShapeHi();
    m_digitShapeHi.push_back(0.0);
    m_digitShapeLo = m_tileInfo->digitsFullShapeLo();
    m_digitShapeLo.push_back(0.0);
  }

  if (m_maskBadChannels || m_rndmEvtOverlay) {
    ATH_CHECK( m_tileBadChanTool.retrieve() );
  }

  m_nShapeHi = m_tileInfo->digitsNBinsHi();
  m_nBinsPerXHi = m_tileInfo->digitsBinsPerXHi();
  m_binTime0Hi = m_tileInfo->digitsTime0BinHi();
  m_timeStepHi = 25.0 / m_nBinsPerXHi;

  m_nShapeLo = m_tileInfo->digitsNBinsLo();
  m_nBinsPerXLo = m_tileInfo->digitsBinsPerXLo();
  m_binTime0Lo = m_tileInfo->digitsTime0BinLo();
  m_timeStepLo = 25.0 / m_nBinsPerXLo;

  if (m_rndmEvtOverlay) {
    m_tileNoise = false;
    m_tileCoherNoise = false;
    m_tileThresh = false;
    m_calibRun = false;
    if (m_allChannels<0) m_allChannels = 2; // create all channels with noise in overlay by default

    ATH_MSG_INFO( "Pileup and/or noise added by overlaying digits of random events");

    // locate the PileUpMergeSvc and initialize our local ptr
    ATH_CHECK( service("PileUpMergeSvc", m_mergeSvc) );

    ATH_MSG_INFO( "PileUpMergeSvc successfully initialized");

    ATH_MSG_DEBUG( "Retrieving TileBeamInfoProvider in TileDigitsMaker");
    ATH_CHECK( m_beamInfo.retrieve() );

  } else {
    if (m_allChannels<0) m_allChannels = 0;                 // do not create all channels by default
    if (m_tileNoise || m_tileCoherNoise) m_allChannels = 2; // unless noise is set to True
    if (msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << "Obtained info from TileInfo" << endmsg;
      msg(MSG::INFO) << "tileNoise=" << ((m_tileNoise) ? "true" : "false")
                     << ", tileCoherNoise=" << ((m_tileCoherNoise) ? "true" : "false")
                     << ", tileThresh=" << ((m_tileThresh) ? "true" : "false");
      if (m_tileThresh)
        msg(MSG::INFO) << ", thresh(hi,lo)=" << m_tileThreshHi << "," << m_tileThreshLo << endmsg;
      else
        msg(MSG::INFO) << endmsg;
    }
  }

  if (m_allChannels>1)
    ATH_MSG_INFO( "Create all channels with noise: true");
  else if (m_allChannels>0)
    ATH_MSG_INFO( "Create all channels without noise: true");
  else
    ATH_MSG_INFO( "Create all channels: false");

  if (m_calibRun)
    m_filteredContainer = "";
  if (m_filteredContainer.size() > 0) {
    ATH_MSG_INFO( "Keep digits with hit energy above " << m_filterThreshold / MeV
                 << " MeV in " << m_filteredContainer << " container");
    ATH_MSG_INFO( "Keep digits from MBTS with original G4 hit energy above "
                 << m_filterThresholdMBTS / MeV << " MeV ");
  } else {
    m_filterThreshold = HUGE_VALL;
    m_filterThresholdMBTS = HUGE_VALL;
  }

  ATH_MSG_DEBUG( "nShapeHi=" << m_nShapeHi
                << " nBinsPerXHi=" << m_nBinsPerXHi
                << " timeStepHi=" << m_timeStepHi
                << " binTime0Hi=" << m_binTime0Hi);

  ATH_MSG_DEBUG( "nShapeLo=" << m_nShapeLo
                << " nBinsPerXLo=" << m_nBinsPerXLo
                << " timeStepLo=" << m_timeStepLo
                << " binTime0Lo=" << m_binTime0Lo);

  // decrease by 1, now they are indexes of last element in a vector
  --m_nShapeHi;
  --m_nShapeLo;

  /* ==================================*/
  // Store HWID's for all 12288 channels (48 channels in each of 64 drawers).
  IdContext drawer_context = m_tileHWID->drawer_context();
  int ndrawers = m_tileHWID->drawer_hash_max();
  const int nchMax = 48; // number of channels per drawer

  ATH_MSG_DEBUG( "ndrawers=" << ndrawers
                 << " nchMax=" << nchMax
                 << " HIGAIN=" << TileID::HIGHGAIN
                 << " LOWGAIN=" << TileID::LOWGAIN);

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
      if (drawerIdx != (int)idhash) {
        ATH_MSG_ERROR("drawer " << m_tileHWID->to_string(drawer_id, -2)
                       << " hash " << idhash << " NOT EQUAL to idx " << drawerIdx);
      } else if (msgLvl(MSG::VERBOSE) && m_cabling->connected(ros, drawer)) {
        msg(MSG::VERBOSE) << "drawer " << m_tileHWID->to_string(drawer_id, -2)
                          << " hash " << idhash << endmsg;
      }

      for (int ch = 0; ch < nchMax; ++ch) {
        adc_ids[ch] = m_tileHWID->adc_id(drawer_id, ch, TileID::HIGHGAIN);
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
  if(m_doDigiTruth){
    m_drawerBufferHi_DigiHSTruth.reserve(nchMax);
    m_drawerBufferLo_DigiHSTruth.reserve(nchMax);
    for (int ich = 0; ich < nchMax; ++ich) {
      double * pDigitSamplesHi = new double[nSamp];
      double * pDigitSamplesLo = new double[nSamp];
      m_drawerBufferHi_DigiHSTruth.push_back(pDigitSamplesHi);
      m_drawerBufferLo_DigiHSTruth.push_back(pDigitSamplesLo);
    }
  }

  /* ==================================*/
  ATH_MSG_INFO( "TileDigitsMaker initialization completed");

  return StatusCode::SUCCESS;
}


StatusCode TileDigitsMaker::execute() {
  ATH_MSG_DEBUG( "Executing TileDigitsMaker");

  static bool first = (msgLvl(MSG::VERBOSE) && !m_rndmEvtOverlay );
  if (first) {
    ATH_MSG_VERBOSE( "Dumping 2G noise parameters");
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
          ATH_MSG_VERBOSE( "Channel " << m_tileHWID->to_string(drawer_id,-2) << "/" << ch
                           << " pedHi="<< pedSimHi
                           << " pedLo="<< pedSimLo
                           << " rmsHi="<< sigmaHi_Hfn1 << "," << sigmaHi_Hfn2 << "," << sigmaHi_Norm
                           << " rmsLo="<< sigmaLo_Hfn1 << "," << sigmaLo_Hfn2 << "," << sigmaLo_Norm);

        }
      }
    }
  }

  // declare array for random number generation
  double Rndm[16];       // Can't use variable size array,
  double RndmLo[16];     // Can't use variable size array,
  double Rndm_dG[1];    // uniform random number for the double gaussian
  double RndmLo_dG[1];  // uniform random number for the double gaussian

  // step1: Get hit container from TES 
  const TileHitContainer* hitCont;
  ATH_CHECK( evtStore()->retrieve(hitCont, m_hitContainer) );
  const TileHitContainer* hitCont_DigiHSTruth;
  if(m_doDigiTruth){
      ATH_CHECK( evtStore()->retrieve(hitCont_DigiHSTruth, m_hitContainer_DigiHSTruth) );
  }

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
  TileDigitsContainer* pDigitsContainer_DigiHSTruth = nullptr;
  if(m_doDigiTruth){
    pDigitsContainer_DigiHSTruth = new TileDigitsContainer(true);
  }

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
  double ech_int_DigiHSTruth[nchMax];
  int over_gain[nchMax];
  memset(over_gain,-1,sizeof(over_gain));

  /* Make a vector of digits (to be filled at the end from m_drawerBuffer arrays) */
  std::vector<float> digitsBuffer(m_nSamples);
  std::vector<float> digitsBufferLo(m_nSamples); // for calib runs
  std::vector<float> digitsBuffer_DigiHSTruth(m_nSamples);
  std::vector<float> digitsBufferLo_DigiHSTruth(m_nSamples); // for calib runs

  /* everything for calculation of coherent noise */
  // booleans for coherent noise
  Bool_t coherNoiseHi = false;
  Bool_t coherNoiseLo = false;
  TMatrixD CorrWeightHi;
  TMatrixD CorrWeightLo;
  std::vector<double *> CorrRndmVec;
  std::vector<double *> CorrRndmVecLo;
  if (m_tileCoherNoise) {
    for (int k = 0; k < m_nSamples; ++k) {
      double * RndmVec = new double[nchMax];
      CorrRndmVec.push_back(RndmVec);
    }
    if (m_calibRun) {
      for (int k = 0; k < m_nSamples; ++k) {
        double * RndmVecLo = new double[nchMax];
        CorrRndmVecLo.push_back(RndmVecLo);
      }
    }
  }

  TileDigitsContainer::const_iterator collItrRndm;
  TileDigitsContainer::const_iterator lastCollRndm;
  if (m_rndmEvtOverlay) {

    typedef PileUpMergeSvc::TimedList<TileDigitsContainer>::type TimedDigitContList;
    TileDigitsContainer::const_iterator rndm_digititer_begin;
    TileDigitsContainer::const_iterator rndm_digititer_end;
    TileDigitsContainer::const_iterator rndm_digititer;

    TimedDigitContList digitContList;
    ATH_CHECK( m_mergeSvc->retrieveSubEvtsData("TileDigitsCnt", digitContList));
    ATH_MSG_DEBUG( "TileDigitsCnt successfully retrieved ");


    if (digitContList.size() == 0) {
      ATH_MSG_WARNING( "No overlay done ... ");
      return StatusCode::SUCCESS;
    }

    TimedDigitContList::iterator iTzeroDigitCont(digitContList.begin());
    const TileDigitsContainer * rndm_digit_container = (iTzeroDigitCont->second);
    collItrRndm = rndm_digit_container->begin();
    lastCollRndm = rndm_digit_container->end();

    typedef PileUpMergeSvc::TimedList<TileRawChannelContainer>::type TimedRawChanContList;
    TimedRawChanContList rawchanContList;
    if (!(m_mergeSvc->retrieveSubEvtsData("TileRawChannelCnt", rawchanContList).isSuccess())) {
      ATH_MSG_ERROR( "Cannot retrieve TileRawChannelContainer for DQ check");
    } else {
      ATH_MSG_DEBUG( "TileRawChannelContainer for DQ check retrieved");
    }

    const TileRawChannelContainer* rndm_rawchan_container(nullptr);
    if (!rawchanContList.empty()) {
      TimedRawChanContList::iterator iTzeroRawChanCont(rawchanContList.begin());
      rndm_rawchan_container = iTzeroRawChanCont->second;
    }

    ATH_MSG_DEBUG( "setContainer method being called in TileDigitsMaker");
    m_beamInfo->setContainers(rndm_digit_container, rndm_rawchan_container);
    ATH_MSG_DEBUG( "Containers successfully set in TileBeamInfoProvider");
    m_DQstatus = m_beamInfo->getDQstatus();
  }

  // iterate over all collections in a container
  TileHitContainer::const_iterator collItr = hitCont->begin();
  TileHitContainer::const_iterator lastColl = hitCont->end();
  // Hit Container and signal hit container are the same size (1 entry per channel)
  TileHitContainer::const_iterator collItr_DigiHSTruth;
  if(m_doDigiTruth) collItr_DigiHSTruth = hitCont_DigiHSTruth->begin();

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
      ATH_MSG_VERBOSE( "ROS "<< ros << " drawer " << drawer << " is connected");
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
    int igainch = (m_allChannels) ? TileID::HIGHGAIN : -1;
    if (m_rndmEvtOverlay) {
      memset(over_gain, -1, sizeof(over_gain));
    } else if (m_tileNoise || m_tileCoherNoise) {
      igainch = TileID::HIGHGAIN;
    }
    for (int ich = 0; ich < nchMax; ++ich) {
      igain[ich] = igainch;
      ech_tot[ich] = 0.;
      ech_int[ich] = 0.;
      ntot_ch[ich] = 0;
      double * pDigitSamplesHi = m_drawerBufferHi[ich];
      double * pDigitSamplesLo = m_drawerBufferLo[ich];
      double * pDigitSamplesHi_DigiHSTruth = nullptr;
      double * pDigitSamplesLo_DigiHSTruth = nullptr;
      if(m_doDigiTruth){
        pDigitSamplesHi_DigiHSTruth = m_drawerBufferHi_DigiHSTruth[ich];
        pDigitSamplesLo_DigiHSTruth = m_drawerBufferLo_DigiHSTruth[ich];
      }
      for (int js = 0; js < m_nSamples; ++js) {
        pDigitSamplesHi[js] = 0.;
        pDigitSamplesLo[js] = 0.;
        if(m_doDigiTruth){
          pDigitSamplesHi_DigiHSTruth[js] = 0.;
          pDigitSamplesLo_DigiHSTruth[js] = 0.;
        }
      }
    }

    if (m_rndmEvtOverlay && collItrRndm != lastCollRndm) {

      if ((*collItr)->identify() != (*collItrRndm)->identify()) {
        ATH_MSG_ERROR ( "Frag IDs for hit collection and digits overlay collection do not match "
                        << MSG::hex << (*collItr)->identify() << " != " << (*collItrRndm)->identify()
                        << MSG::dec );
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
        float dig(2047.),digmin(65536.),digmax(-65536.);
        if (good_dq) {
          for (int js = 0; js < nSamp2; ++js) {
            dig=digits[js];
            if (dig<digmin) digmin=dig;
            if (dig>digmax) digmax=dig;
            if (dig<0.01) { // skip zeros
              --goodDigits;
            } else if (dig>1022.99) { // skip overflows
              if (dig>1023.01) { // ignore everything in case of invalid digits
                dig=2047.0;
                goodDigits = 0;
                break;
              }
              --goodDigits;
            }
          } 
        } else if (goodDigits>0) {
          goodDigits = 0;
          dig = digmin = digmax = digits[0];
          for (int js = 1; js < nSamp2; ++js) {
            dig=digits[js];
            if (dig<digmin) digmin=dig;
            else if (dig>digmax) digmax=dig;
          }
        }
        
        if (goodDigits>0) {
          over_gain[channel] = gain;
          if (nSamp2 != m_nSamples) {
            digits.resize(m_nSamples);
            for (int js = nSamp2; js < m_nSamples; ++js)
              digits[js] = digits[js - 1]; // repeat last value in vector (nSamp-nSamp2) times
          }

          double * buffer;
          double * bufferLG=0;
          if (gain == TileID::HIGHGAIN) {
            if (digmax - digmin > 5. && good_ch ) {// 5 ADC counts cut - to ignore pure noise in HG (less than 0.1 count effect in LG)
              float ratio = m_tileToolEmscale->doCalibCisOnl(drawerIdx, channel, TileID::HIGHGAIN, 1.)
                          / m_tileToolEmscale->doCalibCisOnl(drawerIdx, channel, TileID::LOWGAIN, 1.); // ratio between low and high gain
              dig=std::min(digits[0],std::max(digmin,m_tileToolNoiseSample->getPed(drawerIdx, channel, TileID::HIGHGAIN)));
              bufferLG = m_drawerBufferLo[channel]; 
              for (int js = 0; js < m_nSamples; ++js)        // in case low gain background is needed later
                bufferLG[js] = (digits[js]-dig)*ratio;    // put in low gain buffer signal divided by 64 (LG/HG ratio) 
            }
            buffer = m_drawerBufferHi[channel];
          } else {
            buffer = m_drawerBufferLo[channel];
          }
          for (int js = 0; js < m_nSamples; ++js)
            buffer[js] = digits[js];

          if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "RNDM BG ADC " << m_tileHWID->to_string(adcId)
                              << " samples=";
            for (int js = 0; js < m_nSamples; ++js)
              msg(MSG::VERBOSE) << " " << buffer[js];
            if (!good_ch)
              msg(MSG::VERBOSE) << " BCH";
            if (!good_dq) {
              msg(MSG::VERBOSE) << " BDQ";
            } else if (bufferLG) {
              msg(MSG::VERBOSE) << "  LG=";
              for (int js = 0; js < m_nSamples; ++js)
                msg(MSG::VERBOSE) << " " << int(bufferLG[js]*100)/100.;
            }
            msg(MSG::VERBOSE) << endmsg;
          }

        } else if (nSamp2 > 0) {
          over_gain[channel] = 10+gain; // flag problematic channel

          double * buffer;
          if (gain == TileID::HIGHGAIN) {
            buffer = m_drawerBufferHi[channel];
          } else {
            buffer = m_drawerBufferLo[channel];
          }

          if (digmin != digmax || (dig!=0. && dig!=1023.)) dig=2047.; // keep only 0 or 1023 as it is
          for (int js = 0; js < m_nSamples; ++js)
            buffer[js] = dig;

          if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "BAD BG  ADC " << m_tileHWID->to_string(adcId)
                              << " samples=";
            for (int js = 0; js < nSamp2; ++js)
              msg(MSG::VERBOSE) << " " << digits[js];
            msg(MSG::VERBOSE) << ((good_ch)?"":" BCH") << ((good_dq)?"":" BDQ") << endmsg;
          }

        } else {
          ATH_MSG_VERBOSE( "NO BG   ADC " << m_tileHWID->to_string(adcId)
                           << " samples= 0 0 0 0 0 0 0"
                           << ((good_ch)?"":" BCH") << ((good_dq)?"":" BDQ") );
        }
      }
      ++collItrRndm; // skip to next digi collection
    }

    std::vector<bool> signal_in_channel(nchMax, 0);
    std::vector<bool> signal_in_channel_DigiHSTruth(nchMax, 0);
    ATH_CHECK(FillDigitCollection( collItr, m_drawerBufferLo, m_drawerBufferHi, igain, over_gain, ech_int, signal_in_channel));
    if(m_doDigiTruth){
      ATH_CHECK(FillDigitCollection( collItr_DigiHSTruth, m_drawerBufferLo_DigiHSTruth, m_drawerBufferHi_DigiHSTruth, igain, over_gain, ech_int_DigiHSTruth, signal_in_channel_DigiHSTruth));
    } // End DigiHSTruth stuff

    /* Now all signals for this collection are stored in m_drawerBuffer, 
     accessed with pDigitSamplesHi and pDigitSampleLo. */
    if (msgLvl(MSG::VERBOSE)) {
      for (int ich = 0; ich < nchMax; ++ich) {
        if (igain[ich] > -1) {
          double * pDigitSamplesHi = m_drawerBufferHi[ich];
          double * pDigitSamplesLo = m_drawerBufferLo[ich];
          msg(MSG::VERBOSE) << "total:  ADC " << m_tileHWID->to_string(adc_ids[ich],-1) << "/" << igain[ich] 
                            << " nhit=" << ntot_ch[ich]
                            << " e_ch=" << ech_tot[ich]
                            << " AinTHi=" << pDigitSamplesHi[m_iTrig]
                            << " AinTLo=" << pDigitSamplesLo[m_iTrig] << endmsg;
        }
      }
    }

    /* ---------------------------------------------------------------      */
    /* Now all signals for this drawer are stored in m_drawerBuffer arrays, */
    /* and we are finished with TileHits for this collection.  Loop over    */
    /* channels to add noise and pedestal.  Check for saturation for        */
    /* each channel, and in case of saturation convert to low gain.         */
    /* -------------------------------------------------------------- */

    // =============CORRELATION MODIFICATION (F Spano)============== 
    //
    // Define CoVariance Matrix corresponding to noise.
    // TO UPDATE:: Such Matrix will have to be loaded from database in the future; 1 matrix per drawer.
    // NOW: 
    // a) define one covariance matrix
    // b) find Cholesky decomposition use for corrlation building
    // if this is set load the matrix
    if (m_tileCoherNoise) {
      ATH_MSG_VERBOSE( "Coherent noise for ROS " << ros
                       << " drawer " << drawer
                       << " with " << nchMax << " channels and "
                       << m_nSamples << "samples ");

      // get decomposed covariance matrix for hi gain
      coherNoiseHi = 1;
      if (coherNoiseHi) {
        CorrWeightHi.ResizeTo(*(m_tileInfo->DecoCovariance(ros, drawer, TileID::HIGHGAIN)));
        CorrWeightHi = *(m_tileInfo->DecoCovariance(ros, drawer, TileID::HIGHGAIN));
      }

      // get decomposed covariance matrix for low gain
      coherNoiseLo = 1;
      if (coherNoiseLo) {
        CorrWeightLo.ResizeTo(*(m_tileInfo->DecoCovariance(ros, drawer, TileID::LOWGAIN)));
        CorrWeightLo = *(m_tileInfo->DecoCovariance(ros, drawer, TileID::LOWGAIN));
      }

      //NOTE: ShootArray's inputs are : the engine, the size, the vector, the mean, the standard dev
      for (int k = 0; k < m_nSamples; ++k) {
        double * RndmVec = CorrRndmVec[k];
        RandGaussQ::shootArray(m_pHRengine, nchMax, RndmVec, 0.0, 1.0);
      }

      if (m_calibRun) {
        for (int k = 0; k < m_nSamples; ++k) {
          double * RndmVecLo = CorrRndmVecLo[k];
          RandGaussQ::shootArray(m_pHRengine, nchMax, RndmVecLo, 0.0, 1.0);
        }
      }
    }
    // =============CORRELATION MODIFICATION (F Spano)============== end

    // looping over channels
    for (int ich = 0; ich < nchMax; ++ich) {
      /* If igain<0, channel is inactive => skip it.                    */
      if (igain[ich] < 0)
        continue;

      bool chanLoIsBad = false;
      bool chanHiIsBad = false;
      if (m_maskBadChannels) {
        TileBchStatus statusLo = m_tileBadChanTool->getAdcStatus(drawerIdx, ich, TileID::LOWGAIN);
        TileBchStatus statusHi = m_tileBadChanTool->getAdcStatus(drawerIdx, ich, TileID::HIGHGAIN);
        chanLoIsBad = statusLo.isBad();
        chanHiIsBad = statusHi.isBad();
      }

      /* Generate the nSamp Digits for high gain. Check each for saturation. */
      ++nChHiSum;
      HWIdentifier adc_id = adc_ids[ich];
      HWIdentifier adc_id_lo; // for calib runs
      Identifier pmt_id = m_cabling->h2s_pmt_id(adc_id);
      ATH_MSG_DEBUG( "Ch " << m_tileHWID->to_string(adc_id,-1)
                     << " PMT " << (pmt_id.is_valid() ? m_tileID->to_string(pmt_id,-1) : (signal_in_channel[ich] ? "fake gap" : "not connected"))
                     << " gain=" << igain[ich]);
      if (m_calibRun) {
        ++nChLoSum;
        adc_id_lo = m_tileHWID->adc_id(drawer_id, ich, TileID::LOWGAIN);
      }
      /* Get pedestal and noise values */
      double pedSimHi(0.), sigmaHi_Hfn1(0.), sigmaHi_Hfn2(0.), sigmaHi_Norm(0.), pedSimLo(0.),
          sigmaLo_Hfn1(0.), sigmaLo_Hfn2(0.), sigmaLo_Norm(0.);
      bool good_ch = (over_gain[ich]<9);
      bool overNoiseHG(over_gain[ich]!=TileID::HIGHGAIN && good_ch); // it's always true if no overlay
      bool overNoiseLG(over_gain[ich]!=TileID::LOWGAIN  && good_ch); // it's always true if no overlay
      bool tileNoiseHG(false),tileNoiseLG(false);

      if (overNoiseHG) {
        overNoiseHG &= (m_rndmEvtOverlay && m_allChannels>1); // set it to true only for overlay
        tileNoiseHG = m_tileNoise || overNoiseHG;

        pedSimHi = m_tileToolNoiseSample->getPed(idhash, ich, TileID::HIGHGAIN);
        // bug fix for wrong ped value in DB
        if (pedSimHi == 0.0 && (signal_in_channel[ich] || pmt_id.is_valid()))
          pedSimHi = 50.;

        sigmaHi_Hfn1 = m_tileToolNoiseSample->getHfn1(idhash, ich, TileID::HIGHGAIN);
        sigmaHi_Hfn2 = m_tileToolNoiseSample->getHfn2(idhash, ich, TileID::HIGHGAIN);
        if (sigmaHi_Hfn1>0 || sigmaHi_Hfn2) {
          sigmaHi_Norm = sigmaHi_Hfn1 / (sigmaHi_Hfn1
                       + sigmaHi_Hfn2 * m_tileToolNoiseSample->getHfnNorm(idhash, ich, TileID::HIGHGAIN));
        } else {
          sigmaHi_Hfn1 = m_tileToolNoiseSample->getHfn(idhash, ich, TileID::HIGHGAIN);
          sigmaHi_Norm = 1.;
        }
      }
      
      if (overNoiseLG) {
        overNoiseLG &= (m_rndmEvtOverlay && m_allChannels>1); // set it to true only for overlay
        tileNoiseLG = m_tileNoise || overNoiseLG;

        pedSimLo = m_tileToolNoiseSample->getPed(idhash, ich, TileID::LOWGAIN);
        // bug fix for wrong ped value in DB
        if (pedSimLo == 0.0 && (signal_in_channel[ich] || pmt_id.is_valid()))
          pedSimLo = 30.;

        sigmaLo_Hfn1 = m_tileToolNoiseSample->getHfn1(idhash, ich, TileID::LOWGAIN);
        sigmaLo_Hfn2 = m_tileToolNoiseSample->getHfn2(idhash, ich, TileID::LOWGAIN);
        if (sigmaLo_Hfn1 > 0 || sigmaLo_Hfn2) {
          sigmaLo_Norm = sigmaLo_Hfn1 / (sigmaLo_Hfn1
                       + sigmaLo_Hfn2 * m_tileToolNoiseSample->getHfnNorm(idhash, ich, TileID::LOWGAIN));
        } else {
          sigmaLo_Hfn1 = m_tileToolNoiseSample->getHfn(idhash, ich, TileID::LOWGAIN);
          sigmaLo_Norm = 1.;
        }
      }

      /* If tileNoise is requested, generate array of random numbers.   */
      if (tileNoiseLG) { // true if tileNoise is set or noise is needed for low gain in overlay
        RandGaussQ::shootArray(m_pHRengine, m_nSamples, Rndm, 0.0, 1.0);
        RandFlat::shootArray(m_pHRengine, 1, Rndm_dG, 0.0, 1.0);
        if (m_calibRun) {
          RandGaussQ::shootArray(m_pHRengine, m_nSamples, RndmLo, 0.0, 1.0);
          RandFlat::shootArray(m_pHRengine, 1, RndmLo_dG, 0.0, 1.0);
        }
      }

      double * pDigitSamplesHi = m_drawerBufferHi[ich];
      double * pDigitSamplesLo = m_drawerBufferLo[ich];
      double * pDigitSamplesHi_DigiHSTruth = nullptr;
      double * pDigitSamplesLo_DigiHSTruth = nullptr;
      if(m_doDigiTruth) pDigitSamplesHi_DigiHSTruth = m_drawerBufferHi_DigiHSTruth[ich];
      if(m_doDigiTruth) pDigitSamplesLo_DigiHSTruth = m_drawerBufferLo_DigiHSTruth[ich];

      ATH_MSG_DEBUG(" Channel " << ros << '/' << drawer << '/' << ich 
                     << " sampHi=" << pDigitSamplesHi[m_iTrig]
                     << " pedHi=" << pedSimHi
                     << " sampLo=" << pDigitSamplesLo[m_iTrig]
                     << " pedLo=" << pedSimLo);

      // looping over samples
      for (int js = 0; js < m_nSamples; ++js) {

        digitsBuffer[js] = pDigitSamplesHi[js] + pedSimHi;
        if(m_doDigiTruth && pDigitSamplesHi_DigiHSTruth != nullptr) digitsBuffer_DigiHSTruth[js] = pDigitSamplesHi_DigiHSTruth[js] + pedSimHi;

        double noiseHi(0.0);
        // Full noise pattern, including coherent noise has priority over normal noise //F Spano'
        if (coherNoiseHi) {
          // get the js-th  correct random vector of 48 elements for the jsth sample k  //F Spano'
          double *CorVec = CorrRndmVec[js];
          // apply Y=C*Z where Z is the random vector of 48 normal indep variables, and C is the Cholesky decomposition //F Spano'
          for (int i = 0; i < nchMax; ++i) noiseHi += CorrWeightHi(i, ich) * CorVec[i];
        } else if (tileNoiseHG) {
          //using the same gaussian(sigma) for all samples in one channel in one event
          if (Rndm_dG[0] < sigmaHi_Norm) noiseHi = sigmaHi_Hfn1 * Rndm[js];
          else noiseHi = sigmaHi_Hfn2 * Rndm[js];
        }

        if (digitsBuffer[js] + noiseHi >= 0.0) {
          digitsBuffer[js] += noiseHi;
          if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] += noiseHi;
        }

        else { 
          digitsBuffer[js] -= noiseHi;
          if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] -= noiseHi;
        }


        if (m_integerDigits) {
          digitsBuffer[js] = round(digitsBuffer[js]);
          if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] = round(digitsBuffer_DigiHSTruth[js]);
        }

        if (m_calibRun) { //Calculate also low gain
          digitsBufferLo[js] = pDigitSamplesLo[js] + pedSimLo;
          if(m_doDigiTruth && pDigitSamplesLo_DigiHSTruth != nullptr) digitsBufferLo_DigiHSTruth[js] = pDigitSamplesLo_DigiHSTruth[js] + pedSimLo;
          double noiseLo(0.0);
          // Full noise pattern, including coherent noise has priority over normal noise //F Spano'
          if (coherNoiseLo) {
            // get the js-th  correct random vector of 48 elements for the jsth sample // F Spano'
            double *CorVecLo = CorrRndmVecLo[js];
            // apply Y=C*Z where Z is the random vector of 48 normal indep variables, and C is the Cholesky decomposition // F Spano'
            for (int i = 0; i < nchMax; ++i) noiseLo += CorrWeightLo(i, ich) * CorVecLo[i];
          } else if (tileNoiseLG) {
            //using the same gaussian (sigma) for all samples in one channel in one event
            if (RndmLo_dG[0] < sigmaLo_Norm) noiseLo = sigmaLo_Hfn1 * RndmLo[js];
            else noiseLo = sigmaLo_Hfn2 * RndmLo[js];
          }
          
          if (digitsBufferLo[js] + noiseLo >= 0.0) {
            digitsBufferLo[js] += noiseLo;
            if(m_doDigiTruth) digitsBufferLo_DigiHSTruth[js] += noiseLo;
          }
          else {
            digitsBufferLo[js] -= noiseLo;
            if(m_doDigiTruth) digitsBufferLo_DigiHSTruth[js] -= noiseLo;
          }
          
          if (m_integerDigits) {
            digitsBufferLo[js] = round(digitsBufferLo[js]);
            if(m_doDigiTruth) digitsBufferLo_DigiHSTruth[js] = round(digitsBufferLo_DigiHSTruth[js]);
          }


        } else if ((digitsBuffer[js] >= m_adcMaxHG && good_ch) || igain[ich] == TileID::LOWGAIN) { // saturation of high gain in non-calib run
                                                                                                 // or low gain in digi overlay
          --nChHiSum;
          ++nChLoSum;
          igain[ich] = TileID::LOWGAIN;
          adc_id = m_tileHWID->adc_id(drawer_id, ich, TileID::LOWGAIN);

          // reset all samples in digitsBuffer[] to Low Gain values
          for (js = 0; js < m_nSamples; ++js) {
            digitsBuffer[js] = pDigitSamplesLo[js] + pedSimLo;
            if(m_doDigiTruth && pDigitSamplesLo_DigiHSTruth != nullptr) digitsBuffer_DigiHSTruth[js] = pDigitSamplesLo_DigiHSTruth[js] + pedSimLo;
            double noiseLo(0.0);
            // Full noise pattern, including coherent noise has priority over normal noise //F Spano'
            if (coherNoiseLo) {
              // get the js-th  correct random vector of 48 elements for the jsth sample // F Spano'
              double *CorVec = CorrRndmVec[js]; // reuse the same rndm as for high gain
              // apply Y=C*Z where Z is the random vector of 48 normal indep variables, and C is the Cholesky decomposition // F Spano'
              for (int i = 0; i < nchMax; ++i) noiseLo += CorrWeightLo(i, ich) * CorVec[i];
            } else if (tileNoiseLG) {
              //using the same gaussian (sigma) for all samples in one channel in one event
	            // reuse the same rndm as for high gain
              if (Rndm_dG[0] < sigmaLo_Norm) noiseLo = sigmaLo_Hfn1 * Rndm[js];
              else noiseLo = sigmaLo_Hfn2 * Rndm[js];
            }

            if (digitsBuffer[js] + noiseLo >= 0.0) {
              digitsBuffer[js] += noiseLo;
              if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] += noiseLo;
            }
            else {
              digitsBuffer[js] -= noiseLo;
              if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] -= noiseLo;
            }

            if (digitsBuffer[js] > m_adcMax && good_ch) {
              digitsBuffer[js] = m_adcMax;
              if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] = m_adcMax;
            }
            if (m_integerDigits) {
              digitsBuffer[js] = round(digitsBuffer[js]);
              if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] = round(digitsBuffer_DigiHSTruth[js]);
            }
          }

          overNoiseHG = false;

          if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "Channel " << ros << '/' << drawer << '/' << ich << "/" << igain[ich]
                              << "  Switch to low gain  Amp(lo)=" << digitsBuffer[m_iTrig] << endmsg;
            if (overNoiseLG) {
              if (sigmaLo_Norm<1.0) {
                msg(MSG::VERBOSE) << "LG Ped & noise from DB "
                                  << pedSimLo << " " << sigmaLo_Hfn1 << " " << sigmaLo_Hfn2 << " " << sigmaLo_Norm
                                  << ((Rndm_dG[0] < sigmaLo_Norm)?(" sig1 used"):(" sig2 used")) << endmsg;
              } else {
                msg(MSG::VERBOSE) << "LG Ped & noise from DB "
                                  << pedSimLo << " " << sigmaLo_Hfn1 << endmsg;
              }
            }
          }
          break;
        }
      }
      if (msgLvl(MSG::VERBOSE)) {
        if (overNoiseHG) {
          if (sigmaHi_Norm<1.0) {
            msg(MSG::VERBOSE) << "HG Ped & noise from DB " 
                              << pedSimHi << " " << sigmaHi_Hfn1 << " " << sigmaHi_Hfn2 << " " << sigmaHi_Norm
                              << ((Rndm_dG[0] < sigmaHi_Norm)?(" sig1 used"):(" sig2 used")) << endmsg;
          } else {
            msg(MSG::VERBOSE) << "HG Ped & noise from DB " 
                              << pedSimHi << " " << sigmaHi_Hfn1 << endmsg;
          }
        }
        if (m_calibRun && overNoiseLG) {
          if (sigmaLo_Norm<1.0) {
            msg(MSG::VERBOSE) << "LG Ped & noise from DB "
                              << pedSimLo << " " << sigmaLo_Hfn1 << " " << sigmaLo_Hfn2 << " " << sigmaLo_Norm
                              << ((RndmLo_dG[0] < sigmaLo_Norm)?(" sig1 used"):(" sig2 used")) << endmsg;
          } else {
            msg(MSG::VERBOSE) << "LG Ped & noise from DB "
                              << pedSimLo << " " << sigmaLo_Hfn1 << endmsg;
          }
        }
      }

      if (m_calibRun) { // calib run - keep both low and high gain

        if (chanHiIsBad) {
          for (int js = 0; js < m_nSamples; ++js) {
            digitsBuffer[js] = 2047;
            if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] = 2047;

          }
          ATH_MSG_DEBUG( "Masking Channel " << ros << '/' << drawer << '/' << ich << "/1 HG" );
        }
        TileDigits* pDigits = new TileDigits(adc_id, digitsBuffer);
        pDigitsContainer->push_back(pDigits);
        if(m_doDigiTruth && pDigitsContainer_DigiHSTruth != nullptr){
          TileDigits* pDigits_DigiHSTruth = new TileDigits(adc_id, digitsBuffer_DigiHSTruth);
          pDigitsContainer_DigiHSTruth->push_back(pDigits_DigiHSTruth);
        }

        if (chanLoIsBad) {
          for (int js = 0; js < m_nSamples; ++js) {
            digitsBufferLo[js] = 2047;
            if(m_doDigiTruth) digitsBufferLo_DigiHSTruth[js] = 2047;
          }
          ATH_MSG_DEBUG( "Masking Channel " << ros << '/' << drawer << '/' << ich << "/0 LG");
        }
        TileDigits* pDigitsLo = new TileDigits(adc_id_lo, digitsBufferLo);
        pDigitsContainer->push_back(pDigitsLo);
        if(m_doDigiTruth && pDigitsContainer_DigiHSTruth != nullptr){
          TileDigits* pDigitsLo_DigiHSTruth = new TileDigits(adc_id_lo, digitsBufferLo_DigiHSTruth);
          pDigitsContainer_DigiHSTruth->push_back(pDigitsLo_DigiHSTruth);
        }
      } else { //normal run

        bool hiGain = (igain[ich] == TileID::HIGHGAIN);

        // If tileThresh, apply threshold cut to the in-time Digits signal
        bool Good = true;
        if (m_tileThresh) {
          if (hiGain) { // make threshold only on high gain
            double ampInTime = digitsBuffer[m_iTrig] - pedSimHi;
            if (m_integerDigits)
              ampInTime = round(ampInTime);
            if (m_tileThreshHi < 0) {
              if (fabs(ampInTime) < fabs(m_tileThreshHi))
                Good = false;
            } else {
              if (ampInTime < m_tileThreshHi)
                Good = false;
            }
          }
        }
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
                  if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] = 2047;
                }
              } else if (good_ch) {
                ATH_MSG_DEBUG( "Disconnected Channel " << ros << '/' << drawer << '/' << ich);
                for (int js = 0; js < m_nSamples; ++js) {
                  digitsBuffer[js] = 0;
                  if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] = 0;
                }
              }
              ATH_MSG_DEBUG( "Masking Channel " << ros << '/' << drawer << '/' << ich << "/1 HG");
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
                  if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] = 2047;
                }
              } else if (good_ch) {
                ATH_MSG_DEBUG( "Disconnected Channel " << ros << '/' << drawer << '/' << ich);
                for (int js = 0; js < m_nSamples; ++js) {
                  digitsBuffer[js] = 0;
                  if(m_doDigiTruth) digitsBuffer_DigiHSTruth[js] = 0;
                }
              }
              ATH_MSG_DEBUG( "Masking Channel " << ros << '/' << drawer << '/' << ich << "/0 LG");
            }
          }

          TileDigits* pDigits = new TileDigits(adc_id, digitsBuffer);
          pDigitsContainer->push_back(pDigits);
          if(m_doDigiTruth && pDigitsContainer_DigiHSTruth != nullptr){
            TileDigits* pDigits_DigiHSTruth = new TileDigits(adc_id, digitsBuffer_DigiHSTruth);
            pDigitsContainer_DigiHSTruth->push_back(pDigits_DigiHSTruth);
          }

          if (ech_int[ich] > m_filterThreshold || ech_int[ich] < -m_filterThresholdMBTS) {
            pFilteredContainer->push_back(pDigits);
            if (hiGain) {
              ++nChHiFlt;
            } else {
              ++nChLoFlt;
            }
          }

          if (msgLvl(MSG::VERBOSE)) {
            double pedSim = ((hiGain) ? pedSimHi : pedSimLo);
            double ampInTime = digitsBuffer[m_iTrig] - pedSim;
            if (m_integerDigits)
              ampInTime = round(ampInTime);
            msg(MSG::VERBOSE) << ((ech_int[ich] > m_filterThreshold
                                  || ech_int[ich] < -m_filterThresholdMBTS) ? "AccFlt" : "Accept")
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
    if(m_doDigiTruth) ++collItr_DigiHSTruth;
  }

  if (m_tileCoherNoise) {
    for (int k = 0; k < m_nSamples; ++k) {
      double * RndmVec = CorrRndmVec[k];
      delete[] RndmVec;
    }
    if (m_calibRun) {
      for (int k = 0; k < m_nSamples; ++k) {
        double * RndmVecLo = CorrRndmVecLo[k];
        delete[] RndmVecLo;
      }
    }
  }

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "TileDigitsMaker execution completed." << endmsg;
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
  ATH_CHECK( evtStore()->record(pDigitsContainer, m_digitsContainer, false) );
  if(m_doDigiTruth && pDigitsContainer_DigiHSTruth != nullptr){
    ATH_CHECK( evtStore()->record(pDigitsContainer_DigiHSTruth, m_digitsContainer_DigiHSTruth, false) );
  }

  if (m_filteredContainer.size() > 0) {
    ATH_CHECK( evtStore()->record(pFilteredContainer, m_filteredContainer, false) );
  }

  return StatusCode::SUCCESS;
}

StatusCode TileDigitsMaker::finalize() {
  ATH_MSG_INFO( "TileDigitsMaker finalized successfully");

  return StatusCode::SUCCESS;
}

StatusCode TileDigitsMaker::FillDigitCollection(TileHitContainer::const_iterator hitContItr, std::vector<double *> &drawerBufferLo, std::vector<double *> &drawerBufferHi, int igain[], int over_gain[], double ech_int[], std::vector<bool> &signal_in_channel) const{
  
  // Zero sums for monitoring.
  int nChSum = 0;
  double HitSum = 0.;
  double EneSum = 0.;
  double RChSum = 0.;

  const int nchMax = 48; // number of channels per drawer
  int ntot_ch[nchMax];
  double ech_tot[nchMax];
  //double ech_int[nchMax];

  IdContext drawer_context = m_tileHWID->drawer_context();

  /* Set up buffers for handling information in a single collection. */
  IdentifierHash idhash;

  HWIdentifier drawer_id = m_tileHWID->drawer_id((*hitContItr)->identify());
  int ros = m_tileHWID->ros(drawer_id);
  int drawer = m_tileHWID->drawer(drawer_id);
  int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);


  // iterate over all hits in a collection
  TileHitCollection::const_iterator hitItr = (*hitContItr)->begin();
  TileHitCollection::const_iterator lastHit = (*hitContItr)->end();

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
        ATH_MSG_VERBOSE("BAD Overlay digits - skip hit in channel " << m_tileHWID->to_string(channel_id,-1));
      }
      continue;
    } else {
      ATH_MSG_VERBOSE("new hit in channel " << m_tileHWID->to_string(channel_id,-1));
    }

    /* Set gain=high and get digitSamples and calibration for this channel. */
    if (igain[ich] < 0)
      igain[ich] = TileID::HIGHGAIN;
    // conversion from scintillator energy to total cell energy (sampling fraction)  
    double hit_calib = m_tileInfo->HitCalib(pmt_id);

    // conversion to ADC counts for high gain
    double efactorHi = hit_calib / m_tileToolEmscale->channelCalib(drawerIdx, ich, TileID::HIGHGAIN, 1.
                                    , TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts);
    // conversion to ADC counts for low gain
    double efactorLo = hit_calib / m_tileToolEmscale->channelCalib(drawerIdx, ich, TileID::LOWGAIN, 1.
                                  , TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts);

    double* pDigitSamplesHi = drawerBufferHi[ich];
    double* pDigitSamplesLo = drawerBufferLo[ich];
    /* Loop over the subhits for this channel.  For each one,
     convolute with shaping function and add to digitSamples.                  */
    int n_hits = (*hitItr)->size();
    for (int ihit = 0; ihit < n_hits; ++ihit) {
      /* Get hit energy and convert to amplitude of high-gain and low-gain channel */
      double e_hit = (*hitItr)->energy(ihit);
      double amp_ch = e_hit * efactorHi;
      double amp_ch_lo = e_hit * efactorLo;
      double ech_sub = e_hit * hit_calib;
      double t_hit = (*hitItr)->time(ihit);

      ech_tot[ich] += ech_sub;
      if (fabs(t_hit) < 50.0) // ene within +/- 50 ns, used for filtered digits cut
        ech_int[ich] += ech_sub * mbts_extra_factor;
      ntot_ch[ich] += 1;
      nChSum += 1;
      HitSum += e_hit;
      EneSum += ech_sub;
      RChSum += amp_ch;

      // Assume time is in nanoseconds, use fine-grain shaping:
      int ishiftHi = (int) (t_hit / m_timeStepHi + 0.5);
      for (int js = 0; js < m_nSamples; ++js) {
        int k = m_binTime0Hi + (js - m_iTrig) * m_nBinsPerXHi - ishiftHi;
        if (k < 0)
          k = 0;
        else if (k > m_nShapeHi)
          k = m_nShapeHi;

        if (m_useCoolPulseShapes) {
          float phase = (k - m_binTime0Hi) * m_timeStepHi;
          float y, dy;
          m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, ich, 1, phase, y, dy);
          double ampl = (double) y;
          pDigitSamplesHi[js] += amp_ch * ampl;
          ATH_MSG_VERBOSE( "Sample no.=" << js
                           << " Pulse index=" << k
                           << " Shape wt. =" << ampl
                           << " HIGAIN from COOL");

        } else {
          pDigitSamplesHi[js] += amp_ch * m_digitShapeHi[k];
          ATH_MSG_VERBOSE( "Sample no.=" << js
                           << " Pulse index=" << k
                           << " Shape wt. =" << m_digitShapeHi[k]
                           << " HIGAIN from TileInfo");
        }

      }
    int ishiftLo = (int) (t_hit / m_timeStepLo + 0.5);
      for (int js = 0; js < m_nSamples; ++js) {
        int k = m_binTime0Lo + (js - m_iTrig) * m_nBinsPerXLo - ishiftLo;
        if (k < 0)
          k = 0;
        else if (k > m_nShapeLo)
          k = m_nShapeLo;

        if (m_useCoolPulseShapes) {
          float phase = (k - m_binTime0Lo) * m_timeStepLo;
          float y, dy;
          m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, ich, 0, phase, y, dy);
          double ampl = (double) y;
          pDigitSamplesLo[js] += amp_ch_lo * ampl;
          ATH_MSG_VERBOSE( "Sample no.=" << js
                           << " Pulse index=" << k
                           << " Shape wt. =" << ampl
                           << " LOGAIN from COOL");
        } else {
          pDigitSamplesLo[js] += amp_ch_lo * m_digitShapeLo[k];
          ATH_MSG_VERBOSE( "Sample no.=" << js
                           << " Pulse index=" << k
                           << " Shape wt. =" << m_digitShapeLo[k]
                           << " LOGAIN from TileInfo");
        }

      }

      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "subHit:  ch=" << ich
                          << " e_hit=" << e_hit
                          << " t_hit=" << t_hit
                          << " SamplesHi[" << m_iTrig << "]=" << pDigitSamplesHi[m_iTrig]
                          << " SamplesLo[" << m_iTrig << "]=" << pDigitSamplesLo[m_iTrig] << endmsg;
      }
    } /* end loop over sub-hits */
  } /* end loop over hits for this collection. */


  return StatusCode::SUCCESS;

}
