/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LArG4HECOptions_h_
#define _LArG4HECOptions_h_
#include "CLHEP/Units/SystemOfUnits.h"

// ----------------------------------------------------------------------//
//                                                                       //
// This class defines options for the HEC part of the Liquid Argon       //
// simulation. A constructor sets the default values.  This class is     //
// supposed to be exported to python, configured through python commands //
// then stored in storegate and ultimately writted to the data stream.   //
// Similar structures exist for the other subcomponents.                 //
//                                                                       //
//-----------------------------------------------------------------------//

class LArG4HECOptions {

 public:
  // A constructor to insure that the data starts up with a reasonable set
  // of defaults:

  inline LArG4HECOptions() : 
    m_HECBirksLaw    (false),
    m_HECBirksk      (0.0486),
    m_HECHVEnable    (false) {}

  void saveMe();
  void printMe();

  // Set methods
  inline void HECBirksLaw(bool value)   { m_HECBirksLaw = value; }  
  inline void HECBirksk(double value)   { m_HECBirksk = value; }  
  inline void HECHVEnable(bool value)   { m_HECHVEnable = value; }  

  // Get methods
  inline bool   HECBirksLaw(void) const      { return m_HECBirksLaw; }
  inline double HECBirksk(void) const        { return m_HECBirksk; }
  inline bool   HECHVEnable(void) const      { return m_HECHVEnable; }

 private:

  // Birks' law  
  bool m_HECBirksLaw;

  // Birks' law, constant k
  double m_HECBirksk;
  
  // HV Imperfections:
  bool m_HECHVEnable;

};

#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LArG4HECOptions, 207610189, 1)

#endif
