/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFParticleOrderedQueue.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
#ifndef ISF_EVENT_ISFPARTICLEORDEREDQUEUE_H
#define ISF_EVENT_ISFPARTICLEORDEREDQUEUE_H

// STL includes
#include <queue>

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleVector.h"

/** ISFParticleOrderedQueue */

namespace ISF {
  /** sort function according to particle order */
  struct ISFParticleOrdering {
    bool operator()(ISF::ISFParticle *lhs, ISF::ISFParticle *rhs) { return lhs->getOrder() < rhs->getOrder(); }
  };

  /** the actual particle priority_queue */
  typedef std::priority_queue<ISF::ISFParticle *, ISF::ISFParticleVector, ISF::ISFParticleOrdering >  ISFParticleOrderedQueue;
}

#endif // ISF_EVENT_ISFPARTICLEORDEREDQUEUE_H

