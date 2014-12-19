/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODASSOCIATIONS_TRACKPARTICLECLUSTERASSOCIATION_H
#define XAODASSOCIATIONS_TRACKPARTICLECLUSTERASSOCIATION_H

// Local include(s):
#include "versions/TrackParticleClusterAssociation_v1.h"

namespace xAOD {
  /// Reference the current persistent version:
  typedef TrackParticleClusterAssociation_v1 TrackParticleClusterAssociation;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackParticleClusterAssociation , 221270585 , 1 )

#endif // XAODASSOCIATIONS_TRACKPARTICLECLUSTERASSOCIATION_H
