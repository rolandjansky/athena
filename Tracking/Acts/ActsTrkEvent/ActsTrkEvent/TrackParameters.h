/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKEVENT_TRACKPARAMETERS_H
#define ACTSTRKEVENT_TRACKPARAMETERS_H 1

#include "Acts/EventData/TrackParameters.hpp"
#include "AthContainers/DataVector.h"

namespace ActsTrk {
  typedef DataVector< Acts::BoundTrackParameters > BoundTrackParametersContainer;
}

// Set up a CLID for the type:
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( Acts::BoundTrackParameters, 207122290, 1 )
CLASS_DEF( ActsTrk::BoundTrackParametersContainer, 1261498502, 1)

#endif
