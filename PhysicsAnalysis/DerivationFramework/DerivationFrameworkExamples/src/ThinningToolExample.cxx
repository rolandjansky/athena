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
#include "egammaEvent/PhotonContainer.h"
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
  m_photonSGKey("PhotonAODCollection"),
  m_photonPtCut(20.0)
  {
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("PhotonContainerKey", m_photonSGKey);
    declareProperty("PhotonPtCut", m_photonPtCut);	
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

     // Retrieve track container	
     const PhotonContainer* importedPhotons(0);
     if (evtStore()->retrieve(importedPhotons,m_photonSGKey).isFailure()) {
         ATH_MSG_ERROR("No photon collection with name " << m_photonSGKey << " found in StoreGate!");
         return StatusCode::FAILURE;
     } 
     
     m_ntot+=importedPhotons->size();

     // Loop over muons, count up and set decision
     std::vector<bool> mask;
     PhotonContainer::const_iterator phoItr;
     for (phoItr=importedPhotons->begin(); phoItr!=importedPhotons->end(); ++phoItr) {  
         if ( (*phoItr)->pt() > m_photonPtCut ) {++m_npass; mask.push_back(true);}
         else { mask.push_back(false); }
     }
     if (m_thinningSvc->filter(*importedPhotons, mask, IThinningSvc::Operator::Or).isFailure()) {
         ATH_MSG_ERROR("Application of thinning service failed! ");
         return StatusCode::FAILURE;
     }

     return StatusCode::SUCCESS;
}  
  
