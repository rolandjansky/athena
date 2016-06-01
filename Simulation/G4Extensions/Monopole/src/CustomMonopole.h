/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONOPOLE_CustomMonopole_h
#define MONOPOLE_CustomMonopole_h 1

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
// ClassName:   CustomMonopole
//
// Authors:   21.03.05  V.Ivanchenko
//
// Modified:
// 15.10.10 S. Burdin adapted for Atlas
//
//----------------------------------------------------------------------------
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "CLHEP/Units/PhysicalConstants.h"

// ######################################################################
// ###                       CustomMonopole                           ###
// ######################################################################

class CustomMonopole : public G4ParticleDefinition
{
public:

  CustomMonopole(
                 const G4String&     aName,        G4double            mass,
                 G4double            width,        G4double            charge,
                 G4int               iSpin,        G4int               iParity,
                 G4int               iConjugation, G4int               iIsospin,
                 G4int               iIsospin3,    G4int               gParity,
                 const G4String&     pType,        G4int               lepton,
                 G4int               baryon,       G4int               encoding,
                 G4bool              stable,       G4double            lifetime,
                 G4DecayTable        *decaytable );


  virtual ~CustomMonopole();

  G4double MagneticCharge() const {return m_magCharge;}

  void SetMagneticCharge(G4double mCharge = 1.0) {m_magCharge = CLHEP::eplus*mCharge*0.5/CLHEP::fine_structure_const;}

  void PrintMonopoleInfo() const;

private:

  G4double m_magCharge;
};

#endif
