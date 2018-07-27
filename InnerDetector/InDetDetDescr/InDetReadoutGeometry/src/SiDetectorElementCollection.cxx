/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "Identifier/IdentifierHash.h"

const InDetDD::SiDetectorElement*
InDetDD::SiDetectorElementCollection::getDetectorElement(const IdentifierHash& hash) const {
  const unsigned int value{hash.value()};
  if (this->size()<=value) return nullptr;
  return this->at(value);  
}
