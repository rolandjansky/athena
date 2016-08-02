/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODVIEWS_TRACKPARTICLEVIEWVECTOR_H
#define XAODVIEWS_TRACKPARTICLEVIEWVECTOR_H
#include "xAODTracking/TrackParticleContainer.h"
#include "AthContainers/ViewVector.h"
#include "xAODCore/CLASS_DEF.h"
namespace xAOD {
  typedef ViewVector<TrackParticleContainer> TrackParticleViewVector;
} 
VIEWVECTOR_CLASS_DEF(xAOD::TrackParticleViewVector, 86648760)
    
#endif // XAODVIEWS_TRACKPARTICLEVIEWVECTOR_H
