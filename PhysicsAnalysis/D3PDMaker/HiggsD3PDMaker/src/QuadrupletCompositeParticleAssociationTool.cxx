/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: QuadrupletCompositeParticleAssociationTool.cxx 485019 2012-02-24 16:16:04Z krasznaa $

// Local include(s):
#include "QuadrupletCompositeParticleAssociationTool.h"

namespace D3PD {

   QuadrupletCompositeParticleAssociationTool::QuadrupletCompositeParticleAssociationTool( const std::string& type,
                                                                                           const std::string& name,
                                                                                           const IInterface* parent )
      : SingleAssociationTool< HSG2::Quadruplet, CompositeParticle >( type, name, parent ) {

   }

   const CompositeParticle* QuadrupletCompositeParticleAssociationTool::get( const HSG2::Quadruplet& q ) {

      return *( q.particle() );
   }

} // namespace D3PD
