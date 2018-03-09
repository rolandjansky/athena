/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H62004CalibSD_H__
#define __LArG4H62004CalibSD_H__

#include "LArG4Code/LArG4CalibSD.h"
#include "LArG4Code/LArG4Identifier.h"

#include <set>
#include <vector>

// Forward declarations.
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class LArG4H62004CalibSD : public LArG4CalibSD
{
public:
  LArG4H62004CalibSD(G4String name,ILArCalibCalculatorSvc* calc,bool doPID=false);
  virtual ~LArG4H62004CalibSD() {}

  // The required functions for all sensitive detectors:
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* ROhist);

  // The calibration sensitive detector can receive a message that
  // is outside Geant4's normal hit processing: for a particular
  // G4Step, get the identifier and deposite a specified amount of
  // energy.
  virtual G4bool SpecialHit(G4Step* step, const std::vector<G4double>& energy);

private:
  LArG4H62004CalibSD (const LArG4H62004CalibSD&);
  LArG4H62004CalibSD& operator= (const LArG4H62004CalibSD&);
};

#endif // __LArG4H62004CalibSD_H__
