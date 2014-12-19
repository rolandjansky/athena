/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODASSOCIATIONS_TRACKPARTICLECLUSTERASSOCIATIONAUXCONTAINER_H
#define XAODASSOCIATIONS_TRACKPARTICLECLUSTERASSOCIATIONAUXCONTAINER_H
 
// Local include(s):
#include "xAODAssociations/versions/TrackParticleClusterAssociationAuxContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current Muon auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::TrackParticleClusterAssociationContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef TrackParticleClusterAssociationAuxContainer_v1 TrackParticleClusterAssociationAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackParticleClusterAssociationAuxContainer , 1108408842 , 1 )

#endif // XAODASSOCIATIONS_TRACKPARTICLECLUSTERASSOCIATIONAUXCONTAINER_H
