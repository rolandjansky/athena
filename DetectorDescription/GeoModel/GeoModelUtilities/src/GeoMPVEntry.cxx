/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoMPVEntry.h"
#include <iostream>

bool GeoMPVEntry::operator ==(const GeoMPVEntry &right) const  
{
  if (thePhotonMomentum == right.thePhotonMomentum) 
    return true;
  else
    return false; 
}

bool GeoMPVEntry::operator <(const GeoMPVEntry &right) const  
{
  if (thePhotonMomentum < right.thePhotonMomentum) 
    return true;
  else
    return false;
}

GeoMPVEntry& GeoMPVEntry::operator =(const GeoMPVEntry& right)
{
  if (this == &right) return *this;
  
  thePhotonMomentum = right.thePhotonMomentum;
  theProperty = right.theProperty;
  return *this;
}

GeoMPVEntry::GeoMPVEntry(double aPhotonMomentum, double aProperty)
{
  thePhotonMomentum = aPhotonMomentum;
  theProperty = aProperty;
}

GeoMPVEntry::GeoMPVEntry(const GeoMPVEntry &right)
{
  thePhotonMomentum = right.thePhotonMomentum;
  theProperty = right.theProperty;
}

GeoMPVEntry::~GeoMPVEntry(){}

void GeoMPVEntry::DumpEntry()
{
  std::cout << "(" 
	    << thePhotonMomentum 
	    << ", " 
	    << theProperty
	    << ")\n"; 
}
