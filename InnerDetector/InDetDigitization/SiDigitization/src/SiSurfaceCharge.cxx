/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiSurfaceCharge.cxx
//   Implementation file for class SiSurfaceCharge
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 01/02/2001 David Calvet
///////////////////////////////////////////////////////////////////

#include "SiDigitization/SiSurfaceCharge.h"
using namespace InDetDD;
// Copy constructor:
SiSurfaceCharge::SiSurfaceCharge(const SiSurfaceCharge &surfaceCharge) :
  m_position(surfaceCharge.m_position),
  m_charge(surfaceCharge.m_charge)
{}

// Constructor with parameters:
SiSurfaceCharge::SiSurfaceCharge(const SiLocalPosition &position,
				 const SiCharge &charge) :
  m_position(position),
  m_charge(charge)
{}

// Assignment operator:
SiSurfaceCharge &SiSurfaceCharge::operator=(const SiSurfaceCharge &surfaceCharge)
{
  if (this!=&surfaceCharge) {
    m_position=surfaceCharge.m_position;
    m_charge=surfaceCharge.m_charge;
  } else {}
  return *this;
}

