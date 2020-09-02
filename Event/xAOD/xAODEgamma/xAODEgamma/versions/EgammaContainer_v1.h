// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaContainer_v1.h 644948 2015-02-07 01:33:06Z christos $
#ifndef XAODEGAMMA_VERSIONS_EGAMMACONTAINER_V1_H
#define XAODEGAMMA_VERSIONS_EGAMMACONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"
// Local include(s):
#include "xAODEgamma/versions/Egamma_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::Egamma_v1 > EgammaContainer_v1;
}


#endif // XAODEGAMMA_VERSIONS_EGAMMACONTAINER_V1_H
