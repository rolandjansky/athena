/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetTrackSelectionToolWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkInDet/InDetTrackSelectionToolWrapper.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  InDetTrackSelectionToolWrapper::InDetTrackSelectionToolWrapper(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_tool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this ),
    m_sgName(""),
    m_containerName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TrackSelectionTool", m_tool);
    declareProperty("DecorationName", m_sgName);
    declareProperty("ContainerName", m_containerName);
  }

  StatusCode InDetTrackSelectionToolWrapper::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No decoration prefix name provided for the output of InDetTrackSelectionToolWrapper!");
      return StatusCode::FAILURE;
    }
    if (m_containerName=="") {
      ATH_MSG_ERROR("No TrackParticle collection provided for InDetTrackSelectionToolWrapper!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_tool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode InDetTrackSelectionToolWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode InDetTrackSelectionToolWrapper::addBranches() const
  {

    // retrieve track container
    const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_containerName );
    if( ! tracks ) {
        ATH_MSG_ERROR ("Couldn't retrieve TrackParticles with key: " << m_containerName );
        return StatusCode::FAILURE;
    }
    // Run tool for each element and decorate with the decision 
    for (xAOD::TrackParticleContainer::const_iterator trItr = tracks->begin(); trItr!=tracks->end(); ++trItr) {
      SG::AuxElement::Decorator< bool > accept(m_sgName);
      accept( **trItr ) = m_tool->accept(*trItr);
    } // end of loop over tracks		 	 	  
    
    return StatusCode::SUCCESS;
  }  

}
