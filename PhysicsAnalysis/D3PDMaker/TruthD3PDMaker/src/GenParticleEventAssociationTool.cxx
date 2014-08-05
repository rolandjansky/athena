/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleEventAssociationTool.cxx 348274 2011-02-28 16:25:06Z krasznaa $

// Local include(s):
#include "GenParticleEventAssociationTool.h"

namespace D3PD {

   GenParticleEventAssociationTool::GenParticleEventAssociationTool( const std::string& type,
                                                                     const std::string& name,
                                                                     const IInterface* parent )
      : Base( type, name, parent ) {

   }

   const HepMC::GenEvent* GenParticleEventAssociationTool::get( const HepMC::GenParticle& p ) {

      return p.parent_event();
   }

} // namespace D3PD
