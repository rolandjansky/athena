/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkInDet/TrackParticleThinning.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>
#include "ExpressionEvaluation/ExpressionParserUser.icc"

// Constructor
DerivationFramework::TrackParticleThinning::TrackParticleThinning(const std::string& t,
                                                                  const std::string& n,
                                                                  const IInterface* p ) :
base_class(t,n,p),
m_selectionString(""),
m_ntot(0),
m_npass(0),
m_ntot_pix_states(0), 
m_npass_pix_states(0),
m_ntot_pix_measurements(0), 
m_npass_pix_measurements(0),
m_ntot_sct_states(0), 
m_npass_sct_states(0),
m_ntot_sct_measurements(0), 
m_npass_sct_measurements(0),
m_ntot_trt_states(0), 
m_npass_trt_states(0),
m_ntot_trt_measurements(0), 
m_npass_trt_measurements(0),
m_thinHitsOnTrack(false)
{
    // logic and selection settings
    declareProperty("SelectionString", m_selectionString);
    declareProperty("ThinHitsOnTrack", m_thinHitsOnTrack);
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
    if (!m_selectionString.empty()) {
       ATH_CHECK(initializeParser(m_selectionString));
    }

    //check xAOD::InDetTrackParticle collection
    ATH_CHECK( m_inDetSGKey.initialize (m_streamName) );
    ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");
    //check availability of xAOD::TrackStateValidation and xAOD::TrackMeasurementValidation containers
    if (m_thinHitsOnTrack) {
      ATH_MSG_INFO("Pixel states collection as source for thinning: " << m_statesPixSGKey.key());
      ATH_MSG_INFO("Pixel measurements collection as source for thinning: " << m_measurementsPixSGKey.key());
      ATH_CHECK( m_statesPixSGKey.initialize (m_streamName, !m_statesPixSGKey.empty()) );
      ATH_CHECK( m_measurementsPixSGKey.initialize (m_streamName, !m_measurementsPixSGKey.empty()) );

      ATH_MSG_INFO("SCT states collection as source for thinning: " << m_statesSctSGKey.key());
      ATH_MSG_INFO("SCT measurements collection as source for thinning: " << m_measurementsSctSGKey.key());
      ATH_CHECK( m_statesSctSGKey.initialize (m_streamName, !m_statesSctSGKey.empty()) );
      ATH_CHECK( m_measurementsSctSGKey.initialize (m_streamName, !m_measurementsSctSGKey.empty()) );

      ATH_MSG_INFO("TRT states collection as source for thinning: " << m_statesTrtSGKey.key());
      ATH_MSG_INFO("TRT measurements collection as source for thinning: " << m_measurementsTrtSGKey.key());
      ATH_CHECK( m_statesTrtSGKey.initialize (m_streamName, !m_statesTrtSGKey.empty()) );
      ATH_CHECK( m_measurementsTrtSGKey.initialize (m_streamName, !m_measurementsTrtSGKey.empty()) );
    }

    ATH_CHECK(m_SCTDetEleCollKey.initialize( !m_SCTDetEleCollKey.key().empty() ));

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
    ATH_CHECK( finalizeParser() );
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::TrackParticleThinning::doThinning() const
{
    const EventContext& ctx = Gaudi::Hive::currentContext();
    
    // Retrieve main TrackParticle collection
    SG::ThinningHandle<xAOD::TrackParticleContainer> importedTrackParticles
      (m_inDetSGKey, ctx);
    
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
    m_npass += std::count (mask.begin(), mask.end(), true);

    // Execute the thinning service based on the mask.
    importedTrackParticles.keep (mask);

    //If thinning hits on track, look over States and Measurements collections as well
    if (m_thinHitsOnTrack) {
      filterTrackHits (ctx,
                       TrkState_Pixel,
                       *importedTrackParticles,
                       mask,
                       m_statesPixSGKey,
                       m_measurementsPixSGKey,
                       m_ntot_pix_states,
                       m_ntot_pix_measurements,
                       m_npass_pix_states,
                       m_npass_pix_measurements);
      filterTrackHits (ctx,
                       TrkState_SCT,
                       *importedTrackParticles,
                       mask,
                       m_statesSctSGKey,
                       m_measurementsSctSGKey,
                       m_ntot_sct_states,
                       m_ntot_sct_measurements,
                       m_npass_sct_states,
                       m_npass_sct_measurements);
      filterTrackHits (ctx,
                       TrkState_TRT,
                       *importedTrackParticles,
                       mask,
                       m_statesTrtSGKey,
                       m_measurementsTrtSGKey,
                       m_ntot_trt_states,
                       m_ntot_trt_measurements,
                       m_npass_trt_states,
                       m_npass_trt_measurements);
    }
    
    return StatusCode::SUCCESS;
}


void DerivationFramework::TrackParticleThinning::filterTrackHits
  (const EventContext& ctx,
   MeasurementType detTypeToSelect,
   const xAOD::TrackParticleContainer& inputTrackParticles,
   const std::vector<bool>& inputMask,
   const SG::ThinningHandleKey<xAOD::TrackStateValidationContainer>& statesKey,
   const SG::ThinningHandleKey<xAOD::TrackMeasurementValidationContainer>& measurementsKey,
   std::atomic<unsigned int>& ntot_states,
   std::atomic<unsigned int>& ntot_measurements,
   std::atomic<unsigned int>& npass_states,
   std::atomic<unsigned int>& npass_measurements) const
{
  std::vector<bool> maskStates;
  std::vector<bool> maskMeasurements;

  selectTrackHits (inputTrackParticles, inputMask, detTypeToSelect,
                   maskStates, maskMeasurements);

  auto count = [] (const std::vector<bool>& m)
               { return std::count (m.begin(), m.end(), true); };
  npass_states += count (maskStates);
  npass_measurements += count (maskMeasurements);

  if (!statesKey.empty()) {
    SG::ThinningHandle<xAOD::TrackStateValidationContainer> importedStates
      (statesKey, ctx);
    unsigned int size_states = importedStates->size();
    if (size_states == 0) {
      ATH_MSG_WARNING("States container is empty: " << statesKey.key());
    }
    else {
      ntot_states += size_states;
      if (maskStates.size() > size_states) {
        ATH_MSG_ERROR("States mask size mismatch " << maskStates.size() <<
                      " > " << size_states);
      }
      maskStates.resize (size_states);
      importedStates.keep (maskStates);
    }
  }

  if (!measurementsKey.empty()) {
    SG::ThinningHandle<xAOD::TrackMeasurementValidationContainer> importedMeasurements
      (measurementsKey, ctx);
    unsigned int size_measurements = importedMeasurements->size();
    if (size_measurements == 0) {
      ATH_MSG_WARNING("Measurements container is empty: " << measurementsKey.key());
    }
    else {
      ntot_measurements += size_measurements;
      if (maskMeasurements.size() > size_measurements) {
        ATH_MSG_ERROR("Measurements mask size mismatch " << maskMeasurements.size() <<
                      " > " << size_measurements);
      }
      maskMeasurements.resize (size_measurements);
      importedMeasurements.keep (maskMeasurements);
    }
  }
}


void DerivationFramework::TrackParticleThinning::selectTrackHits(const xAOD::TrackParticleContainer& inputTrackParticles,
                                                                 const std::vector<bool>& inputMask,
								 MeasurementType detTypeToSelect,
								 std::vector<bool>& outputStatesMask, std::vector<bool>& outputMeasurementsMask) const
{
  // loop over track particles, consider only the ones pre-selected by the mask
  int trkIndex=-1;
  for (const xAOD::TrackParticle* trkIt : inputTrackParticles) {
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
      if (trkState_el.index() >= outputStatesMask.size()) {
        outputStatesMask.resize (trkState_el.index()+1);
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
      if (trkMeasurement_el.index() >= outputMeasurementsMask.size()) {
        outputMeasurementsMask.resize (trkMeasurement_el.index()+1);
      }
      outputMeasurementsMask[trkMeasurement_el.index()] = true;
    }
  } // end loop over xAOD::TrackParticle container
}
