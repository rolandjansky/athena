/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_LARG4TBECBEAMCHCALCULATOR_H
#define LARG4H6SD_LARG4TBECBEAMCHCALCULATOR_H

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Step;

class LArG4TBECBeamChCalculator
{
  public:
        LArG4TBECBeamChCalculator(void);
        ~LArG4TBECBeamChCalculator() {;}

        G4bool Process(const G4Step*);
        G4int identifier() const { return m_number; }

        G4double x() const { return m_x; }
        G4double y() const { return m_y; };

  private:

        // number ob beamchamber
        G4int m_number;
        G4double m_x;
        G4double m_y;
};

#endif
