/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDiodesParameters.cxx
//   Implementation file for class SiDiodesParameters
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 14/08/2001 David Calvet
///////////////////////////////////////////////////////////////////

#include "InDetReadoutGeometry/SiDiodesParameters.h"

namespace InDetDD {

// Implicit constructor:
SiDiodesParameters::SiDiodesParameters() :
  m_centre(),
  m_width()
{}

// Copy constructor:
SiDiodesParameters::SiDiodesParameters(const SiDiodesParameters &parameters) :
  m_centre(parameters.m_centre),
  m_width(parameters.m_width)
{}

// Constructor with parameters:
SiDiodesParameters::SiDiodesParameters(const SiLocalPosition &centre,
				       const SiLocalPosition &width) :
  m_centre(centre),
  m_width(width)
{}

// Assignment operator:
SiDiodesParameters &SiDiodesParameters::operator=(const SiDiodesParameters &parameters)
{
  if (this!=&parameters) {
    m_centre=parameters.m_centre;
    m_width=parameters.m_width;
  } else {}
  return *this;
}

} // namespace InDetDD
