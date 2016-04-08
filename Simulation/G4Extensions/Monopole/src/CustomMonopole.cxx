/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//---------------------------------------------------------------------------
//
// ClassName:   CustomMonopole
//
// Description:
//
// Authors:   21.03.05  V.Ivanchenko
//
// Modified:
//
//----------------------------------------------------------------------------
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// class header
#include "CustomMonopole.h"
// Geant4 headers
#include "G4ParticleTable.hh"
// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

// ######################################################################
// ###                        Monopole                                ###
// ######################################################################

// CustomMonopole* CustomMonopole::theMonopole = 0;
// G4double    CustomMonopole::magCharge = 0.0;

CustomMonopole::CustomMonopole(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,
       G4int               iSpin,        G4int               iParity,
       G4int               iConjugation, G4int               iIsospin,
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable)
 : G4ParticleDefinition( aName,mass,width,charge,iSpin,iParity,
           iConjugation,iIsospin,iIsospin3,gParity,pType,
           lepton,baryon,encoding,stable,lifetime,decaytable )
 , m_magCharge(0.)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

CustomMonopole::~CustomMonopole()
{}

void CustomMonopole::PrintMonopoleInfo() const
{    G4cout << "CustomMonopole Info: m(GeV)= " << this->GetPDGMass()/CLHEP::GeV
            << " Qel= " << this->GetPDGCharge()/CLHEP::eplus
            << " Qmag= " << m_magCharge/CLHEP::eplus
            << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
//    Arguments for constructor are as follows
//               name             mass          width         charge
//             2*spin           parity  C-conjugation
//          2*Isospin       2*Isospin3       G-parity
//               type    lepton number  baryon number   PDG encoding
//             stable         lifetime    decay table
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// CustomMonopole* CustomMonopole::MonopoleDefinition(
//                                                 const G4String&     aName,        G4double            mass,
//                                                 G4double            width,        G4double            charge,
//                                                 G4int               iSpin,        G4int               iParity,
//                                                 G4int               iConjugation, G4int               iIsospin,
//                                                 G4int               iIsospin3,    G4int               gParity,
//                                                 const G4String&     pType,        G4int               lepton,
//                                                 G4int               baryon,       G4int               encoding,
//                                                 G4bool              stable,       G4double            lifetime,
//                                                 G4DecayTable        *decaytable,
//                                                 G4double mCharge)
// {

//   if(!theMonopole) {
//     m_magCharge = eplus*mCharge*0.5/fine_structure_const;
//     theMonopole = new CustomMonopole( aName,mass,width,charge,iSpin,iParity,
//                                    iConjugation,iIsospin,iIsospin3,gParity,pType,
//                                    lepton,baryon,encoding,stable,lifetime,decaytable);

//     G4cout << "Monopole is created: m(GeV)= " << theMonopole->GetPDGMass()/GeV
//         << " Qel= " << theMonopole->GetPDGCharge()/eplus
//         << " Qmag= " << m_magCharge/eplus
//         << G4endl;
//   }

//   return theMonopole;
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// CustomMonopole* CustomMonopole::Monopole()
// {
//   if(!theMonopole) theMonopole = MonopoleDefinition();
//   return theMonopole;
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
