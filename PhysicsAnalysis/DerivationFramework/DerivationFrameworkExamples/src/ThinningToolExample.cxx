/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ThinningToolExample.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// This is a trivial example of an implementation of a thinning tool 
// which removes all ID tracks which do not pass a user-defined cut 

#include "DerivationFrameworkExamples/ThinningToolExample.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::ThinningToolExample::ThinningToolExample( 	const std::string& t,
                                                 		const std::string& n,
                                                 		const IInterface* p ) : 
  base_class(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_trackPtCut(20.0)
  {
    declareProperty("TrackPtCut", m_trackPtCut);	
  }
  
// Destructor
DerivationFramework::ThinningToolExample::~ThinningToolExample() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::ThinningToolExample::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     ATH_CHECK( m_inDetSGKey.initialize (m_streamName) );
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
      const EventContext& ctx = Gaudi::Hive::currentContext();

      // Get the track container
      SG::ThinningHandle<xAOD::TrackParticleContainer> tracks (m_inDetSGKey, ctx);
      m_ntot+=tracks->size();	
      // Loop over tracks, see if they pass, set mask
      std::vector<bool> mask;
      for (xAOD::TrackParticleContainer::const_iterator trackIt=tracks->begin(); trackIt!=tracks->end(); ++trackIt) {
         if ( (*trackIt)->pt() > m_trackPtCut ) {++m_npass; mask.push_back(true);}
         else { mask.push_back(false); }
     }
     tracks.keep (mask);

     return StatusCode::SUCCESS;
}  
  
