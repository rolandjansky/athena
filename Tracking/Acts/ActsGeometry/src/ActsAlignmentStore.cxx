/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsAlignmentStore.h"

#include "ActsGeometry/ActsDetectorElement.h"

#include "Acts/Utilities/Definitions.hpp"


void
ActsAlignmentStore::setTransform(const ActsDetectorElement* ade, const Acts::Transform3D& xf)
{
  if(!m_transforms.setTransform(ade, xf)) {
    throw ExcAlignmentStore("Attempted to overwrite Delta in the Alignment Store");    
  }
}

const Acts::Transform3D*
ActsAlignmentStore::getTransform(const ActsDetectorElement* ade) const
{
  return m_transforms.getTransform(ade);
}
