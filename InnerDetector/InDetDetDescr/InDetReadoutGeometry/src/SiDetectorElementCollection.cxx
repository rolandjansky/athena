/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "Identifier/IdentifierHash.h"

InDetDD::SiDetectorElementCollection::~SiDetectorElementCollection() {
  for (InDetDD::SiDetectorElement* ele: *this) delete ele;
}

const InDetDD::SiDetectorElement*
InDetDD::SiDetectorElementCollection::getDetectorElement(const IdentifierHash& hash) const {
  const unsigned int value{hash.value()};
  if (this->size()<=value) return nullptr;
  return this->at(value);  
}
