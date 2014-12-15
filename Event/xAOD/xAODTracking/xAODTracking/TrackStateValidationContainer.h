/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_TRACKSTATEVALIDATIONCONTAINER_H
#define XAODTRACKING_TRACKSTATEVALIDATIONCONTAINER_H

 
// Local include(s):
#include "xAODTracking/TrackStateValidation.h"
#include "xAODTracking/versions/TrackStateValidationContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current "TrackStateValidation container version"
   typedef TrackStateValidationContainer_v1 TrackStateValidationContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackStateValidationContainer , 1294535933 , 1 )


#endif // XAODTRACKING_TRACKSTATEVALIDATIONCONTAINER_H
