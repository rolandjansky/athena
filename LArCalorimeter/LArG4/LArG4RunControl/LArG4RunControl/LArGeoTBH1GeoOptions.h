/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LArGeoTBH1GeoOptions_h_
#define _LArGeoTBH1GeoOptions_h_
#include "CLHEP/Units/SystemOfUnits.h"

// ----------------------------------------------------------------------//
//                                                                       //
// This class defines options for the Positioning of Test Beam components//
// in test beam simulation.  It sets the nickname and nicknumber of      //
// a row in the LArTBPositions table in Oracle (or replica).  It is      //
// originally used in the LAr H6 2003 TB but is probably applicable and  //
// going to be used in other test beam situations.                       //
//                                                                       //
//-----------------------------------------------------------------------//

class LArGeoTBH1GeoOptions {

 public:
  // A constructor to insure that the data starts up with a reasonable set
  // of defaults:

  inline LArGeoTBH1GeoOptions() : 
    m_CryoXPosition(0.0),
    m_TableYPosition(0.0)
  {}

  void saveMe();
  void printMe();

  // Set methods
  inline void CryoXPosition(double value)			{ m_CryoXPosition    = value; }
  inline void TableYPosition(double value)			{ m_TableYPosition   = value; }

  // Get methods
  inline double CryoXPosition(void) const		      { return m_CryoXPosition; }
  inline double TableYPosition(void) const		      { return m_TableYPosition; }

 private:
  // X position of cryostat
  double m_CryoXPosition;
  // Y position of beamline table
  double m_TableYPosition;
 
};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LArGeoTBH1GeoOptions, 104187856, 1)
#endif

#endif
