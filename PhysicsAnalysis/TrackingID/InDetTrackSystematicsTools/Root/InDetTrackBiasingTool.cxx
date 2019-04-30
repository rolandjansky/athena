/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    declareProperty("runNumber", m_runNumber);
    declareProperty("isData", m_isData);
    declareProperty("isSimulation", m_isSimulation);

    declareProperty("calibFileData15", m_calibFileData15 = "InDetTrackSystematicsTools/CalibData_21.2_2018-v18/data15_13TeV_all_CorrectionResult.root");
    declareProperty("calibFileData16_preTS1", m_calibFileData16_preTS1 = "InDetTrackSystematicsTools/CalibData_21.2_2018-v18/data16_13TeV_preTS1_CorrectionResult.root");
    declareProperty("calibFileData16_postTS1", m_calibFileData16_postTS1 = "InDetTrackSystematicsTools/CalibData_21.2_2018-v18/data16_13TeV_preTS1_CorrectionResult.root");
    declareProperty("calibFileData17_preFire", m_calibFileData17_preFire = "InDetTrackSystematicsTools/CalibData_21.2_2018-v18/data17_13TeV_preFire_CorrectionResult.root");
    declareProperty("calibFileData17_postFire", m_calibFileData17_postFire = "InDetTrackSystematicsTools/CalibData_21.2_2018-v18/data17_13TeV_postFire_CorrectionResult.root");
    declareProperty("calibFileData18", m_calibFileData18 = "InDetTrackSystematicsTools/CalibData_21.2_2018-v21/z_2018_weak_mode_recommendations.root");
    
    m_firstTime=true;
  }

  InDetTrackBiasingTool::~InDetTrackBiasingTool() = default;

  StatusCode InDetTrackBiasingTool::initialize()
  {
    
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

    ATH_MSG_INFO( "Using for Data15 the calibration file " << PathResolverFindCalibFile(m_calibFileData15) );
    ATH_MSG_INFO( "Using for Data16 preTS1 the calibration file " << PathResolverFindCalibFile(m_calibFileData16_preTS1) );
    ATH_MSG_INFO( "Using for Data16 postTS1 the calibration file " << PathResolverFindCalibFile(m_calibFileData16_postTS1) );
    ATH_MSG_INFO( "Using for Data17 preFire the calibration file " << PathResolverFindCalibFile(m_calibFileData17_preFire) );
    ATH_MSG_INFO( "Using for Data17 postFire the calibration file " << PathResolverFindCalibFile(m_calibFileData17_postFire) );
    ATH_MSG_INFO( "Using for Data18 the calibration file " << PathResolverFindCalibFile(m_calibFileData18) );

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

    float phi = track.phi0();
    float eta = track.eta();

    if (m_firstTime) {
      m_firstTime = false; // don't try to do this again
      if ( ! firstCall().isSuccess() ) { // this will check data vs. MC and run number.
	return CP::CorrectionCode::Error;
      }
    }

    // declare static accessors to avoid repeating string lookups
    static SG::AuxElement::Accessor< float > accD0( "d0" );
    static SG::AuxElement::Accessor< float > accZ0( "z0" );
    static SG::AuxElement::Accessor< float > accQOverP( "qOverP" );
    
    // do the biasing
    if ( m_doD0Bias ) {
      bool d0WmActive = isActive( TRK_BIAS_D0_WM );
      if ( m_isData || d0WmActive ) {
	accD0( track ) += readHistogram(m_biasD0, m_biasD0Histogram, phi, eta);
	if ( m_isData && d0WmActive ) {
	  accD0( track ) += readHistogram(0., m_biasD0HistError, phi, eta);
	}
      }
    }
    if ( m_doZ0Bias ) {
      bool z0WmActive = isActive( TRK_BIAS_Z0_WM );
      if ( m_isData || z0WmActive ) {
	accZ0( track ) += readHistogram(m_biasZ0, m_biasZ0Histogram, phi, eta);
	if ( m_isData && z0WmActive ) {
	  accZ0( track ) += readHistogram(0., m_biasZ0HistError, phi, eta);
	}
      }
    }
    if ( m_doQoverPBias ) {
      bool qOverPWmActive = isActive( TRK_BIAS_QOVERP_SAGITTA_WM );
      if ( m_isData || qOverPWmActive ) {
	auto sinTheta = 1.0/cosh(eta);
	// readHistogram flips the sign of the correction if m_isSimulation is true
	accQOverP( track ) += 1.e-6*sinTheta*readHistogram(m_biasQoverPsagitta, m_biasQoverPsagittaHistogram, phi, eta);
	if ( m_isData && qOverPWmActive ) {
	  accQOverP( track ) += 1.e-6*sinTheta*readHistogram(0., m_biasQoverPsagittaHistError, phi, eta);
	}
      }
    }

    return CP::CorrectionCode::Ok;
  }

  StatusCode InDetTrackBiasingTool::initHistograms(int runNumber)
  {
    
    string rootfileName;
    if (runNumber <= 0) {
      ATH_MSG_WARNING( "Run number not set." );
    }
    if (runNumber >= 286282 && runNumber <= 287931) {
      ATH_MSG_INFO( "Calibrating for 2015 HI and 5 TeV pp runs (286282 to 287931)." );
      ATH_MSG_ERROR( "The 5 TeV and heavy ion runs do not have biasing maps for release 21. "
		     "Contact the tracking CP group to discuss the derivation of these maps." );
      m_biasD0Histogram = nullptr;
      m_biasZ0Histogram = nullptr;
      m_biasQoverPsagittaHistogram = nullptr;
      m_biasD0HistError = nullptr;
      m_biasZ0HistError = nullptr;
      m_biasQoverPsagittaHistError = nullptr;
      return StatusCode::FAILURE;
    } else if (runNumber <= 364485) {
      if (runNumber < 297730) {
        ATH_MSG_INFO( "Calibrating for 2015 runs (before 297730)." ); // 2015
        rootfileName = m_calibFileData15;
      } else if (runNumber <= 300908) {
        ATH_MSG_INFO( "Calibrating for 2016 runs before IBL temperature change (297730 to 300908)." ); // pre-TSI 2016
        rootfileName = m_calibFileData16_preTS1;
      } else if (runNumber <= 311481) {
        ATH_MSG_INFO( "Calibrating for 2016 runs after IBL temperature change (301912 to 311481)." ); // post-TS1 2016
        rootfileName = m_calibFileData16_postTS1;
      } else if (runNumber <= 334737) {
        ATH_MSG_INFO( "Calibrating for 2017 runs pre-fire (323427 to 334737)." ); // pre-fire 2017
        rootfileName = m_calibFileData17_preFire;
      } else if (runNumber <= 341649) {
        ATH_MSG_INFO( "Calibrating for 2017 runs post-fire (334842 to 341649)." ); // post-fire 2017
        rootfileName = m_calibFileData17_postFire;
      } else {
        ATH_MSG_INFO( "Calibrating for 2018 data taking.");
        rootfileName = m_calibFileData18;
      }
      ATH_CHECK ( initObject<TH2>(m_biasD0Histogram, rootfileName, "d0/theNominal_d0") );
      ATH_CHECK ( initObject<TH2>(m_biasZ0Histogram, rootfileName, "z0/theNominal_z0") );
      ATH_CHECK ( initObject<TH2>(m_biasQoverPsagittaHistogram, rootfileName, "sagitta/theNominal_sagitta") );
      ATH_CHECK ( initObject<TH2>(m_biasD0HistError, rootfileName, "d0/theUncertainty_d0") );
      ATH_CHECK ( initObject<TH2>(m_biasZ0HistError, rootfileName, "z0/theUncertainty_z0") );
      ATH_CHECK ( initObject<TH2>(m_biasQoverPsagittaHistError, rootfileName, "sagitta/theUncertainty_sagitta") );
    } else {
      ATH_MSG_ERROR( "Run number = " << runNumber << " not in recognized range (286282 to 364485)." );
      return StatusCode::FAILURE;
    }

    
    m_doD0Bias = m_biasD0Histogram != nullptr;
    m_doZ0Bias = m_biasZ0Histogram != nullptr;
    m_doQoverPBias = m_biasQoverPsagittaHistogram != nullptr;
    
    if (!m_doD0Bias) ATH_MSG_WARNING( "Will not perform d0 bias." );
    if (!m_doZ0Bias) ATH_MSG_WARNING( "Will not perform z0 bias." );
    if (!m_doQoverPBias) ATH_MSG_WARNING( "Will not perform q/p sagitta bias." );

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
      runNumber = m_runNumber;
    }
    if ( ! initHistograms( runNumber ).isSuccess() ) {
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  float InDetTrackBiasingTool::readHistogram(float fDefault, TH2* histogram, float phi, float eta) const {
    if (histogram == nullptr) {
      ATH_MSG_ERROR( "Configuration histogram is invalid. Check the run number and systematic configuration combination.");
      throw std::runtime_error( "invalid configuration" );
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
