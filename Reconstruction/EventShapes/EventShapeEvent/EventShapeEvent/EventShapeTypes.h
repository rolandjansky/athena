/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPES_EVENTSHAPETYPES_H
#define EVENTSHAPES_EVENTSHAPETYPES_H

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

namespace EventShapeTypes
{
  typedef INavigable4Momentum              es_fourvector_t;
  typedef INavigable4MomentumCollection    es_4vect_vect_t;
} 

#endif
