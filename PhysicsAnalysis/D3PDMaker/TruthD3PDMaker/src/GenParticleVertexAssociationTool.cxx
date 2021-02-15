/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleVertexAssociationTool.cxx 348274 2011-02-28 16:25:06Z krasznaa $

// Local include(s):
#include "GenParticleVertexAssociationTool.h"

namespace D3PD {

   GenParticleVertexAssociationTool::GenParticleVertexAssociationTool( const std::string& type,
                                                                       const std::string& name,
                                                                       const IInterface* parent )
      : Base( type, name, parent ) {

      declareProperty( "DecayVertex", m_decay = false,
                       "When set to True, the tool will return the decay vertex and not the "
                       "production vertex" );
   }

#ifdef HEPMC3
const HepMC::GenVertex* GenParticleVertexAssociationTool::get( const HepMC::GenParticle& p ) {
      if( m_decay ) return p.end_vertex().get();
      return p.production_vertex().get();
}
#else
   const HepMC::GenVertex* GenParticleVertexAssociationTool::get( const HepMC::GenParticle& p ) {

      if( m_decay ) return p.end_vertex();
      return p.production_vertex();
   }
#endif

} // namespace D3PD
