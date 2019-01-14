/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileCondToolOfc.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCablingSvc.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"


//
//____________________________________________________________________
TileCondToolOfc::TileCondToolOfc(const std::string& type, const std::string& name,
                                 const IInterface* parent)
    : base_class(type, name, parent)
    , m_tileInfo(0)
{
  declareProperty("nSamples", m_nSamples = 7, "number of samples used in the run");
  declareProperty("OptFilterDeltaCorrelation", m_deltaCorrelation = false
                  , "true=> use delta correlation; false=>use calculation obtained from data");

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
  ATH_CHECK( m_tileToolPulseShape.retrieve() );

  if (!m_deltaCorrelation) {
    ATH_CHECK(m_tileToolAutoCr.retrieve());
  } else {
    m_tileToolAutoCr.disable();
  }

  //==== TileInfo
  ATH_CHECK( detStore()->retrieve(m_tileInfo, "TileInfo") );

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

  //=== Initialize max values
  ServiceHandle<TileCablingSvc> cablingSvc("TileCablingSvc", name());
  ATH_CHECK( cablingSvc.retrieve());

  const TileCablingService* cabling = cablingSvc->cablingService();
  if (!cabling) {
    ATH_MSG_ERROR( "Unable to retrieve TileCablingService" );
    return StatusCode::FAILURE;
  }


  //--------------------------------------------------------
  ATH_MSG_INFO( "TileCondToolOfc initialization completed. " );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolOfc::finalize() {
  ATH_MSG_DEBUG( "TileCondToolOfc finalize called" );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________

////////////////////////////////////////
StatusCode TileCondToolOfc::getOfcWeights(unsigned int drawerIdx
                                          , unsigned int channel
                                          , unsigned int gain
                                          , float& phase
                                          , bool of2
                                          , TileOfcWeightsStruct& weights) const
{
  ATH_MSG_DEBUG( "TileCondToolOfc weights, drawerIdx:" << drawerIdx
                << " channel: " << channel
                << " gain: " << gain
                << " phase: " << phase );

  std::fill (std::begin(weights.g), std::end(weights.g), 0);
  std::fill (std::begin(weights.dg), std::end(weights.dg), 0);
  std::fill (std::begin(weights.w_a), std::end(weights.w_a), 0);
  std::fill (std::begin(weights.w_b), std::end(weights.w_b), 0);
  std::fill (std::begin(weights.w_c), std::end(weights.w_c), 0);


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
  weights.n_samples = m_nSamples;
  weights.of2 = of2;

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
      weights.g[i] = PulseShape[i][0];
      weights.dg[i] = DPulseShape[i][0];

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
      weights.w_a[ismp] = (double) Result[ismp];
//      ATH_MSG_DEBUG( "w_a " << weights.w_a[ismp] << " ismp " << ismp  );

    }

    Result = solve(SystemMatrix, IndependTermsTime);

    for (int ismp = 0; ismp < m_nSamples; ismp++) {
      weights.w_b[ismp] = (double) Result[ismp];
//      ATH_MSG_DEBUG( "w_b " << weights.w_b[ismp] << " ismp " << ismp );
    }

    if (of2) { // OF2
      Result = solve(SystemMatrix, IndependTermsPed);

      for (int ismp = 0; ismp < m_nSamples; ismp++) {
        weights.w_c[ismp] = (double) Result[ismp];
//        ATH_MSG_DEBUG( "w_c " << weights.w_c[ismp] << " ismp " << ismp );
      }
    }

  }

#if 0
  else {
    for (int ismp=0; ismp<m_nSamples; ismp++) {
      weights.w_a[ismp] = 0.;
      weights.w_b[ismp] = 0.;
      weights.w_c[ismp] = 0.;
      weights.g[ismp] = 0.;
      weights.dg[ismp] = 0.;
    }
  }
#endif

  ATH_MSG_DEBUG( "...OFC weights fixed-phase calculated" );

  return StatusCode::SUCCESS;
}
