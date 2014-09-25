// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CompositeParticleContainer_v1.h 615672 2014-09-08 20:16:15Z kkoeneke $
#ifndef XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLECONTAINER_V1_H
#define XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLECONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODParticleEvent/versions/CompositeParticle_v1.h"
#include "xAODParticleEvent/CompositeParticleContainerFwd.h"

// To complete the DATAVECTOR_BASE macro:
template struct DataVector_detail::DVLEltBaseInit< xAOD::CompositeParticle_v1 >;

namespace xAOD {
   /// Define the composite particle container as a simple DataVector
   typedef DataVector< CompositeParticle_v1 > CompositeParticleContainer_v1;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CompositeParticleContainer_v1, 1213067199, 1 )

#endif // XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLECONTAINER_V1_H
