// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthParticleContainer.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_TRUTHPARTICLECONTAINER_H
#define XAODTRUTH_TRUTHPARTICLECONTAINER_H

// Local include(s):
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/versions/TruthParticleContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the truth particle container
   typedef TruthParticleContainer_v1 TruthParticleContainer;
}

// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthParticleContainer, 1237340765, 1 )

#endif // XAODTRUTH_TRUTHPARTICLECONTAINER_H
