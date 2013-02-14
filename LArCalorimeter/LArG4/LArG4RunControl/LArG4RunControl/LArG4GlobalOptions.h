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
    m_OutOfTimeCut   (300*CLHEP::ns),
    m_TimeBinType   ("Default"),
    m_TimeBinWidth   (2.5*CLHEP::ns),
    m_doCalibHitParticleID(false),
    m_allowHitModification(false) {}

  void saveMe();
  void printMe();

  // Set methods
  inline void OutOfTimeCut(double value)     { m_OutOfTimeCut = value; }
  inline void TimeBinType(std::string value) { m_TimeBinType = value; }
  inline void TimeBinWidth(double value)     { m_TimeBinWidth = value; }
  inline void SetDoCalibHitParticleID(bool doParticleID) { m_doCalibHitParticleID=doParticleID; }
  inline void AllowHitModification(bool allowHitModification) { m_allowHitModification=allowHitModification; }

  // Get methods
  inline double OutOfTimeCut(void) const     { return m_OutOfTimeCut; }
  inline std::string TimeBinType(void) const { return m_TimeBinType; }
  inline double TimeBinWidth(void) const     { return m_TimeBinWidth; }
  bool   GetDoCalibHitParticleID() const { return m_doCalibHitParticleID;}
  inline bool AllowHitModification(void) const { return m_allowHitModification; }

 private:
  // Value of the substep length in the current simulation
  double m_OutOfTimeCut;


  // -- removing  double m_TimeBinWidth;
  // Two types of LAr hit time binning
  // 1. 'Default'
  //
  //     All negative times to the bin 0
  //     0 <= time < 10 by 2.5ns
  //     10 <= time < 50 by 10ns
  //     50 <= time < 100 by 25ns
  //     All others to the bin 14
  //
  //
  // 2. 'Uniform'
  // 
  // Old style time binning by 2.5ns
  std::string m_TimeBinType;

  // Width of the time bins in the uniform binning
  double m_TimeBinWidth;

  // Flag to produce calibration hits signed with primary particle ID
  bool   m_doCalibHitParticleID;

  // Flag to write hit collections to StoreGate as non-const to allow modifications by various simulation algorithms
  bool   m_allowHitModification;
};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LArG4GlobalOptions, 321345321, 1)
#endif

#endif
