/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "ForwardScrutinizer.h"

ForwardScrutinizer::ForwardScrutinizer(const GeoVFullPhysVol *fullPhysVol)
  :GeoVDetectorElement(fullPhysVol)
{
}

ForwardScrutinizer::~ForwardScrutinizer()
{
}

Identifier ForwardScrutinizer::identify() const
{
  return Identifier();
}
