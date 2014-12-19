/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODASSOCIATIONS_TRACKPARTICLECLUSTERASSOCIATIONCONTAINER_H
#define XAODASSOCIATIONS_TRACKPARTICLECLUSTERASSOCIATIONCONTAINER_H
 
// Local include(s):
#include "xAODAssociations/TrackParticleClusterAssociation.h"
#include "xAODAssociations/versions/TrackParticleClusterAssociationContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current "TrackParticleClusterAssociation container version"
   typedef TrackParticleClusterAssociationContainer_v1 TrackParticleClusterAssociationContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackParticleClusterAssociationContainer , 1245808135 , 1 )

#endif // XAODASSOCIATIONS_TRACKPARTICLECLUSTERASSOCIATIONCONTAINER_H
