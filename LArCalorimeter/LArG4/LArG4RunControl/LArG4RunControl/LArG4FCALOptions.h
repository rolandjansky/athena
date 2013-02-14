/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LArG4FCALOptions_h_
#define _LArG4FCALOptions_h_
#include "CLHEP/Units/SystemOfUnits.h"

// ----------------------------------------------------------------------//
//                                                                       //
// This class defines options for the FCAL part of the Liquid Argon       //
// simulation. A constructor sets the default values.  This class is     //
// supposed to be exported to python, configured through python commands //
// then stored in storegate and ultimately writted to the data stream.   //
// Similar structures exist for the other subcomponents.                 //
//                                                                       //
//-----------------------------------------------------------------------//

class LArG4FCALOptions {

 public:
  // A constructor to insure that the data starts up with a reasonable set
  // of defaults:

  inline LArG4FCALOptions() : 
    m_FCALBirksLaw    (false),
    m_FCALBirksk      (0.0486), 
    m_FCALHVEnable(false){}

  void saveMe();
  void printMe();

  // Set methods
  inline void FCALBirksLaw(bool value)   { m_FCALBirksLaw = value; }  
  inline void FCALBirksk(double value)   { m_FCALBirksk = value; }  
  inline void FCALEnableHV(bool value)   { m_FCALHVEnable = value; }
  // Get methods
  inline bool   FCALBirksLaw(void)  const { return m_FCALBirksLaw; }
  inline double FCALBirksk(void)    const { return m_FCALBirksk; }
  inline bool   FCALHVEnable(void ) const { return m_FCALHVEnable; }
 private:

  // Birks' law  
  bool m_FCALBirksLaw;

  // Birks' law, constant k
  double m_FCALBirksk;

  // Enable HV
  bool m_FCALHVEnable;

};

#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LArG4FCALOptions, 190061068, 1)

#endif
