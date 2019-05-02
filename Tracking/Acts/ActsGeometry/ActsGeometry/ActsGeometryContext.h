/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSGEOMETRYCONTEXT_H
#define ACTSGEOMETRY_ACTSGEOMETRYCONTEXT_H

#include "ActsGeometry/ActsAlignmentStore.h"

struct ActsGeometryContext {

  bool nominal{true};

  const ActsAlignmentStore* alignmentStore{nullptr};

};


#endif
