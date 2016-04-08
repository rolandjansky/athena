/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_SCTRAWHITVALIDATIONCONTAINER_H
#define XAODTRACKING_SCTRAWHITVALIDATIONCONTAINER_H
 
// Local include(s):
#include "xAODTracking/SCTRawHitValidation.h"
#include "xAODTracking/versions/SCTRawHitValidationContainer_v1.h"
 
namespace xAOD {
    /// Definition of the current "SCTRawHitValidationContainer version"
    typedef SCTRawHitValidationContainer_v1 SCTRawHitValidationContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF(xAOD::SCTRawHitValidationContainer, 1314367244, 1)
 
#endif // XAODTRACKING_SCTRAWHITVALIDATIONCONTAINER_H
