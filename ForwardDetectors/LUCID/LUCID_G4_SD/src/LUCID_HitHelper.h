/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_HITIDHELPER
#define LUCID_HITIDHELPER 1

// Athena includes
#include "HitManagement/HitIdHelper.h"

// Geant4 includes
#include "globals.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"

// CLHEP includes
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

// STL includes
#include <cmath>

class LUCID_HitHelper: HitIdHelper {

 public:

  LUCID_HitHelper(){}
  ~LUCID_HitHelper(){}

  inline G4ThreeVector GetPreStepPoint (G4Step* aStep) const { return aStep->GetPreStepPoint()->GetPosition();    }
  inline G4ThreeVector GetPostStepPoint(G4Step* aStep) const { return aStep->GetPostStepPoint()->GetPosition();   }
  inline float         GetPreStepTime  (G4Step* aStep) const { return aStep->GetPreStepPoint()->GetGlobalTime();  }
  inline float         GetPostStepTime (G4Step* aStep) const { return aStep->GetPostStepPoint()->GetGlobalTime(); }

  inline double GetWaveLength(double energy) const { return 2.*M_PI*CLHEP::hbarc/(CLHEP::eV*CLHEP::nm)/energy; }
  inline double GetEnergy    (double lambda) const { return 2.*M_PI*CLHEP::hbarc/(CLHEP::eV*CLHEP::nm)/lambda; }

  static int GetVolNumber(const G4String&) ;
  int GetTubNumber(G4Step* ) const;
};

#endif
