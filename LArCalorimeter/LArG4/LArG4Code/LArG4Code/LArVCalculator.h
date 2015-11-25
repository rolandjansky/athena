/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArVCalculator.hh 
// Prepared 27-Jun-2001 Bill Seligman

// This is an abstract base for a geometry calculator for ATLAS EM
// detectors.

// A "calculator" is used in much the same way as a hand-held
// calculator might be.  The user supplies a value and hits 'Enter'
// (i.e., invokes the Process() method).  Then they read off whatever
// values are of interest.

// 12-Jul-2002 WGS: Added access method to return an identifier.  This
// replaces any other access methods for hit ID information.

// 09-Jan-2002 WGS: Major modification -- The main calculation method
// FindCell(G4ThreeVector&) has been changed to Process(G4Step*).  The
// reason for this change is that it became apparent the calculators
// needed more information than just the point of the energy deposit.

// Also: since this class may also be used by the HEC, changed its
// named from LArVEMCalculator to LArVCalculator.

#ifndef __LArVCalculator_H__
#define __LArVCalculator_H__

#include "globals.hh"

#include "LArG4Code/LArG4Identifier.h"

// Forward declaractions:
class G4Step;

// struct to pass the hit info
struct LArHitData { LArG4Identifier id; G4double time; G4double energy; };

class LArVCalculator {
public:

  virtual ~LArVCalculator() {};

  // "OOTcut" the time cut (ns) after which an energy deposit is
  // considered to be out-of-time w.r.t. the event.
  virtual G4float OOTcut() const = 0;

  // 29-Mar-2002 WGS: The Process method now returns a boolean value.
  // If it's true, the hit can be used by Geant4; if it's false,
  // there's something wrong with the hit and it should be ignored.
  virtual G4bool Process (const G4Step*) = 0;

  //New interface, to pass the hit info directly
  virtual G4bool Process (const G4Step*, std::vector<LArHitData>&) = 0;

  // Override if your calculator will return multiple hits:
  virtual int getNumHits() const {return 1;}

  // The cell identifier determined by the Process method.
  virtual const LArG4Identifier& identifier(int i=0) const = 0;

  // Routines added 09-Jan-2002 WGS: For some detectors, the
  // determination of the time, energy deposition, or whether the hit
  // was out-of-time is non-trivial, so these new methods were added.
  virtual G4double time(int i=0) const = 0;    // units = ns
  virtual G4double energy(int i=0) const = 0;  // units = native G4 unit of energy
  virtual G4bool isInTime(int i=0) const = 0;
  virtual G4bool isOutOfTime(int i=0) const = 0;


};

#endif // __LArVCalculator_H__
