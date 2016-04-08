/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingsAuxContainer.h 659538 2015-04-08 17:29:18Z wsfreund $ 
#ifndef XAODCALORINGS_CALORINGSAUXCONTAINER_H
#define XAODCALORINGS_CALORINGSAUXCONTAINER_H

// Local include(s):
#include "xAODCaloRings/versions/CaloRingsAuxContainer_v1.h"

namespace xAOD {
/// Definition of the current CaloRings auxiliary container
///
/// All reconstruction code should attach the typedefed auxiliary
/// container to the xAOD::CaloRingsContainer, so it will be easy to change
/// the container type as we get new I/O technologies for these
/// objects.
///
typedef CaloRingsAuxContainer_v1 CaloRingsAuxContainer;
} // namespace xAOD

#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloRingsAuxContainer , 1133975188 , 1 )
#endif

#endif // XAODCALORINGS_CALORINGSAUXCONTAINER_H
