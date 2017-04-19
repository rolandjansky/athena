/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSystematicsTools/InDetTrackBiasingTool.h"
#include "xAODEventInfo/EventInfo.h"
#include <math.h>

#include <TH2.h>
#include <TFile.h>

using std::string;

namespace {
  using TrackCorrTool_t = CP::CorrectionTool< xAOD::TrackParticleContainer >;
}

namespace InDet {

  static const CP::SystematicSet BiasSystematics = 
    {
      InDet::TrackSystematicMap[TRK_BIAS_D0_WM],
      InDet::TrackSystematicMap[TRK_BIAS_Z0_WM],
      InDet::TrackSystematicMap[TRK_BIAS_QOVERP_SAGITTA_WM],
    };

  InDetTrackBiasingTool::InDetTrackBiasingTool(const string& name) :
    InDetTrackSystematicsTool(name)
  {

#ifndef XAOD_STANDALONE
    declareInterface<IInDetTrackBiasingTool>(this);
#endif

    declareProperty("biasD0", m_biasD0);
    declareProperty("biasZ0", m_biasZ0);
    declareProperty("biasQoverPsagitta", m_biasQoverPsagitta);
    // it was shown that splitting into two periods did not affect the result significantly
    // this feature is now disabled to prevent confusion
    //    declareProperty("splitPeriods", m_splitPeriods);
    declareProperty("runNumber", m_runNumber);
    declareProperty("isData", m_isData);
    declareProperty("isSimulation", m_isSimulation);
  }

  InDetTrackBiasingTool::~InDetTrackBiasingTool() = default;

  StatusCode InDetTrackBiasingTool::initialize()
  {

    if (m_runNumber > 0) ATH_CHECK( initHistograms(m_runNumber) );
    
    if (m_isData && m_isSimulation) {
      ATH_MSG_ERROR( "Cannot manually set for both data and simulation!" );
      return StatusCode::FAILURE;
    }

    if (m_biasD0 != 0.) {
      ATH_MSG_INFO( "overall d0 bias added = " << m_biasD0
		    << " mm (not part of an official recommendation)" );
    }
    if (m_biasZ0 != 0.) {
      ATH_MSG_INFO( "overall z0 bias added = " << m_biasZ0
		    << " mm (not part of an official recommendation)" );
    }
    if (m_biasQoverPsagitta != 0.) {
      ATH_MSG_INFO( "overall QoverP sagitta bias added = " << m_biasQoverPsagitta
		    << " TeV^-1 (not part of an official recommendation)" );
    }

    ATH_CHECK( InDetTrackSystematicsTool::initialize() );

    return StatusCode::SUCCESS;
  }

  StatusCode InDetTrackBiasingTool::finalize() {
    m_runNumber = -1;
    delete m_biasD0Histogram; m_biasD0Histogram = nullptr;
    delete m_biasZ0Histogram; m_biasZ0Histogram = nullptr;
    delete m_biasQoverPsagittaHistogram; m_biasQoverPsagittaHistogram = nullptr;
    delete m_biasD0HistError; m_biasD0HistError = nullptr;
    delete m_biasZ0HistError; m_biasZ0HistError = nullptr;
    delete m_biasQoverPsagittaHistError; m_biasQoverPsagittaHistError = nullptr;
    return StatusCode::SUCCESS;
  }

  CP::CorrectionCode InDetTrackBiasingTool::applyCorrection(xAOD::TrackParticle& track) {

    float d0 = track.d0();
    float z0 = track.z0();
    float phi0 = track.phi0();
    float theta = track.theta();
    float qOverP = track.qOverP();

    float eta = track.eta();

    static bool firstTime = true;
    if (firstTime) {
      firstTime = false; // don't try to do this again
      if ( ! firstCall().isSuccess() ) { // this will check data vs. MC and run number.
	return CP::CorrectionCode::Error;
      }
    }

    bool d0WmActive = isActive( TRK_BIAS_D0_WM );
    bool z0WmActive = isActive( TRK_BIAS_Z0_WM );
    bool qOverPWmActive = isActive( TRK_BIAS_QOVERP_SAGITTA_WM );
    
    if ( m_isData || d0WmActive ) {
      d0 += readHistogram(m_biasD0, m_biasD0Histogram, phi0, eta);
      if ( m_isData && d0WmActive ) {
	d0 += readHistogram(0., m_biasD0HistError, phi0, eta);
      }
    }
    if ( m_isData || z0WmActive ) {
      z0 += readHistogram(m_biasZ0, m_biasZ0Histogram, phi0, eta);
      if ( m_isData && z0WmActive ) {
	z0 += readHistogram(0., m_biasZ0HistError, phi0, eta);
      }
    }
    if ( m_isData || qOverPWmActive ) {
      auto sinTheta = sin(theta);
      // readHistogram flips the sign of the correction if m_isSimulation is true
      qOverP += 1.e-6*sinTheta*readHistogram(m_biasQoverPsagitta, m_biasQoverPsagittaHistogram, phi0, eta);
      if ( m_isData && qOverPWmActive ) {
	qOverP += 1.e-6*sinTheta*readHistogram(0., m_biasQoverPsagittaHistError, phi0, eta);
      }
    }

    /// Set the defining parameters.     
    track.setDefiningParameters(d0, z0, phi0, theta, qOverP);
    
    return CP::CorrectionCode::Ok;
  }

