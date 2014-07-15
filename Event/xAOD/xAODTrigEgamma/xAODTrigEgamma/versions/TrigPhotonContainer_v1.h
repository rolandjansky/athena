// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhotonContainer_v1.h 602892 2014-06-20 12:23:53Z krasznaa $
#ifndef XAODTRIGEGAMMA_VERSIONS_TRIGPHOTONCONTAINER_V1_H
#define XAODTRIGEGAMMA_VERSIONS_TRIGPHOTONCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigEgamma/versions/TrigPhoton_v1.h"

// Declare IParticle as a base class of TrigPhoton_v1:
DATAVECTOR_BASE( xAOD::TrigPhoton_v1, xAOD::IParticle );

namespace xAOD {
   /// Declare the container as a simple type definition
   typedef DataVector< xAOD::TrigPhoton_v1 > TrigPhotonContainer_v1;
}

// Setup a CLID for the container...
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigPhotonContainer_v1, 1141180511, 1 )
#endif // not XAOD_STANDALONE

#endif // not XAODTRIGEGAMMA_VERSIONS_TRIGPHOTONCONTAINER_V1_H
