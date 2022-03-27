/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_ReadoutGeometry/HGTD_DetectorElementCollection.h"

#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"
#include "Identifier/IdentifierHash.h"

const InDetDD::HGTD_DetectorElement*
InDetDD::HGTD_DetectorElementCollection::getDetectorElement(const IdentifierHash& hash) const {
  const unsigned int value{hash.value()};
  if (this->size()<=value) return nullptr;
  return this->at(value);
}
