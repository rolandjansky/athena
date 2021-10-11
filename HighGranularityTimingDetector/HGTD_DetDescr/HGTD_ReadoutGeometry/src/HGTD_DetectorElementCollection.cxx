/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_ReadoutGeometry/HGTD_DetectorElementCollection.h"

#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"
#include "Identifier/IdentifierHash.h"

InDetDD::HGTD_DetectorElementCollection::~HGTD_DetectorElementCollection() {
  for (InDetDD::HGTD_DetectorElement* ele: *this) delete ele;
}

const InDetDD::HGTD_DetectorElement*
InDetDD::HGTD_DetectorElementCollection::getDetectorElement(const IdentifierHash& hash) const {
  const unsigned int value{hash.value()};
  if (this->size()<=value) return nullptr;
  return this->at(value);  
}
