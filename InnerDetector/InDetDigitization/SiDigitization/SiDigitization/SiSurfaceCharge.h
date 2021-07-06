/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiSurfaceCharge.h
//   Header file for class SiSurfaceCharge
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class which contains a charge located at the sensor surface
///////////////////////////////////////////////////////////////////
// Version 1.0 09/02/2001 David Calvet
///////////////////////////////////////////////////////////////////

#ifndef SIDIGITIZATION_SISURFACECHARGE_H
#define SIDIGITIZATION_SISURFACECHARGE_H

#include "ReadoutGeometryBase/SiLocalPosition.h"
#include "InDetSimEvent/SiCharge.h"


class SiSurfaceCharge {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Copy constructor:
  SiSurfaceCharge(const SiSurfaceCharge &surfaceCharge);

  // Constructor with parameters:
  //   position at the sensor surface
  //   charge located at this position
  SiSurfaceCharge(const InDetDD::SiLocalPosition &position,const SiCharge &charge);

  // Destructor:
  ~SiSurfaceCharge();

  // Assignment operator:
  SiSurfaceCharge &operator=(const SiSurfaceCharge &surfaceCharge);

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // position at the sensor surface:
  InDetDD::SiLocalPosition position() const;

  // charge located at this position:
  SiCharge charge() const;

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  SiSurfaceCharge();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  InDetDD::SiLocalPosition m_position; // position at the sensor surface
  SiCharge m_charge; // charge located at this position
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline SiSurfaceCharge::~SiSurfaceCharge() 
{}

inline InDetDD::SiLocalPosition SiSurfaceCharge::position() const
{
  return m_position;
}

inline SiCharge SiSurfaceCharge::charge() const
{
  return m_charge;
}

#endif // SIDIGITIZATION_SISURFACECHARGE_H