  StatusCode InDetTrackBiasingTool::initHistograms(int runNumber)
  {
    string rootfileName;
    if (runNumber <= 0) {
      ATH_MSG_WARNING( "Run number not set. Defaulting to 2015 setting." );
    }
    if (runNumber < 297730) {
      ATH_MSG_INFO( "Calibrating for 2015 runs (before 297730)." );
      rootfileName = "correctionmaps_HighGran_IBLon_NoGRL_INDET_2015_datareproAll25ns_correctedEp.root";
      ATH_CHECK ( initObject<TH2>(m_biasD0Histogram, rootfileName, "d0CorrectionVsEtaPhi") );
      ATH_CHECK ( initObject<TH2>(m_biasZ0Histogram, rootfileName, "z0CorrectionVsEtaPhi") );
      ATH_CHECK ( initObject<TH2>(m_biasQoverPsagittaHistogram, rootfileName, "LambdaCorrectionVsEtaPhi_reweightedToEP") );
    } else if (runNumber <= 300908) {
      ATH_MSG_INFO( "Calibrating for 2016 runs before IBL temperature change (297730 to 300908)." ); // pre-TSI: 297730 - 300908
      rootfileName = "CorrectionsResult_PreTSI.root";
      ATH_CHECK ( initObject<TH2>(m_biasD0Histogram, rootfileName, "d0/theNominal_d0") );
      ATH_CHECK ( initObject<TH2>(m_biasZ0Histogram, rootfileName, "z0/theNominal_z0") );
      ATH_CHECK ( initObject<TH2>(m_biasQoverPsagittaHistogram, rootfileName, "sagitta/theNominal_sagitta") );
      rootfileName = "UncertaintiesResult_PreTSI.root";
      ATH_CHECK ( initObject<TH2>(m_biasD0HistError, rootfileName, "d0/theUncertainty_d0") );
      ATH_CHECK ( initObject<TH2>(m_biasZ0HistError, rootfileName, "z0/theUncertainty_z0") );
      ATH_CHECK ( initObject<TH2>(m_biasQoverPsagittaHistError, rootfileName, "sagitta/theUncertainty_sagitta") );
    } else if (runNumber <= 304178) { // pre d0 fix: 301912 - 304178
      ATH_MSG_INFO( "Calibrating for 2016 runs after IBL temperature change, before d0 fix (301912 to 304178)." );
      rootfileName = "CorrectionsResult_Pred0fix.root";
      ATH_CHECK ( initObject<TH2>(m_biasD0Histogram, rootfileName, "d0/theNominal_d0") );
      ATH_CHECK ( initObject<TH2>(m_biasZ0Histogram, rootfileName, "z0/theNominal_z0") );
      ATH_CHECK ( initObject<TH2>(m_biasQoverPsagittaHistogram, rootfileName, "sagitta/theNominal_sagitta") );
      rootfileName = "UncertaintiesResult_Pred0fix.root";
      ATH_CHECK ( initObject<TH2>(m_biasD0HistError, rootfileName, "d0/theUncertainty_d0") );
      ATH_CHECK ( initObject<TH2>(m_biasZ0HistError, rootfileName, "z0/theUncertainty_z0") );
      ATH_CHECK ( initObject<TH2>(m_biasQoverPsagittaHistError, rootfileName, "sagitta/theUncertainty_sagitta") );
    } else if (runNumber <= 311481) {  // post d0 fix: 304198 - 311481
      ATH_MSG_INFO( "Calibrating for 2016 runs after IBL temperature change, after d0 fix (304198 to 311481)." );
      rootfileName = "CorrectionsResult_Postd0fix.root";
      ATH_CHECK ( initObject<TH2>(m_biasD0Histogram, rootfileName, "d0/theNominal_d0") );
      ATH_CHECK ( initObject<TH2>(m_biasZ0Histogram, rootfileName, "z0/theNominal_z0") );
      ATH_CHECK ( initObject<TH2>(m_biasQoverPsagittaHistogram, rootfileName, "sagitta/theNominal_sagitta") );
      rootfileName = "UncertaintiesResult_Postd0fix.root";
      ATH_CHECK ( initObject<TH2>(m_biasD0HistError, rootfileName, "d0/theUncertainty_d0") );
      ATH_CHECK ( initObject<TH2>(m_biasZ0HistError, rootfileName, "z0/theUncertainty_z0") );
      ATH_CHECK ( initObject<TH2>(m_biasQoverPsagittaHistError, rootfileName, "sagitta/theUncertainty_sagitta") );
    } else {
      ATH_MSG_ERROR( "Run number = " << runNumber << " too large to be recognized (after 311481)." );
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode InDetTrackBiasingTool::firstCall()
  {
    assert( ! (m_isData && m_isSimulation) );
    // if this option is set, we need to figure out the run number
    const xAOD::EventInfo* ei = nullptr;
    auto sc = evtStore()->retrieve( ei, "EventInfo" );
    if ( ! sc.isSuccess() ) {
      if (m_runNumber <= 0 || !(m_isData||m_isSimulation)) {
	ATH_MSG_ERROR( "Unable to retrieve from event store. Manually set data/simulation and/or run number." );
	return CP::CorrectionCode::Error;
      }
    }
    bool isSim = ei->eventType( xAOD::EventInfo::IS_SIMULATION );
    if (isSim) {
      if ( m_isData ) {
	ATH_MSG_WARNING( "Manually set to data setting, but the type is detected as simulation." );
	ATH_MSG_WARNING( "Ensure that this behaviour is desired." );
      } else {
	m_isSimulation = true;
      }
    } else {
      if ( m_isSimulation ) {
	ATH_MSG_WARNING( "Manually set to simulation setting, but the type is detected as data." );
	ATH_MSG_WARNING( "Ensure that this behaviour is desired." );
      } else {
	m_isData = true;
      }
    }
    assert( m_isData != m_isSimulation ); // one must be true and the other false
    if (m_isData) ATH_MSG_INFO( "Set to data. Will apply biases to correct those observed in data." );
    if (m_isSimulation) ATH_MSG_INFO( "Set to simulation. Will apply biases in direction that is observed in data." );

    auto runNumber = ei->runNumber();
    if (m_runNumber > 0) {
      if ( m_runNumber != runNumber && ! isSim) { // only warn for mis-matched run number if it is a data file
	ATH_MSG_WARNING( "Manually-set run number (" << m_runNumber <<
			 ") does not match that from the event store (" << runNumber << ")." );
	ATH_MSG_WARNING( "Will use the manually set run number, but you must make sure this is the desired behaviour!" );
      }
    } else {
      if ( ! initHistograms( runNumber ).isSuccess() ) {
	return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }

  float InDetTrackBiasingTool::readHistogram(float fDefault, TH2* histogram, float phi, float eta) const {
    if (histogram == nullptr) {
      throw std::runtime_error( "Bias histogram is null. Check your run number / configuration combination." );
    }
    
    // safety measure:
    if( eta>2.499 )  eta= 2.499;
    if( eta<-2.499 ) eta=-2.499;

    // the sign assumes that we apply a correction opposite to what the maps give
    float f = histogram->GetBinContent(histogram->FindBin(eta, phi));
    if (m_isSimulation) f = -f;
    f += fDefault;   // should be zero unless a manual override is provided

    return f;
  }

  // // this function is not currently being used
  // float InDetTrackBiasingTool::readEtaHistogram(float fDefault, TH1* histogram, float eta) const {
  //   // safety measure:
  //   if( eta>2.499 )  eta= 2.499;
  //   if( eta<-2.499 ) eta=-2.499;

  //   float f = histogram->GetBinContent(histogram->FindBin(eta));
  //   f += fDefault;   // to be reconsidered

  //   return f;
  // }


  CP::CorrectionCode InDetTrackBiasingTool::correctedCopy( const xAOD::TrackParticle& in,
							    xAOD::TrackParticle*& out )
  {
    return TrackCorrTool_t::correctedCopy(in, out);
  }
  
  CP::CorrectionCode InDetTrackBiasingTool::applyContainerCorrection( xAOD::TrackParticleContainer& cont )
  {
    return TrackCorrTool_t::applyContainerCorrection(cont);
  }

  bool InDetTrackBiasingTool::isAffectedBySystematic( const CP::SystematicVariation& syst ) const
  {
    return InDetTrackSystematicsTool::isAffectedBySystematic( syst );
  }

  CP::SystematicSet InDetTrackBiasingTool::affectingSystematics() const
  {
    return BiasSystematics;
  }

  CP::SystematicSet InDetTrackBiasingTool::recommendedSystematics() const
  {
    return InDetTrackSystematicsTool::recommendedSystematics();
  }

  CP::SystematicCode InDetTrackBiasingTool::applySystematicVariation( const CP::SystematicSet& systs )
  {
    return InDetTrackSystematicsTool::applySystematicVariation(systs);
  }


}
