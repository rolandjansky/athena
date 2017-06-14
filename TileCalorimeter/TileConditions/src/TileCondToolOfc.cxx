/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileConditions/TileCondToolOfc.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCablingSvc.h"


//
//____________________________________________________________________
const InterfaceID& TileCondToolOfc::interfaceID() {
  return ITileCondToolOfc::interfaceID();
}

//
//____________________________________________________________________
TileCondToolOfc::TileCondToolOfc(const std::string& type, const std::string& name,
    const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_tileToolPulseShape("TileCondToolPulseShape")
    , m_tileToolAutoCr("TileCondToolAutoCr")
//  , m_tileToolNoiseSample("TileCondToolNoiseSample")
    , m_tileInfo(0)
    , m_maxChannels(0)
    , m_maxGains(0)
    , m_drawerCacheSize(0)
{
  declareInterface<ITileCondToolOfc>(this);
  declareProperty("nSamples", m_nSamples = 7, "number of samples used in the run");
  declareProperty("OptFilterDeltaCorrelation", m_deltaCorrelation = false
                  , "true=> use delta correlation; false=>use calculation obtained from data");
  declareProperty("TileCondToolPulseShape", m_tileToolPulseShape);
  declareProperty("TileCondToolAutoCr", m_tileToolAutoCr);
  declareProperty("CacheSize", m_cache = 0, ">0 create cache; ==0 calculate on-fly");

  m_t0Sample = (m_nSamples - 1) / 2;
}

//
//____________________________________________________________________
TileCondToolOfc::~TileCondToolOfc() {

}

//
//____________________________________________________________________
StatusCode TileCondToolOfc::initialize() {

  ATH_MSG_INFO( "In initialize()" );

  //=== Get TileCondToolPulseShape
  CHECK( m_tileToolPulseShape.retrieve() );

  if (!m_deltaCorrelation) {
    CHECK(m_tileToolAutoCr.retrieve());
  }

  //==== TileInfo
  CHECK( detStore()->retrieve(m_tileInfo, "TileInfo") );

  if (m_nSamples != m_tileInfo->NdigitSamples()) {
    ATH_MSG_WARNING( "Changing number of samples from " << m_nSamples
                    << " to " << m_tileInfo->NdigitSamples() );

    m_nSamples = m_tileInfo->NdigitSamples();
  } else {
    ATH_MSG_INFO( "Number of samples is " << m_nSamples );
  }

  m_t0Sample = (m_nSamples - 1) / 2;
  if (m_t0Sample != m_tileInfo->ItrigSample()) {
    ATH_MSG_WARNING( "Changing T0 sample from " << m_t0Sample
                    << " to " << m_tileInfo->ItrigSample() );

    m_t0Sample = m_tileInfo->ItrigSample();
  } else {
    ATH_MSG_INFO( "T0 sample is " << m_t0Sample );
  }

  //=== Prepare for calculation of OFCs
  m_weights.n_samples = m_nSamples;

  //=== Initialize max values
  ServiceHandle<TileCablingSvc> cablingSvc("TileCablingSvc", name());
  CHECK( cablingSvc.retrieve());

  const TileCablingService* cabling = cablingSvc->cablingService();
  if (!cabling) {
    ATH_MSG_ERROR( "Unable to retrieve TileCablingService" );
    return StatusCode::FAILURE;
  }

  m_maxChannels = cabling->getMaxChannels();
  m_maxGains = cabling->getMaxGains();
  m_drawerCacheSize = m_maxChannels * m_maxGains;

  // Prepare cache table for all channels (array of pointers to "MAP"s)
  if (m_cache) {

    m_ofc_phase_cache.reserve(m_drawerCacheSize * TileCalibUtils::MAX_DRAWERIDX);

    for (unsigned int drawerIdx = 0; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++ drawerIdx) {
      for (unsigned int channel = 0; channel < m_maxChannels; ++channel) {
        for (unsigned int adc = 0; adc < m_maxGains; ++adc) {
          m_ofc_phase_cache.push_back(std::make_unique<OfcPhaseCache>());
        }
      }
    }
  }


  //--------------------------------------------------------
  ATH_MSG_INFO( "TileCondToolOfc initialization completed. " );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolOfc::finalize() {
  ATH_MSG_DEBUG( "TileCondToolOfc finalize called" );

  if (m_cache) {
    unsigned int cache_size = 0;
    for (std::unique_ptr<OfcPhaseCache>& ofcCache : m_ofc_phase_cache) {

      ATH_MSG_DEBUG( "TileCondToolOfc cache table size: " << ofcCache->size()
                     << " " << sizeof(m_weights) );

      cache_size += ofcCache->size() * sizeof(m_weights);
    }
    ATH_MSG_INFO( "TileCondToolOfc total cache size " << cache_size << " bytes" );
  }

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________

////////////////////////////////////////
void TileCondToolOfc::CalcWeights(unsigned int drawerIdx
                                  , unsigned int channel
                                  , int gain
                                  , float phase
                                  , bool of2) {

  CLHEP::HepMatrix Correlation(m_nSamples, m_nSamples, 1);
  //aa , Inverse(m_nSamples,m_nSamples,0), Zero(m_nSamples,m_nSamples,0);
  CLHEP::HepMatrix PulseShape(m_nSamples, 1, 0), DPulseShape(m_nSamples, 1, 0);
  CLHEP::HepMatrix a(m_nSamples, 1, 0), b(m_nSamples, 1, 0);


  //int ierr=0;

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " Calculating " << (of2 ? "OF2" : "OF1") << " weights ";

    if (m_deltaCorrelation)
      msg(MSG::DEBUG) << "with Delta correlation matrix " << endmsg;
    else
      msg(MSG::DEBUG) << "with correlation matrix obtained from DB " << endmsg;

    msg(MSG::DEBUG) << "for drawerIdx= " << drawerIdx
                    << " channel=" << channel
                    << " gain=" << gain
                    << " phase " << phase << endmsg;
  }

  int npr = 2;
  if (of2) npr = 3;
  m_weights.n_samples = m_nSamples;
  m_weights.of2 = of2;

  if (!m_deltaCorrelation) { //=== Retrieve autocorrelations from COOL DB
    std::vector<float> vecAutoCr;
    m_tileToolAutoCr->getAutoCorr(drawerIdx, channel, gain, vecAutoCr);
    
    ATH_MSG_DEBUG( " vecAutoCr size: " << vecAutoCr.size() );
    ATH_MSG_VERBOSE( " vecAutoCr " << vecAutoCr );
    if (vecAutoCr[0] > -1233.){
      if (vecAutoCr.size() == 28){
        int AutoCrPosition = 0;
	for (int i = 0; i < m_nSamples; ++i)
	  for (int j = i; j < m_nSamples; ++j){
	    Correlation[i][j] = vecAutoCr[AutoCrPosition];
	    Correlation[j][i] = vecAutoCr[AutoCrPosition];
            ++AutoCrPosition;
	  }
      } else {
        for (int i = 0; i < m_nSamples; i++)
          Correlation[i][i] = 1.;
	for (int i = 0; i < m_nSamples - 1; i++) {// Fill non-diag. elements
          for (int j = 0; j < m_nSamples - i - 1; j++) {
            Correlation[i][j + i + 1] = vecAutoCr[j];
            Correlation[j + i + 1][i] = vecAutoCr[j];
          }
        }
      }
    }
  }
  
  ATH_MSG_VERBOSE( " Correlation: " << Correlation );

  float py = 0.;
  float pdy = 0.;
  //if (ierr==0)

  {
    for (int i = 0; i < m_nSamples; i++) {
      m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, channel, gain
                                             , phase + 25 * (i - m_t0Sample), py, pdy);

      PulseShape[i][0] = py;
      DPulseShape[i][0] = pdy;
      m_weights.g[i] = PulseShape[i][0];
      m_weights.dg[i] = DPulseShape[i][0];

      ATH_MSG_VERBOSE( " Pulse shape: isamp " << i
                      << " phase " << phase
                      << " py " << py
                      << " pdy " << pdy );

    }

    // Build System Matrix with Correlations and pulse function points

    CLHEP::HepMatrix SystemMatrix(m_nSamples + npr, m_nSamples + npr, 0);
    CLHEP::HepVector Result(m_nSamples + npr, 0);
    CLHEP::HepVector IndependTermsAmp(m_nSamples + npr, 0);
    CLHEP::HepVector IndependTermsTime(m_nSamples + npr, 0);
    CLHEP::HepVector IndependTermsPed(m_nSamples + npr, 0);

    for (int i = 0; i < m_nSamples; i++) {
      for (int j = 0; j < m_nSamples; j++) {
        SystemMatrix[i][j] = Correlation[i][j];
      }

      SystemMatrix[m_nSamples][i] = PulseShape[i][0];
      SystemMatrix[i][m_nSamples] = PulseShape[i][0];

      SystemMatrix[m_nSamples + 1][i] = DPulseShape[i][0];
      SystemMatrix[i][m_nSamples + 1] = DPulseShape[i][0];

      if (of2) {
        SystemMatrix[m_nSamples + 2][i] = 1.;
        SystemMatrix[i][m_nSamples + 2] = 1.;
      }
    }

    IndependTermsAmp[m_nSamples] = 1.;
    IndependTermsTime[m_nSamples + 1] = -1.;

    if (of2) IndependTermsPed[m_nSamples + 2] = 1.;

    Result = solve(SystemMatrix, IndependTermsAmp);

    for (int ismp = 0; ismp < m_nSamples; ismp++) {
      m_weights.w_a[ismp] = (double) Result[ismp];
//      ATH_MSG_DEBUG( "w_a " << m_weights.w_a[ismp] << " ismp " << ismp  );

    }

    Result = solve(SystemMatrix, IndependTermsTime);

    for (int ismp = 0; ismp < m_nSamples; ismp++) {
      m_weights.w_b[ismp] = (double) Result[ismp];
//      ATH_MSG_DEBUG( "w_b " << m_weights.w_b[ismp] << " ismp " << ismp );
    }

    if (of2) { // OF2
      Result = solve(SystemMatrix, IndependTermsPed);

      for (int ismp = 0; ismp < m_nSamples; ismp++) {
        m_weights.w_c[ismp] = (double) Result[ismp];
//        ATH_MSG_DEBUG( "w_c " << m_weights.w_c[ismp] << " ismp " << ismp );
      }
    }

  }

#if 0
  else {
    for (int ismp=0; ismp<m_nSamples; ismp++) {
      m_weights.w_a[ismp] = 0.;
      m_weights.w_b[ismp] = 0.;
      m_weights.w_c[ismp] = 0.;
      m_weights.g[ismp] = 0.;
      m_weights.dg[ismp] = 0.;
    }
  }
#endif

  ATH_MSG_DEBUG( "...OFC weights fixed-phase calculated" );

}

const TileOfcWeightsStruct* TileCondToolOfc::getOfcWeights(unsigned int drawerIdx
                                                           , unsigned int channel
                                                           , unsigned int adc
                                                           , float& phase
                                                           , bool of2) {

  ATH_MSG_DEBUG( "TileCondToolOfc weights, drawerIdx:" << drawerIdx
                << " channel: " << channel
                << " gain: " << adc
                << " phase: " << phase
                << " Cache " << m_cache );


  unsigned int adcIdx = 0;
  if (m_cache) adcIdx = cacheIndex(drawerIdx, channel, adc);
  // --- calculate on-fly if #of cached phases for this channel is too big
  if (m_cache && ((m_ofc_phase_cache[adcIdx])->size() < m_cache)) {
    int iphase = int(phase + (phase < 0 ? -0.5 : 0.5)); // 1 ns step

    if ((m_ofc_phase_cache[adcIdx])->find(iphase) == (m_ofc_phase_cache[adcIdx])->end()) {
      CalcWeights(drawerIdx, channel, adc, float(iphase), of2);
      (*m_ofc_phase_cache[adcIdx])[iphase] = std::make_unique<TileOfcWeightsStruct>(m_weights);
    }
    
    phase = float(iphase);
    return ((*m_ofc_phase_cache[adcIdx])[iphase]).get();
  } else {
    CalcWeights(drawerIdx, channel, adc, phase, of2);
    return &m_weights;
  }

}
//--------------------------------------------------------
int TileCondToolOfc::getNSamples() {
  return m_weights.n_samples;
}

