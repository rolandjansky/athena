/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetConfAuxContainer.h 659538 2015-04-08 17:29:18Z wsfreund $ 
#ifndef XAODCALORINGS_RINGSETCONFAUXCONTAINER_H
#define XAODCALORINGS_RINGSETCONFAUXCONTAINER_H

// Local include(s):
#include "xAODCaloRings/versions/RingSetConfAuxContainer_v1.h"

namespace xAOD {
/// Definition of the current RingSetConf auxiliary container
///
/// All reconstruction code should attach the typedefed auxiliary
/// container to the xAOD::RingSetConfContainer, so it will be easy to change
/// the container type as we get new I/O technologies for these
/// objects.
///
typedef RingSetConfAuxContainer_v1 RingSetConfAuxContainer;
} // namespace xAOD

#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::RingSetConfAuxContainer , 1307745126 , 1 )
#endif

#endif // XAODCALORINGS_RINGSETCONFAUXCONTAINER_H
