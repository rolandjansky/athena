/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingsContainer_v1.h 707323 2015-11-12 02:45:01Z wsfreund $
#ifndef XAODCALORINGS_VERSIONS_CALORINGSCONTAINER_V1_H
#define XAODCALORINGS_VERSIONS_CALORINGSCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODCaloRings/versions/CaloRings_v1.h"

namespace xAOD {
/// The container is a simple typedef for now
typedef DataVector< xAOD::CaloRings_v1 > CaloRingsContainer_v1;
/// Define EL vector for this version:
typedef std::vector< ElementLink< CaloRingsContainer_v1 > >
  CaloRingsLinks_v1;
}

#endif // XAODCALORINGS_VERSIONS_CALORINGSCONTAINER_V1_H

