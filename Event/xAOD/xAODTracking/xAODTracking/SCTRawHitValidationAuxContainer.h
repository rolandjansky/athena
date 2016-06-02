/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_SCTRAWHITVALIDATIONAUXCONTAINER_H
#define XAODTRACKING_SCTRAWHITVALIDATIONAUXCONTAINER_H
 
#include "xAODTracking/versions/SCTRawHitValidationAuxContainer_v1.h"
 
namespace xAOD {
  typedef SCTRawHitValidationAuxContainer_v1 SCTRawHitValidationAuxContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF(xAOD::SCTRawHitValidationAuxContainer, 1171743595, 1)

#endif
