/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSALIGNMENTSTORE_H
#define ACTSGEOMETRY_ACTSALIGNMENTSTORE_H

#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "GeoModelUtilities/TransformMap.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

#include "Acts/Definitions/Algebra.hpp"

#include <stdexcept>

class ActsDetectorElement;

class ActsAlignmentStore : public GeoAlignmentStore
{
  public:
    ActsAlignmentStore() {}
    ActsAlignmentStore(const GeoAlignmentStore& gas);

    void setTransform(const ActsDetectorElement* key, const Acts::Transform3&);
    const Acts::Transform3* getTransform(const ActsDetectorElement* key) const;

    void append(const GeoAlignmentStore& gas);

  private:
    TransformMap<ActsDetectorElement, Acts::Transform3> m_transforms;
};

CLASS_DEF(ActsAlignmentStore, 58496671, 1)
CONDCONT_DEF( ActsAlignmentStore , 44989665 );

#endif
