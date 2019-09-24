/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ExampleAlg.h"

ExampleAlg::ExampleAlg (const std::string& name, ISvcLocator* svcLocator)
  : AthReentrantAlgorithm (name, svcLocator)
{ }

StatusCode ExampleAlg::initialize()
{
  ATH_CHECK( m_trackParticleKey.initialize());
  ATH_CHECK( m_algTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode ExampleAlg::execute(const EventContext& ctx) const
{
  SG::ReadHandle<xAOD::TrackParticleContainer> handle(m_trackParticleKey, ctx);
  ATH_MSG_VERBOSE(ctx<< " Got back "<<handle->size()<<" trackparticles");
  unsigned int count=0;
  for (auto tp : *handle) {
    if ( m_algTool->selectTrackParticle( *tp ) ) count++;
  }
  ATH_MSG_VERBOSE(ctx << " " << count << " pass selection");
  
  return StatusCode::SUCCESS;
}
