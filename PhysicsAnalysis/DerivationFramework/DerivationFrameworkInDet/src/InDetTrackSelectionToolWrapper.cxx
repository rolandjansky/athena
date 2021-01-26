/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetTrackSelectionToolWrapper.cxx
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkInDet/InDetTrackSelectionToolWrapper.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ReadHandle.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  InDetTrackSelectionToolWrapper::InDetTrackSelectionToolWrapper(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_tool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this ),
    m_sgName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TrackSelectionTool", m_tool);
    declareProperty("DecorationName", m_sgName);
    declareProperty("ContainerName", m_tracksKey);
  }

  StatusCode InDetTrackSelectionToolWrapper::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No decoration prefix name provided for the output of InDetTrackSelectionToolWrapper!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( m_tracksKey.initialize() );
    ATH_MSG_INFO("Using " << m_tracksKey << "as the source collection for inner detector track particles");
    ATH_CHECK(m_tool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode InDetTrackSelectionToolWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode InDetTrackSelectionToolWrapper::addBranches() const
  {

    // Get current context 
    const EventContext& ctx = Gaudi::Hive::currentContext();

    // retrieve track container
    SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_tracksKey, ctx);
    if (!tracks.isValid()) {
    //if( ! tracks ) {
        ATH_MSG_ERROR ("Couldn't retrieve TrackParticles with key: " << tracks.key() );
        return StatusCode::FAILURE;
    }
    // Run tool for each element and decorate with the decision 
    for (xAOD::TrackParticleContainer::const_iterator trItr = tracks->begin(); trItr!=tracks->end(); ++trItr) {
      SG::AuxElement::Decorator< bool > accept(m_sgName);
      accept( **trItr ) = m_tool->accept(*trItr).getCutResult(0);
    } // end of loop over tracks		 	 	  
    
    return StatusCode::SUCCESS;
  }  

}
