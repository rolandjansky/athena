/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticlePositionFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ISF_HepMC_Tools/GenParticlePositionFilter.h"

// HepMC includes
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

/** Constructor **/
ISF::GenParticlePositionFilter::GenParticlePositionFilter( const std::string& t,
                                                           const std::string& n,
                                                           const IInterface* p )
  : AthAlgTool(t,n,p),
    m_geoIDSvc("ISF_GeoIDSvc", n)
{
    declareInterface<ISF::IGenParticleFilter>(this);

    // the GeoID indentification service
    declareProperty("GeoIDService"      ,        m_geoIDSvc);
}


// Athena algtool's Hooks
StatusCode  ISF::GenParticlePositionFilter::initialize()
{
    ATH_MSG_VERBOSE("initialize()");

    // retrieve the GeoIDService
    if ( m_geoIDSvc.retrieve().isFailure()){
        ATH_MSG_ERROR( "Could not retrieve " << m_geoIDSvc << ". Abort.");
        return StatusCode::FAILURE;
    }

    ATH_MSG_VERBOSE("initialize() successful");
    return StatusCode::SUCCESS;
}


/** does the given particle pass the filter? */
bool ISF::GenParticlePositionFilter::pass(const HepMC::GenParticle& particle) const
{
  // the GenParticle production vertex
  HepMC::GenVertex* vtx = particle.production_vertex();

  // no production vertex?
  if (!vtx) {
    ATH_MSG_DEBUG("GenParticle does not have a production vertex, filtering it out");
    return false;
  }

  // (x,y,z) position
  HepMC::ThreeVector pos = vtx->point3d();

  // check if the particle position is inside (or on surface)
  // of any of the ISF Simulation GeoIDs
  bool inside = false;
  for ( short geoID=AtlasDetDescr::fFirstAtlasRegion;
        (geoID<AtlasDetDescr::fNumAtlasRegions) && (!inside);
        ++geoID) {
    // consult the GeoID identification service
    ISF::InsideType insideCheck = m_geoIDSvc->inside( pos.x(),
                                                      pos.y(),
                                                      pos.z(),
                                                      AtlasDetDescr::AtlasRegion(geoID) );
    // is inside only if ==fInside or ==fSurface
    inside |= (insideCheck==ISF::fInside) || (insideCheck==ISF::fSurface);
  }

  // return whether pos was inside any of the simulation geometries
  if (inside)
    ATH_MSG_VERBOSE("GenParticle is inside AtlasVolume and passed the filter");
  else
    ATH_MSG_VERBOSE("GenParticle is outside AtlasVolume and got fitered out");
  return inside;
}


StatusCode  ISF::GenParticlePositionFilter::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}

