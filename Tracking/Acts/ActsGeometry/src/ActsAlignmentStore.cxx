/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsDetectorElement.h"
#include "ActsGeometry/ActsAlignmentStore.h"
#include "Acts/Definitions/Algebra.hpp"

ActsAlignmentStore::ActsAlignmentStore(const GeoAlignmentStore &gas){

  m_deltas = gas.getDeltas();
  m_absPositions = gas.getAbsPositions();
  m_defAbsPositions = gas.getDefAbsPositions();

}

void ActsAlignmentStore::setTransform(const ActsDetectorElement *ade,
                                      const Acts::Transform3 &xf) {
  if (!m_transforms.setTransform(ade, xf)) {
    throw ExcAlignmentStore(
        "Attempted to overwrite Delta in the Alignment Store");
  }
}

const Acts::Transform3 *
ActsAlignmentStore::getTransform(const ActsDetectorElement *ade) const {
  return m_transforms.getTransform(ade);
}

void ActsAlignmentStore::append(const GeoAlignmentStore& gas) {
  for(const auto& it : gas.getDeltas().container()) {
    setDelta(it.first, it.second);
  }

  for(const auto& it : gas.getAbsPositions().container()) {
    setAbsPosition(it.first, it.second);
  }

  for(const auto& it : gas.getDefAbsPositions().container()) {
    setDefAbsPosition(it.first, it.second);
  }
}
