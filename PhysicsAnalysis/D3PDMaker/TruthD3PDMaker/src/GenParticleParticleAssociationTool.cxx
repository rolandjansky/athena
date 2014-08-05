/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleParticleAssociationTool.cxx 348274 2011-02-28 16:25:06Z krasznaa $

// Local include(s):
#include "GenParticleParticleAssociationTool.h"

namespace D3PD {

   GenParticleParticleAssociationTool::GenParticleParticleAssociationTool( const std::string& type,
                                                                           const std::string& name,
                                                                           const IInterface* parent )
      : Base( type, name, parent ) {

   }

   const HepMC::GenParticle* GenParticleParticleAssociationTool::get( const HepMC::GenParticle& p ) {

      return &p;
   }

} // namespace D3PD
