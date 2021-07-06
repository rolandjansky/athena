/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetTrackSelectionToolWrapper.cxx
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkInDet/InDetTrackSelectionToolWrapper.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteDecorHandle.h"
#include <vector>
#include <string>
#include <string_view>

namespace DerivationFramework {

  InDetTrackSelectionToolWrapper::InDetTrackSelectionToolWrapper(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode InDetTrackSelectionToolWrapper::initialize()
  {
    if (m_decorationKey.empty()) {
      ATH_MSG_ERROR("No decoration prefix name provided for the output of InDetTrackSelectionToolWrapper!");
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_tracksKey.initialize() );
    std::string::size_type pos = m_tracksKey.key().find("+");
    pos =  (pos != std::string::npos) ? pos+1 : 0;
    std::string_view container_name( &(m_tracksKey.key().data()[pos]), m_tracksKey.key().size() - pos);
    std::string_view decor_name(m_decorationKey.key());
    if (!(decor_name.compare(0,m_tracksKey.key().size(),std::string_view(m_tracksKey.key()))==0 && decor_name.compare(0,container_name.size(),container_name)==0)) {
       m_decorationKey = std::string(container_name) + "." + m_decorationKey.key();
    }
    std::cout << "DEBUG " << name() << "  InDetTrackSelectionToolWrapper::initialize container key " << m_tracksKey.key() << " -> " << container_name << " decor=" << m_decorationKey.key() << std::endl;
    ATH_CHECK(m_decorationKey.initialize());
    ATH_MSG_INFO("Using " << m_tracksKey << "as the source collection for inner detector track particles");
    ATH_CHECK(m_tool.retrieve());
    ATH_MSG_INFO(" InDetTrackSelectionToolWrapper::initialize i: " << inputHandles().size() << " o:" << outputHandles().size() );
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
    SG::WriteDecorHandle<xAOD::TrackParticleContainer,bool > accept(m_decorationKey);
    for (xAOD::TrackParticleContainer::const_iterator trItr = tracks->begin(); trItr!=tracks->end(); ++trItr) {
      accept( **trItr ) = m_tool->accept(*trItr).getCutResult(0);
    } // end of loop over tracks

    return StatusCode::SUCCESS;
  }  

}
