/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LArG4BarrelOptions_h_
#define _LArG4BarrelOptions_h_
#include "CLHEP/Units/SystemOfUnits.h"

// ----------------------------------------------------------------------//
//                                                                       //
// This class defines options for the EMB part of the Liquid Argon       //
// simulation. A constructor sets the default values.  This class is     //
// supposed to be exported to python, configured through python commands //
// then stored in storegate and ultimately writted to the data stream.   //
// Similar structures exist for the other subcomponents.                 //
//                                                                       //
//-----------------------------------------------------------------------//

class LArG4BarrelOptions {

 public:
  // A constructor to insure that the data starts up with a reasonable set
  // of defaults:

  inline LArG4BarrelOptions() : 
    m_EMBCurr        (true),
    m_EMBEtaTrans    (true),
    m_EMBXtalk       (true),
    m_EMBTimeCurr    (false),
    m_EMBTimeShower  (false),
    m_EMBTimeProp    (false),
    m_EMBdstep       (0.2*CLHEP::mm),
    m_EMBBirksLaw    (false),
    m_EMBBirksk      (0.0486),
    m_EMBHVEnable    (false) {}

  void saveMe();
  void printMe();

  // Set methods
  inline void EMBCurr(bool value)       { m_EMBCurr = value; }
  inline void EMBEtaTrans(bool value)   { m_EMBEtaTrans = value; }
  inline void EMBXtalk(bool value)      { m_EMBXtalk = value; }
  inline void EMBTimeCurr(bool value)   { m_EMBTimeCurr = value; }
  inline void EMBTimeShower(bool value) { m_EMBTimeShower = value; }
  inline void EMBTimeProp(bool value)   { m_EMBTimeProp = value; }
  inline void EMBdstep(double value)    { m_EMBdstep = value; }
  inline void EMBBirksLaw(bool value)   { m_EMBBirksLaw = value; }  
  inline void EMBBirksk(double value)   { m_EMBBirksk = value; }  
  inline void EMBHVEnable(bool value)   { m_EMBHVEnable = value; }  

  // Get methods
  inline bool   EMBCurr(void) const          { return m_EMBCurr; }
  inline bool   EMBEtaTrans(void) const      { return m_EMBEtaTrans; }
  inline bool   EMBXtalk(void) const         { return m_EMBXtalk; }
  inline bool   EMBTimeCurr(void) const      { return m_EMBTimeCurr; }
  inline bool   EMBTimeShower(void) const    { return m_EMBTimeShower; }
  inline bool   EMBTimeProp(void) const      { return m_EMBTimeProp; }
  inline double EMBdstep(void) const         { return m_EMBdstep; }
  inline bool   EMBBirksLaw(void) const      { return m_EMBBirksLaw; }
  inline double EMBBirksk(void) const        { return m_EMBBirksk; }
  inline bool   EMBHVEnable(void) const      { return m_EMBHVEnable; }

 private:
  // Switch for energy-current computation.
  bool m_EMBCurr;

  // Switch for transition effect at eta=0.8
  bool m_EMBEtaTrans;

  // Switch for crosstalk effect in the strips:
  bool m_EMBXtalk;
  
  // Switch for effect of current maps on time
  bool m_EMBTimeCurr;
 
  // Switch to simulation effect of shower propagation with a cell, on time.
  bool m_EMBTimeShower;

  // Switch to simulate the effect of signal propagation in the electrode, on time. 
  bool m_EMBTimeProp;

  // Value of the substep length in the current simulation
  double m_EMBdstep;

  // Birks' law  
  bool m_EMBBirksLaw;

  // Birks' law, constant k
  double m_EMBBirksk;

  // Enable HV imperfections in simulation
  bool m_EMBHVEnable;

};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LArG4BarrelOptions, 321345322, 1)
#endif

#endif
