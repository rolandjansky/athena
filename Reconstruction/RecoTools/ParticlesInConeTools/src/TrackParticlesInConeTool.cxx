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
    declareProperty("TrackParticleLocation",m_indetTrackParticleLocation = "InDetTrackParticles");
  }

  TrackParticlesInConeTool::~TrackParticlesInConeTool()
  {}

  StatusCode TrackParticlesInConeTool::initialize() {
    return StatusCode::SUCCESS;
  }

  StatusCode TrackParticlesInConeTool::finalize() {
    return StatusCode::SUCCESS;
  }

  const TrackParticleContainer* TrackParticlesInConeTool::retrieveTrackParticleContainer() const {
    // retrieve MuonSpectrometer tracks
    const TrackParticleContainer* indetTrackParticles = 0;
    if(evtStore()->contains<TrackParticleContainer>(m_indetTrackParticleLocation)) {
      if(evtStore()->retrieve(indetTrackParticles,m_indetTrackParticleLocation).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << m_indetTrackParticleLocation );
        return 0;
      }
    }
    return indetTrackParticles;
  }

  const TrackParticlesInConeTool::LookUpTable*
  TrackParticlesInConeTool::getTable() const
  {
    const std::string tableName = name() + "LookUpTable";
    SG::ReadHandle<LookUpTable> rh (tableName);
    if (rh.isValid())
      return &*rh;

    const TrackParticleContainer* indetTrackParticles = retrieveTrackParticleContainer();
    if( !indetTrackParticles ) return nullptr;
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
