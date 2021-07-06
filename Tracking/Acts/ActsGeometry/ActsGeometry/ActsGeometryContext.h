/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSGEOMETRYCONTEXT_H
#define ACTSGEOMETRY_ACTSGEOMETRYCONTEXT_H

#include "ActsGeometry/ActsAlignmentStore.h"

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

#include "Acts/Geometry/GeometryContext.hpp"

#include <memory>

class ActsGeometryContext {
public:
  bool construction{false};

  std::unique_ptr<const ActsAlignmentStore> ownedAlignmentStore{nullptr};

  const ActsAlignmentStore* alignmentStore{nullptr};

  Acts::GeometryContext
  context() const
  {
    return Acts::GeometryContext(this);
  }
};

CLASS_DEF(ActsGeometryContext, 51464195, 1)
CONDCONT_DEF( ActsGeometryContext , 11228079 );

#endif
