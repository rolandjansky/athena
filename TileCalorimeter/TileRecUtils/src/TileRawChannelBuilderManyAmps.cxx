/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// small hack to enable datapool usage
#include "TileEvent/TileRawChannel.h"
// Tile includes
#include "TileRecUtils/TileRawChannelBuilderManyAmps.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "TileRecUtils/TileFilterManager.h"
#include "TileRecUtils/TileFilterTester.h"

// Gaudi includes
#include "GaudiKernel/Property.h"

//Atlas includes
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"


// lang include
#include <algorithm>
#include <cmath>

// C++ STL includes
#include <vector>


/**
 * Standard constructor
 */
TileRawChannelBuilderManyAmps::TileRawChannelBuilderManyAmps(const std::string& type,
    const std::string& name, const IInterface *parent)
  : TileRawChannelBuilder(type, name, parent)
  , m_tileFilterManagerHi(0)
  , m_tileFilterManagerLo(0)
{
  //declare interfaces
  declareInterface< TileRawChannelBuilder >( this );
  declareInterface< TileRawChannelBuilderManyAmps >(this);
    
  m_rawChannelContainerKey = "TileRawChannelManyAmps";

  //declare properties
  declareProperty("FilterMode",  m_digitFilterMode = 2);  // can be 2 or 3
  declareProperty("FilterLevel", m_digitFilterLevel = 5); // number of parameters for fit (3-9 for mode 2)
  declareProperty("FilterTester", m_digitFilterTest = 0);  // non-zero means call Tester (during initialization phase).  
}


/**
 * Destructor
 */
TileRawChannelBuilderManyAmps::~TileRawChannelBuilderManyAmps() {
}


/**
 * Initialize
 */
StatusCode TileRawChannelBuilderManyAmps::initialize() {

  ATH_MSG_INFO( "TileRawChannelBuilderManyAmps::initialize()" );

  m_rChType = TileFragHash::ManyAmps;

  // init in superclass
  CHECK( TileRawChannelBuilder::initialize() );


  //  **************************************************************************************
  ATH_MSG_INFO( " TileRawChannelBuilderManyAmps init:"
               << " FilterMode=" << m_digitFilterMode
               << ", FilterLevel=" << m_digitFilterLevel
               << ", FilterTest=" << m_digitFilterTest );

  if (!(m_digitFilterMode == 2) && !(m_digitFilterMode == 3)) { // filter mode different from 2 or 3

    ATH_MSG_WARNING( "Wrong Filter mode defined: FilterMode=" << m_digitFilterMode );
    ATH_MSG_WARNING( "Switching to FilterMode=2" );
    m_digitFilterMode = 2;
  }

  //  if( (m_digitFilterMode == 2) || (m_digitFilterMode == 3) ) { 
  /* Get information needed to initialize TileFilterManager. */
  int Nsamp = m_tileInfo->NdigitSamples();
  // Set maximum number of parameters to fit, using FilterLevel
  int nParamMax = m_digitFilterLevel;
  if (nParamMax > Nsamp + 1) nParamMax = Nsamp;
  if (nParamMax < 3) nParamMax = 3;
  int InTsamp = m_tileInfo->ItrigSample();
  int jBsamp = 0;
  int jEsamp = Nsamp - 1;
  int jBcross = 0;
  int jEcross = Nsamp - 1;
  int Nshape = m_tileInfo->NdigitSamples();
  int InTshape = m_tileInfo->ItrigSample(); // need new method to give this !!! 
  std::vector<double> ShapeXHi = m_tileInfo->digitsShapeHi();
  std::vector<double> ShapeXLo = m_tileInfo->digitsShapeLo();

  bool lVerbose = msgLvl(MSG::VERBOSE);
  m_tileFilterManagerHi = new TileFilterManager(m_digitFilterMode, m_digitFilterLevel, nParamMax,
      Nsamp, InTsamp, jBsamp, jEsamp, jBcross, jEcross, Nshape, InTshape, ShapeXHi, lVerbose);

  m_tileFilterManagerLo = new TileFilterManager(m_digitFilterMode, m_digitFilterLevel, nParamMax,
      Nsamp, InTsamp, jBsamp, jEsamp, jBcross, jEcross, Nshape, InTshape, ShapeXLo, lVerbose);

  // Run TileFilterTester if m_digitFilterTest in non-zero).

  if (m_digitFilterTest > 0) {

    TileFilterTester* tFilterTestHi = new TileFilterTester(m_tileFilterManagerHi
        , m_digitFilterMode, m_digitFilterTest, lVerbose);

    tFilterTestHi->genEvents(10);
    delete tFilterTestHi;

    TileFilterTester* tFilterTestLo = new TileFilterTester(m_tileFilterManagerLo
        , m_digitFilterMode, m_digitFilterTest, lVerbose);

    tFilterTestLo->genEvents(10);
    delete tFilterTestLo;
  } // end FilterTest

  ATH_MSG_DEBUG( "TileRawChannelBuilderManyAmps::initialize() completed successfully" );

  return StatusCode::SUCCESS;
}

