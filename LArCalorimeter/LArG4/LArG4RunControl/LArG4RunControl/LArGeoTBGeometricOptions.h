/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LArGeoTBGeometricOptions_h_
#define _LArGeoTBGeometricOptions_h_
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

class LArGeoTBGeometricOptions {

 public:
  // A constructor to insure that the data starts up with a reasonable set
  // of defaults:

  inline LArGeoTBGeometricOptions() : 
    m_CryoEtaPosition(0.),
    m_CryoPhiPosition(0.)
    {}
  
  void saveMe();
  void printMe();

  // Set methods
  inline void CryoEtaPosition(double value)			{ m_CryoEtaPosition    = value; }
  inline void CryoPhiPosition(double value)			{ m_CryoPhiPosition    = value; }

  // Get methods
  inline double CryoEtaPosition(void) const		        { return m_CryoEtaPosition; }
  inline double CryoPhiPosition(void) const		        { return m_CryoPhiPosition; }

 private:

  // Eta position of cryostat
  double m_CryoEtaPosition;

  // Phi position of cryostat
  double m_CryoPhiPosition;

};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LArGeoTBGeometricOptions, 205400553, 1)
#endif

#endif
