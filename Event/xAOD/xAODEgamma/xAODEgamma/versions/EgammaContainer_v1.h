// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaContainer_v1.h 581559 2014-02-05 10:54:05Z krasznaa $
#ifndef XAODEGAMMA_VERSIONS_EGAMMACONTAINER_V1_H
#define XAODEGAMMA_VERSIONS_EGAMMACONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODEgamma/versions/Egamma_v1.h"

// Declare IParticle as a base class of Egamma_v1:
DATAVECTOR_BASE( xAOD::Egamma_v1, xAOD::IParticle );

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::Egamma_v1 > EgammaContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::EgammaContainer_v1 , 1087548811 , 1 )
#endif // XAOD_STANDALONE

#endif // XAODEGAMMA_VERSIONS_EGAMMACONTAINER_V1_H
