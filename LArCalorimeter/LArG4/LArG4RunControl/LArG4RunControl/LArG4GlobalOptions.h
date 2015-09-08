/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LArG4GlobalOptions_h_
#define _LArG4GlobalOptions_h_
#include "CLHEP/Units/SystemOfUnits.h"

// ----------------------------------------------------------------------//
//                                                                       //
// This class defines options for the Global part of the Liquid Argon    //
// simulation. A constructor sets the default values.  This class is     //
// supposed to be exported to python, configured through python commands //
// then stored in storegate and ultimately writted to the data stream.   //
// Similar structures exist for the other subcomponents.                 //
//                                                                       //
//-----------------------------------------------------------------------//

#include <string>

class LArG4GlobalOptions {

 public:
  // A constructor to insure that the data starts up with a reasonable set
  // of defaults:

  inline LArG4GlobalOptions() : 
    m_OutOfTimeCut   (300*CLHEP::ns) {}

  void saveMe();
  void printMe();

  // Set methods
  inline void OutOfTimeCut(double value)     { m_OutOfTimeCut = value; }

  // Get methods
  inline double OutOfTimeCut(void) const     { return m_OutOfTimeCut; }

 private:
  // Value of the substep length in the current simulation
  double m_OutOfTimeCut;

};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LArG4GlobalOptions, 321345321, 1)
#endif

#endif
