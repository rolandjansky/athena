/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// NANOThinningTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Kerim Suruliz (Kerim.Suruliz@cern.ch)
// This thinning tool that thins all objects in the containers
// whose names are supplied to the tool. The thinning is done in the OR
// mode, so if any other tool keps the objects, they will ultimately
// be kept.

#include "DerivationFrameworkExamples/NANOThinningTool.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::NANOThinningTool::NANOThinningTool( 	const std::string& t,
                                                 		const std::string& n,
                                                 		const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  m_containersToThin()
  {
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("ContainersToThin", m_containersToThin);
  }
  
// Destructor
DerivationFramework::NANOThinningTool::~NANOThinningTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::NANOThinningTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::NANOThinningTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::NANOThinningTool::doThinning() const
{

  // Loop over names of containers to thin

  for (const auto &contName : m_containersToThin) {
    
    // Get the container
    const xAOD::IParticleContainer* objects = evtStore()->retrieve< const xAOD::IParticleContainer >(contName);
    if(!objects) {
      ATH_MSG_ERROR ("Couldn't retrieve IParticleContainer with key " << contName);
      return StatusCode::FAILURE;
    }
    
    // Loop over objects, see if they pass, set mask
    std::vector<bool> mask( objects->size(), false );
    
    if (m_thinningSvc->filter(*objects, mask, IThinningSvc::Operator::Or).isFailure()) {
      ATH_MSG_ERROR("Application of thinning service failed! ");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}  
  
