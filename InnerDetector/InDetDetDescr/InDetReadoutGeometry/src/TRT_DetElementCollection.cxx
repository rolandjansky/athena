/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_DetElementCollection.h"

#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "Identifier/IdentifierHash.h"

const InDetDD::TRT_BaseElement*
InDetDD::TRT_DetElementCollection::getDetectorElement(IdentifierHash& hash) const {
  unsigned int value{hash.value()};
  if (this->size()<=value) return nullptr;
  return (*this)[value];  
}
