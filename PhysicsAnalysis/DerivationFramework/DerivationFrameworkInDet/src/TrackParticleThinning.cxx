/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkInDet/TrackParticleThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TrackParticleThinning::TrackParticleThinning(const std::string& t,
                                                                  const std::string& n,
                                                                  const IInterface* p ) :
AthAlgTool(t,n,p),
m_thinningSvc("ThinningSvc",n),
m_parser(0),
m_selectionString(""),
m_ntot(0),
m_npass(0),
m_inDetSGKey("InDetTrackParticles"),
m_ntot_pix_states(0), 
m_npass_pix_states(0),
m_statesPixSGKey("PixelMSOSs"),
m_ntot_pix_measurements(0), 
m_npass_pix_measurements(0),
m_measurementsPixSGKey("PixelClusters"),
m_ntot_sct_states(0), 
m_npass_sct_states(0),
m_statesSctSGKey("SCT_MSOSs"),
m_ntot_sct_measurements(0), 
m_npass_sct_measurements(0),
m_measurementsSctSGKey("SCT_Clusters"),
m_ntot_trt_states(0), 
m_npass_trt_states(0),
m_statesTrtSGKey("TRT_MSOSs"),
m_ntot_trt_measurements(0), 
m_npass_trt_measurements(0),
m_measurementsTrtSGKey("TRT_DriftCircles"),
m_and(false),
m_thinHitsOnTrack(false)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    //thinning service
    declareProperty("ThinningService", m_thinningSvc);
    // logic and selection settings
    declareProperty("SelectionString", m_selectionString);
    declareProperty("ApplyAnd", m_and);
    declareProperty("ThinHitsOnTrack", m_thinHitsOnTrack);
    //keys for xAOD::TrackParticle container
    declareProperty("InDetTrackParticlesKey", m_inDetSGKey);
    //keys for xAOD::TrackStateValidation and xAOD::TrackMeasurementValidation containers
    declareProperty("InDetTrackStatesPixKey", m_statesPixSGKey);
    declareProperty("InDetTrackMeasurementsPixKey", m_measurementsPixSGKey);
    declareProperty("InDetTrackStatesSctKey", m_statesSctSGKey);
    declareProperty("InDetTrackMeasurementsSctKey", m_measurementsSctSGKey);
    declareProperty("InDetTrackStatesTrtKey", m_statesTrtSGKey);
    declareProperty("InDetTrackMeasurementsTrtKey", m_measurementsTrtSGKey);
}

