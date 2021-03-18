/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TrackMeasurementThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: David Salek (David.Salek@cern.ch)
// Thinning tool for TrackMeasurementValidationContainer

#include "DerivationFrameworkInDet/TrackMeasurementThinning.h"
#include "StoreGate/ThinningHandle.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TrackMeasurementThinning::TrackMeasurementThinning( 	const std::string& t,
                                                 		const std::string& n,
                                                 		const IInterface* p ) : 
  base_class(t,n,p)
  {
  }
  
// Destructor
DerivationFramework::TrackMeasurementThinning::~TrackMeasurementThinning() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::TrackMeasurementThinning::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_selectionString.empty()) {
        ATH_MSG_FATAL("No inner detector track selection string provided!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Track thinning selection string: " << m_selectionString);}
    
    //check xAOD::InDetTrackParticle collection
    ATH_CHECK( m_SGKey.initialize (m_streamName) );
    ATH_MSG_INFO("Using " << m_SGKey << "as the source collection for thinning.");

    // Set up the text-parsing machinery for thinning the tracks directly according to user cuts
    if (!m_selectionString.empty()) {
       ATH_CHECK( initializeParser(m_selectionString) );
    }
    return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::TrackMeasurementThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" measurements, "<< m_npass<< " were retained ");
    ATH_CHECK( finalizeParser());
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::TrackMeasurementThinning::doThinning() const
{
    const EventContext& ctx = Gaudi::Hive::currentContext();

    // Get the cluster container
    SG::ThinningHandle<xAOD::TrackMeasurementValidationContainer> clusters
      (m_SGKey, ctx);

    // Check the event contains clusters
    unsigned int nClusters = clusters->size();
    if (nClusters==0) return StatusCode::SUCCESS;

    // Set up a mask with the same entries as the full collection
    std::vector<bool> mask;
    mask.assign(nClusters,false); // default: don't keep anything
    m_ntot += nClusters;

    // Execute the text parser and update the mask
    if (m_parser) {
        std::vector<int> entries =  m_parser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nClusters != nEntries ) {
            ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used the same collection as the input collection??");
            return StatusCode::FAILURE;
        } else {
            // set mask
            for (unsigned int i=0; i<nClusters; ++i) if (entries[i]==1) mask[i]=true;
        }
    }
    // Count the mask
    unsigned int n_pass=0;
    for (unsigned int i=0; i<nClusters; ++i) {
        if (mask[i]) ++n_pass;
    }
    m_npass += n_pass;
 
    // Execute the thinning service based on the mask. Finish.
    clusters.keep (mask);

    return StatusCode::SUCCESS;
}  
  
