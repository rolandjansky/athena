/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoVDetectorElement.h"

GeoVDetectorElement::GeoVDetectorElement (const GeoVFullPhysVol* fullPhysVol)
  : m_materialGeom(fullPhysVol)
{
}

GeoVDetectorElement::~GeoVDetectorElement()
{
}
