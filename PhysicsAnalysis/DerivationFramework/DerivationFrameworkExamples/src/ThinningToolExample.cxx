/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ThinningToolExample.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// This is a trivial example of an implementation of a thinning tool 
// which removes all ID tracks which do not pass a user-defined cut 

#include "DerivationFrameworkExamples/ThinningToolExample.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::ThinningToolExample::ThinningToolExample( 	const std::string& t,
                                                 		const std::string& n,
                                                 		const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  m_ntot(0),
  m_npass(0),
  m_trackPtCut(20.0)
  {
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("TrackPtCut", m_trackPtCut);	
  }
  
// Destructor
DerivationFramework::ThinningToolExample::~ThinningToolExample() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::ThinningToolExample::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::ThinningToolExample::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
     return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::ThinningToolExample::doThinning() const
{

      // Get the track container
       const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >("InDetTrackParticles");
      if(!tracks) {
         ATH_MSG_ERROR ("Couldn't retrieve TrackParticleContainer with key InDetTrackParticles");
         return StatusCode::FAILURE;
      }
      m_ntot+=tracks->size();	
      // Loop over tracks, see if they pass, set mask
      std::vector<bool> mask;
      for (xAOD::TrackParticleContainer::const_iterator trackIt=tracks->begin(); trackIt!=tracks->end(); ++trackIt) {
         if ( (*trackIt)->pt() > m_trackPtCut ) {++m_npass; mask.push_back(true);}
         else { mask.push_back(false); }
     }
     if (m_thinningSvc->filter(*tracks, mask, IThinningSvc::Operator::Or).isFailure()) {
         ATH_MSG_ERROR("Application of thinning service failed! ");
         return StatusCode::FAILURE;
     }

     return StatusCode::SUCCESS;
}  
  
