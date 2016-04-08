/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoMPVEntry.h"
#include <iostream>

bool GeoMPVEntry::operator ==(const GeoMPVEntry &right) const  
{
  if (m_thePhotonMomentum == right.m_thePhotonMomentum) 
    return true;
  else
    return false; 
}

bool GeoMPVEntry::operator <(const GeoMPVEntry &right) const  
{
  if (m_thePhotonMomentum < right.m_thePhotonMomentum) 
    return true;
  else
    return false;
}

GeoMPVEntry& GeoMPVEntry::operator =(const GeoMPVEntry& right)
{
  if (this == &right) return *this;
  
  m_thePhotonMomentum = right.m_thePhotonMomentum;
  m_theProperty = right.m_theProperty;
  return *this;
}

GeoMPVEntry::GeoMPVEntry(double aPhotonMomentum, double aProperty)
{
  m_thePhotonMomentum = aPhotonMomentum;
  m_theProperty = aProperty;
}

GeoMPVEntry::GeoMPVEntry(const GeoMPVEntry &right)
{
  m_thePhotonMomentum = right.m_thePhotonMomentum;
  m_theProperty = right.m_theProperty;
}

GeoMPVEntry::~GeoMPVEntry(){}

void GeoMPVEntry::DumpEntry()
{
  std::cout << "(" 
	    << m_thePhotonMomentum 
	    << ", " 
	    << m_theProperty
	    << ")\n"; 
}