// Destructor
DerivationFramework::TrackParticleThinning::~TrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TrackParticleThinning::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_selectionString=="") {
        ATH_MSG_FATAL("No inner detector track selection string provided!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Track thinning selection string: " << m_selectionString);}
    
    // Set up the text-parsing machinery for thinning the tracks directly according to user cuts
    if (m_selectionString!="") {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
	    m_parser->loadExpression(m_selectionString);
    }

    //check xAOD::InDetTrackParticle collection
    if (m_inDetSGKey=="") {
        ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");}
    //check availability of xAOD::TrackStateValidation and xAOD::TrackMeasurementValidation containers
    if (m_thinHitsOnTrack) {
      ATH_MSG_INFO("Pixel states collection as source for thinning: " << m_statesPixSGKey);
      ATH_MSG_INFO("Pixel measurements collection as source for thinning: " << m_measurementsPixSGKey);
      if ((m_statesPixSGKey == "") or (m_measurementsPixSGKey == "")) {
	ATH_MSG_WARNING("No Pixel measurement or state collection specified. Pixel hits won't be thinned.");
      }

      ATH_MSG_INFO("SCT states collection as source for thinning: " << m_statesSctSGKey);
      ATH_MSG_INFO("SCT measurements collection as source for thinning: " << m_measurementsSctSGKey);
      if ((m_statesSctSGKey == "") or (m_measurementsSctSGKey == "")) {
	ATH_MSG_WARNING("No SCT measurement or state collection specified. SCT hits won't be thinned.");
      } 

      ATH_MSG_INFO("TRT states collection as source for thinning: " << m_statesTrtSGKey);
      ATH_MSG_INFO("TRT measurements collection as source for thinning: " << m_measurementsTrtSGKey);
      if ((m_statesTrtSGKey == "") or (m_measurementsTrtSGKey == "")) {
	ATH_MSG_WARNING("No TRT measurement or state collection specified. TRT hits won't be thinned.");
      } 
    }
        
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    if (m_thinHitsOnTrack) {
      ATH_MSG_INFO("Pixel state objects thinning, Total / Passed (Efficiency): " 
		   << m_ntot_pix_states << " / " << m_npass_pix_states 
		   << " (" << (m_ntot_pix_states == 0 ? 0 : static_cast<float>(m_npass_pix_states) / m_ntot_pix_states) << ")");
      ATH_MSG_INFO("Pixel measurements objects thinning, Total / Passed (Efficiency): " 
		   << m_ntot_pix_measurements << " / " << m_npass_pix_measurements 
		   << " (" << (m_ntot_pix_measurements == 0 ? 0 : static_cast<float>(m_npass_pix_measurements) / m_ntot_pix_measurements) << ")");
      ATH_MSG_INFO("SCT state objects thinning, Total / Passed (Efficiency): " 
		   << m_ntot_sct_states << " / " << m_npass_sct_states 
		   << " (" << (m_ntot_sct_states == 0 ? 0 : static_cast<float>(m_npass_sct_states) / m_ntot_sct_states) << ")");
      ATH_MSG_INFO("SCT measurements objects thinning, Total / Passed (Efficiency): " 
		   << m_ntot_sct_measurements << " / " << m_npass_sct_measurements 
		   << " (" << (m_ntot_sct_measurements == 0 ? 0 : static_cast<float>(m_npass_sct_measurements) / m_ntot_sct_measurements) << ")");
      ATH_MSG_INFO("TRT state objects thinning, Total / Passed (Efficiency): " 
		   << m_ntot_trt_states << " / " << m_npass_trt_states 
		   << " (" << (m_ntot_trt_states == 0 ? 0 : static_cast<float>(m_npass_trt_states) / m_ntot_trt_states) << ")");
      ATH_MSG_INFO("TRT measurements objects thinning, Total / Passed (Efficiency): " 
		   << m_ntot_trt_measurements << " / " << m_npass_trt_measurements 
		   << " (" << (m_ntot_trt_measurements == 0 ? 0 : static_cast<float>(m_npass_trt_measurements) / m_ntot_trt_measurements) << ")");
    }
    if (m_parser) {
        delete m_parser;
        m_parser = 0;
    }
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::TrackParticleThinning::doThinning() const
{
    
    // Retrieve main TrackParticle collection
    const xAOD::TrackParticleContainer* importedTrackParticles;
    if (evtStore()->retrieve(importedTrackParticles,m_inDetSGKey).isFailure()) {
        ATH_MSG_ERROR("No TrackParticle collection with name " << m_inDetSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    
    // Check the event contains tracks
    unsigned int nTracks = importedTrackParticles->size();
    if (nTracks==0) return StatusCode::SUCCESS;
    
    // Set up a mask with the same entries as the full TrackParticle collection
    std::vector<bool> mask;
    mask.assign(nTracks,false); // default: don't keep any tracks
    m_ntot += nTracks;
    
    // Execute the text parser and update the mask
    if (m_parser) {
    	std::vector<int> entries =  m_parser->evaluateAsVector();
    	unsigned int nEntries = entries.size();
    	// check the sizes are compatible
    	if (nTracks != nEntries ) {
    		ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used ID TrackParticles?");
            return StatusCode::FAILURE;
    	} else {
            // set mask
            for (unsigned int i=0; i<nTracks; ++i) if (entries[i]==1) mask[i]=true;
    	}
    }
    // Count the mask
    for (unsigned int i=0; i<nTracks; ++i) {
        if (mask[i]) ++m_npass;
    }

    //If thinning hits on track, look over States and Measurements collections as well
    // note: declare containers and masks anyway
    const xAOD::TrackStateValidationContainer *importedStatesPix(0), 
      *importedStatesSct(0), *importedStatesTrt(0);
    std::vector<bool> maskStatesPix, maskStatesSct, maskStatesTrt;
    const xAOD::TrackMeasurementValidationContainer *importedMeasurementsPix(0), 
      *importedMeasurementsSct(0), *importedMeasurementsTrt(0);
    std::vector<bool> maskMeasurementsPix, maskMeasurementsSct, maskMeasurementsTrt;
    if (m_thinHitsOnTrack) {
      // -- Retrieve containers and setup mask vectors
      ATH_MSG_DEBUG("Retrieving states and measurements containers");
      // Pixels
      if (evtStore()->retrieve(importedStatesPix,m_statesPixSGKey).isFailure()) {
        ATH_MSG_WARNING("Skipping Pixel states thinning. No xAOD::TrackStateValidation Pixel collection with name " 
			<< m_statesPixSGKey << " found in StoreGate.");
      } else {
	unsigned int size_states_pix = importedStatesPix->size();
	if (size_states_pix == 0) {
	  ATH_MSG_WARNING("Pixel states container is empty: " << m_statesPixSGKey);
	} else {
	  maskStatesPix.assign(size_states_pix,false); // default: don't keep any
	  m_ntot_pix_states += size_states_pix;
	}
      }
      if (evtStore()->retrieve(importedMeasurementsPix,m_measurementsPixSGKey).isFailure()) {
        ATH_MSG_WARNING("Skipping Pixel measurements thinning. No xAOD::TrackMeasurementValidation Pixel collection with name " 
			<< m_measurementsPixSGKey << " found in StoreGate.");
      } else {
	unsigned int size_measurements_pix = importedMeasurementsPix->size();
	if (size_measurements_pix == 0) {
	  ATH_MSG_WARNING("Pixel measurements container is empty: " << m_measurementsPixSGKey);
	} else {
	  maskMeasurementsPix.assign(size_measurements_pix,false); // default: don't keep any
	  m_ntot_pix_measurements += size_measurements_pix;
	}
      }
      // SCT
      if (evtStore()->retrieve(importedStatesSct,m_statesSctSGKey).isFailure()) {
        ATH_MSG_WARNING("Skipping SCT states thinning. No xAOD::TrackStateValidation SCT collection with name " 
			<< m_statesSctSGKey << " found in StoreGate.");
      } else {
	unsigned int size_states_sct = importedStatesSct->size();
	if (size_states_sct == 0) {
	  ATH_MSG_WARNING("SCT states container is empty: " << m_statesSctSGKey);
	} else {
	  maskStatesSct.assign(size_states_sct,false); // default: don't keep any
	  m_ntot_sct_states += size_states_sct;
	}
      }
      if (evtStore()->retrieve(importedMeasurementsSct,m_measurementsSctSGKey).isFailure()) {
        ATH_MSG_WARNING("Skipping SCT measurements thinning. No xAOD::TrackMeasurementValidation SCT collection with name " 
			<< m_measurementsSctSGKey << " found in StoreGate.");
      } else {
	unsigned int size_measurements_sct = importedMeasurementsSct->size();
	if (size_measurements_sct == 0) {
	  ATH_MSG_WARNING("SCT measurements container is empty: " << m_measurementsSctSGKey);
	} else {
	  maskMeasurementsSct.assign(size_measurements_sct,false); // default: don't keep any
	  m_ntot_sct_measurements += size_measurements_sct;
	}
      }
      // TRT
      if (evtStore()->retrieve(importedStatesTrt,m_statesTrtSGKey).isFailure()) {
        ATH_MSG_WARNING("Skipping TRT states thinning. No xAOD::TrackStateValidation TRT collection with name " 
			<< m_statesTrtSGKey << " found in StoreGate.");
      } else {
	unsigned int size_states_trt = importedStatesTrt->size();
	if (size_states_trt == 0) {
	  ATH_MSG_WARNING("TRT states container is empty: " << m_statesTrtSGKey);
	} else {
	  maskStatesTrt.assign(size_states_trt,false); // default: don't keep any
	  m_ntot_trt_states += size_states_trt;
	}
      }
      if (evtStore()->retrieve(importedMeasurementsTrt,m_measurementsTrtSGKey).isFailure()) {
        ATH_MSG_WARNING("Skipping TRT measurements thinning. No xAOD::TrackMeasurementValidation TRT collection with name " 
			<< m_measurementsTrtSGKey << " found in StoreGate.");
      } else {
	unsigned int size_measurements_trt = importedMeasurementsTrt->size();
	if (size_measurements_trt == 0) {
	  ATH_MSG_WARNING("TRT measurements container is empty: " << m_measurementsTrtSGKey);
	} else {
	  maskMeasurementsTrt.assign(size_measurements_trt,false); // default: don't keep any
	  m_ntot_trt_measurements += size_measurements_trt;      
	}
      }

      // -- Thin containers based on selected xAOD::TrackParticle objects
      ATH_MSG_DEBUG("Now calculating thinning masks the Pixel hits containers");
      selectTrackHits(importedTrackParticles, mask, TrkState_Pixel, maskStatesPix, maskMeasurementsPix);
      ATH_MSG_DEBUG("Now calculating thinning masks the SCT hits containers");
      selectTrackHits(importedTrackParticles, mask, TrkState_SCT, maskStatesSct, maskMeasurementsSct);
      ATH_MSG_DEBUG("Now calculating thinning masks the TRT hits containers");
      selectTrackHits(importedTrackParticles, mask, TrkState_TRT, maskStatesTrt, maskMeasurementsTrt);

      if (importedStatesPix)       for (bool isPassed : maskStatesPix) if (isPassed) m_npass_pix_states++;
      if (importedMeasurementsPix) for (bool isPassed : maskMeasurementsPix) if (isPassed) m_npass_pix_measurements++;
      if (importedStatesSct)       for (bool isPassed : maskStatesSct) if (isPassed) m_npass_sct_states++;
      if (importedMeasurementsSct) for (bool isPassed : maskMeasurementsSct) if (isPassed) m_npass_sct_measurements++;
      if (importedStatesTrt)       for (bool isPassed : maskStatesTrt) if (isPassed) m_npass_trt_states++;
      if (importedMeasurementsTrt) for (bool isPassed : maskMeasurementsTrt) if (isPassed) m_npass_trt_measurements++;
    }
    
    // Execute the thinning service based on the mask. Finish.
      IThinningSvc::Operator::Type thinningOperator;
      if (m_and) thinningOperator = IThinningSvc::Operator::And;
      else thinningOperator = IThinningSvc::Operator::Or;
      if (m_thinningSvc->filter(*importedTrackParticles, mask, thinningOperator).isFailure()) {
	ATH_MSG_ERROR("Application of thinning service failed! ");
	return StatusCode::FAILURE;
      }

      if (m_thinHitsOnTrack) {
	//apply thinning to non-null containers
	if (importedStatesPix) {
	  ATH_MSG_DEBUG("Now thinning the Pixel states containers");
	  if (m_thinningSvc->filter(*importedStatesPix, maskStatesPix, thinningOperator).isFailure()) {
	    ATH_MSG_ERROR("Application of thinning service failed for Pixel States! ");
	    return StatusCode::FAILURE;
	  }	  
	}
	if (importedMeasurementsPix) {
	  ATH_MSG_DEBUG("Now thinning the Pixel measurements containers");       
	  if (m_thinningSvc->filter(*importedMeasurementsPix, maskMeasurementsPix, thinningOperator).isFailure()) {
	    ATH_MSG_ERROR("Application of thinning service failed for Pixel Measurements! ");
	    return StatusCode::FAILURE;
	  }
	}
	if (importedStatesSct) {
	  ATH_MSG_DEBUG("Now thinning the SCT states containers");
	  if (m_thinningSvc->filter(*importedStatesSct, maskStatesSct, thinningOperator).isFailure()) {
	    ATH_MSG_ERROR("Application of thinning service failed for SCT States! ");
	    return StatusCode::FAILURE;
	  }	  
	}
	if (importedMeasurementsSct) {
	  ATH_MSG_DEBUG("Now thinning the SCT measurements containers");
	  if (m_thinningSvc->filter(*importedMeasurementsSct, maskMeasurementsSct, thinningOperator).isFailure()) {
	    ATH_MSG_ERROR("Application of thinning service failed for SCT Measurements! ");
	    return StatusCode::FAILURE;
	  }
	}
	if (importedStatesTrt) {
	  ATH_MSG_DEBUG("Now thinning the TRT states containers");
	  if (m_thinningSvc->filter(*importedStatesTrt, maskStatesTrt, thinningOperator).isFailure()) {
	    ATH_MSG_ERROR("Application of thinning service failed for TRT States! ");
	    return StatusCode::FAILURE;
	  }	  
	}
	if (importedMeasurementsTrt) {
	  ATH_MSG_DEBUG("Now thinning the TRT measurements containers");
	  if (m_thinningSvc->filter(*importedMeasurementsTrt, maskMeasurementsTrt, thinningOperator).isFailure()) {
	    ATH_MSG_ERROR("Application of thinning service failed for TRT Measurements! ");
	    return StatusCode::FAILURE;
	  }
	}
	
      } //m_thinHitsOnTrack
    
    return StatusCode::SUCCESS;
}

void DerivationFramework::TrackParticleThinning::selectTrackHits(const xAOD::TrackParticleContainer *inputTrackParticles, std::vector<bool>& inputMask,
								 MeasurementType detTypeToSelect,
								 std::vector<bool>& outputStatesMask, std::vector<bool>& outputMeasurementsMask) const
{
  // loop over track particles, consider only the ones pre-selected by the mask
  int trkIndex=-1;
  for (const xAOD::TrackParticle* trkIt : *inputTrackParticles) {
    trkIndex++;
    if (not inputMask[trkIndex]) continue;

    // loop over the TrackStateValidation objects, and add them to the outputStatesMask    
    typedef std::vector<ElementLink< xAOD::TrackStateValidationContainer > > StatesOnTrack;
    static const char* trackStateNames = "msosLink";
    if( ! trkIt->isAvailable< StatesOnTrack >( trackStateNames ) ) {
      ATH_MSG_DEBUG("Cannot find TrackState link from xAOD::TrackParticle. Skipping track.");
      continue;
    }
    const StatesOnTrack& measurementsOnTrack = trkIt->auxdataConst< StatesOnTrack >( trackStateNames );
    for( const ElementLink<xAOD::TrackStateValidationContainer>& trkState_el : measurementsOnTrack) {
      if (not trkState_el.isValid()) {
	ATH_MSG_DEBUG("Cannot find a valid link to TrackStateValidation object for track index: " << trkIndex);
	continue; //not a valid link
      }
      if ((*trkState_el)->detType() != detTypeToSelect) {
	ATH_MSG_VERBOSE("Discarding TrackState as not of correct type " << detTypeToSelect);
	continue;
      }
      outputStatesMask[trkState_el.index()] = true;

      // get the corresponding TrackMeasurementValidation object, if any, and add it to the outputMeasurementsMask
      const ElementLink<xAOD::TrackMeasurementValidationContainer> trkMeasurement_el = (*trkState_el)->trackMeasurementValidationLink();
      if (not trkMeasurement_el.isValid()) {
	ATH_MSG_VERBOSE("Cannot find a valid link to TrackMeasurementValidation object from track state for track index: " << trkIndex
			<< ", trackState index: " << trkState_el.index());
	continue; //not a valid link
      }
      if (*trkMeasurement_el == 0) {
	ATH_MSG_VERBOSE("Invalid pointer to TrackMeasurementValidation object from track state for track index: " << trkIndex
			<< ", trackState index: " << trkState_el.index());
	continue; //not linking to a valid object -- is it necessary?
      }
      outputMeasurementsMask[trkMeasurement_el.index()] = true;
    }
  } // end loop over xAOD::TrackParticle container
}
