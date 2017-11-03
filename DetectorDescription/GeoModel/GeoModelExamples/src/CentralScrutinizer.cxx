/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "CentralScrutinizer.h"

CentralScrutinizer::CentralScrutinizer(const GeoVFullPhysVol *fullPhysVol)
  :GeoVDetectorElement(fullPhysVol)
{
}

CentralScrutinizer::~CentralScrutinizer()
{
}

Identifier CentralScrutinizer::identify() const
{
  return Identifier();
}

