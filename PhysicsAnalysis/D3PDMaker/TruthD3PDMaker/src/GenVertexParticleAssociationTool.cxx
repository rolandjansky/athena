/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenVertexParticleAssociationTool.cxx 348274 2011-02-28 16:25:06Z krasznaa $

// Local include(s):
#include "GenVertexParticleAssociationTool.h"

namespace D3PD {

   GenVertexParticleAssociationTool::GenVertexParticleAssociationTool( const std::string& type,
                                                                       const std::string& name,
                                                                       const IInterface* parent )
      : Base( type, name, parent ) {

      declareProperty( "InParticles", m_inParticles = false,
                       "Associate the incoming particles of the vertex" );
   }

   StatusCode GenVertexParticleAssociationTool::reset( const HepMC::GenVertex& p ) {

#ifdef HEPMC3
      if( m_inParticles ) {
         m_partItr = p.particles_in().begin();
         m_partEnd = p.particles_in().end();
         return StatusCode::SUCCESS;
      }

      m_partItr = p.particles_out().begin();
      m_partEnd = p.particles_out().end();
#else
      if( m_inParticles ) {
         m_partItr = p.particles_in_const_begin();
         m_partEnd = p.particles_in_const_end();
         return StatusCode::SUCCESS;
      }

      m_partItr = p.particles_out_const_begin();
      m_partEnd = p.particles_out_const_end();
#endif
      return StatusCode::SUCCESS;
   }

#ifdef HEPMC3
const HepMC::GenParticle* GenVertexParticleAssociationTool::next() {
return nullptr;
}
#else
   const HepMC::GenParticle* GenVertexParticleAssociationTool::next() {

      if( m_partItr == m_partEnd ) return 0;

      // I like to be verbose about the following:
      const HepMC::GenParticle* part = *m_partItr;
      ++m_partItr;

      return part;
   }
#endif

} // namespace D3PD