/**
 *  Finalize
 */
StatusCode TileRawChannelBuilderManyAmps::finalize() {

  ATH_MSG_DEBUG( "Finalizing" );
  delete m_tileFilterManagerHi;
  delete m_tileFilterManagerLo;
  return StatusCode::SUCCESS;
}

TileRawChannel* TileRawChannelBuilderManyAmps::rawChannel(const TileDigits* tiledigits) {
  ++m_chCounter;

  const HWIdentifier adcId = tiledigits->adc_HWID();
  unsigned int drawerIdx(0), channel(0), gain(0);
  m_tileIdTransforms->getIndices(adcId, drawerIdx, channel, gain);

  bool lVerbose(false);
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Running ManyAmps Fit for TileRawChannel with HWID "
                      << m_tileHWID->to_string(adcId) << endmsg;
    lVerbose = true;
  }


  /* get pedestal value assumed for reconstruction */
  //double digSigma = m_tileInfo->DigitsResolution(gain);
  // new way to get channel-dependent sigma:
  // but we lost difference between sigma used in digitization and 
  // sigma assumed in reconstruction - it's the same sigma now
  double digSigma = m_tileToolNoiseSample->getHfn(drawerIdx, channel, gain);

  /* Get vector of time-slice amplitudes. */
  std::vector<float> digits = tiledigits->samples();
  double amp_ch = 0;      // Fitted amplitude of RC (to be returned from Filtering code).
  double err_ch;      // Error in amp_ch (from Filtering code)
  double ped_ch;      // Fitted pedestal of raw channel (from Filtering).
  double chisq_ch = 0.; // Chisq resulting from Filtering.
  double t_ch = 0.;     // Fitted time to be supplied by Filtering code
  // ---------------------------------------------------------------------------------------

  //int icode = 0;  

  // Instantiate tResult, which will collect results from Filtering code.
  TileFilterResult tResult(digits, digSigma);
  // Call Fitter to extract the in-time pulse height and related info.
  if (TileID::HIGHGAIN == gain)
  /*icode =*/m_tileFilterManagerHi->fitDigits(tResult, lVerbose);

  if (TileID::LOWGAIN == gain)
  /*icode =*/m_tileFilterManagerLo->fitDigits(tResult, lVerbose);

  tResult.getInTime(amp_ch, err_ch, ped_ch, chisq_ch, t_ch);

  ATH_MSG_VERBOSE( " TileRawChannelBuilderManyAmps: return from FilterManager/Fitter."
                  << ", chisq_ch=" << chisq_ch );

  // convert to pCb (if needed)
  if (m_calibrateEnergy) {
    amp_ch = m_tileToolEmscale->doCalibCis(drawerIdx, channel, gain, amp_ch);
  }
  // we know that time is zero here, put negative chi^2 to indicate that
  chisq_ch = -fabs(chisq_ch);

  //  TileRawChannel *rawCh = new TileRawChannel(adcId,amp_ch,t_ch,chisq_ch);
  DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  rawCh->assign (adcId, amp_ch, t_ch, chisq_ch, 0);
  ATH_MSG_VERBOSE(  "Creating RawChannel"
                  << " a=" << amp_ch
                  << " t=" << t_ch
                  << " q=" << chisq_ch );

  if (TileID::HIGHGAIN == gain) {
    ++m_nChH;
    m_RChSumH += amp_ch;
  } else {
    ++m_nChL;
    m_RChSumL += amp_ch;
  }

  return rawCh;
}
