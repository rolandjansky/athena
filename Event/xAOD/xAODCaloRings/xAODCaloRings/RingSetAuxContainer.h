/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetAuxContainer.h 659538 2015-04-08 17:29:18Z wsfreund $ 
#ifndef XAODCALORINGS_RINGSETAUXCONTAINER_H
#define XAODCALORINGS_RINGSETAUXCONTAINER_H

// Local include(s):
#include "xAODCaloRings/versions/RingSetAuxContainer_v1.h"

namespace xAOD {
/// Definition of the current RingSet auxiliary container
///
/// All reconstruction code should attach the typedefed auxiliary
/// container to the xAOD::RingSetContainer, so it will be easy to change
/// the container type as we get new I/O technologies for these
/// objects.
///
typedef RingSetAuxContainer_v1 RingSetAuxContainer;
} // namespace xAOD

#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::RingSetAuxContainer , 1229579132 , 1 )
#endif

#endif // XAODCALORINGS_RINGSETAUXCONTAINER_H
