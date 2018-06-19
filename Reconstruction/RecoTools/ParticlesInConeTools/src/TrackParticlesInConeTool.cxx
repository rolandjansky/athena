/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// TrackParticlesInConeTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

#include "TrackParticlesInConeTool.h"

namespace xAOD {
 
  TrackParticlesInConeTool::TrackParticlesInConeTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent)
  {
    declareInterface<ITrackParticlesInConeTool>(this);
  }

  TrackParticlesInConeTool::~TrackParticlesInConeTool()
  {}

  StatusCode TrackParticlesInConeTool::initialize() {
    ATH_CHECK(m_indetTrackParticleLocation.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode TrackParticlesInConeTool::finalize() {
    return StatusCode::SUCCESS;
  }

  const TrackParticlesInConeTool::LookUpTable*
  TrackParticlesInConeTool::getTable() const
  {
    const std::string tableName = name() + "LookUpTable";
    SG::ReadHandle<LookUpTable> rh (tableName);
    if (rh.isValid())
      return &*rh;

    SG::ReadHandle<TrackParticleContainer> indetTrackParticles(m_indetTrackParticleLocation);
    if( !indetTrackParticles.isValid() ) {
      ATH_MSG_ERROR("Could not open the TrackParticle container with key " 
		    << m_indetTrackParticleLocation.key());
      return nullptr;
    }
    auto lut = std::make_unique<LookUpTable>();
    lut->init(*indetTrackParticles);
    SG::WriteHandle<LookUpTable> wh (tableName);
    return wh.put (std::move (lut), true);
  }

  bool TrackParticlesInConeTool::particlesInCone( float eta, float phi, float dr, std::vector< const TrackParticle*>& output ) const {

    /// initialize if needed
    const LookUpTable* lut = getTable();
    return lut->iParticlesInCone( eta, phi, dr, output );
  }
}	// end of namespace
