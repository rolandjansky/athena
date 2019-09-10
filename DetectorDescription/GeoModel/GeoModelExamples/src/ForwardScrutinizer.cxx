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


#if defined BUILDVP1LIGHT
	int ForwardScrutinizer::identify() const
	{
	  return 0;
	}
#else
	Identifier ForwardScrutinizer::identify() const
	{
	  return Identifier();
	}
#endif
