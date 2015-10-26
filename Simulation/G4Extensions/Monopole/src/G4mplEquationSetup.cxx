//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file Derived from example exoticphysics G4MonopolFieldSetup.cc
/// \brief Implementation of the G4mplEquationSetup class
//
// $Id: G4mplEquationSetup.cxx 684372 2015-07-20 15:34:53Z jchapman $
//
//
// G4mplEquationSetup is responsible for switching between two different
// equation of motions, one for monopoles and another for the rest of the
// particles.
//

// =======================================================================
// Modified: 28 August 2013, W. Taylor: adapted for ATLAS
// Created:  23 May 2013,    J. Apostolakis
//            Adapted from G4MonopoleFieldSetup by B. Bozsogi
// =======================================================================

// class header
#include "G4mplEquationSetup.hh"
// package headers
#include "G4mplEqMagElectricField.hh"
// Geant4 headers
#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"
#include "G4NystromRK4.hh" // Used for non-magnetic particles
#include "G4ClassicalRK4.hh" // Used for monopoles
// NOTE:
// Only Steppers which can integrate over any number of variables can be used
//  to integrate the equation of motion of a monopole.
// The following cannot be used:
//    Helix...  - these integrate over 6 variables only; also assume helix as 'base'
//    AtlasRK4, NystromRK4 - these have the equation of motion embedded inside
#include "G4SystemOfUnits.hh"

G4mplEquationSetup* G4mplEquationSetup::fG4mplEquationSetup=0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4mplEquationSetup::G4mplEquationSetup():
  fEquation(0),
  fMonopoleEquation(0),
  fMonopoleStepper(0),
  fMinStep(0.01*CLHEP::mm), // minimal step of 1 mm is default
  fStepper(0),
  fCreatedOrdinaryStepper(false),
  fVerbose(false),
  fChordFinder(0)

{
  G4cout << "!!! G4mplEquationSetup constructor" <<  G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4mplEquationSetup* G4mplEquationSetup::GetInstance()
{
  if ( fG4mplEquationSetup == 0 )
    {
      static G4mplEquationSetup theInstance;
      fG4mplEquationSetup = &theInstance;
    }

  return fG4mplEquationSetup;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4mplEquationSetup::~G4mplEquationSetup()
{
  G4cout << "!!! G4mplEquationSetup destructor" << G4endl;

  if ( fEquation ) delete fEquation;
  if ( fMonopoleEquation ) delete fMonopoleEquation;
  // WJT: Avoid segmentation violation in G4FieldManager destructor
  //if ( fChordFinder )   delete fChordFinder;
  if ( fCreatedOrdinaryStepper ) delete fStepper;
  if ( fMonopoleStepper )  delete fMonopoleStepper;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void
G4mplEquationSetup::InitialiseForField(G4FieldManager* fieldManager )
{
  G4cout << "G4mplEquationSetup::InitialiseForField called for field Manager " << fieldManager << " for monopole transport. " << G4endl;

  const G4MagneticField* magField = dynamic_cast<const G4MagneticField*>(fieldManager->GetDetectorField());

  G4MagneticField* magFieldNC = const_cast<G4MagneticField*>(magField);

  delete fEquation;
  fEquation = new G4Mag_UsualEqRhs(magFieldNC);

  delete fMonopoleEquation;
  fMonopoleEquation = new G4mplEqMagElectricField(magFieldNC);

  delete fMonopoleStepper;
  fMonopoleStepper = new G4ClassicalRK4( fMonopoleEquation, 8 ); // for time information..

  if ( !fMonopoleStepper )
    {
      G4cout << " G4mplEquationSetup: Warning - found no stepper.  Created G4Nystrom stepper - for ordinary particles. " << G4endl;
      delete fStepper;
      fStepper = new G4NystromRK4( fEquation );
      fCreatedOrdinaryStepper = true;
      SwitchStepperAndChordFinder( 0, fieldManager ); // Set for 'ordinary' matter
    }
  else
    {
      if( fVerbose ) G4cout << "G4mplEquationSetup: fMonopoleStepper has " << fMonopoleStepper->GetNumberOfVariables() << " variables." << G4endl;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4mplEquationSetup::SwitchStepperAndChordFinder(G4bool useMonopoleEq,
                                                     G4FieldManager* fieldManager)
{
  // For this to work, InitialiseForField must have been called
  //   - for *this* field manager (not another) before this call
  //   - potentially after a change of 'field' object in this field manager (tbc)

  const G4MagneticField* magField = dynamic_cast<const G4MagneticField*>(fieldManager->GetDetectorField());
  G4MagneticField* magFieldNC= const_cast<G4MagneticField*>(magField);

  if ( magField )
    {
      // WJT: Avoid segmentation violation in G4FieldManager destructor
      //if (fChordFinder) delete fChordFinder;
      fChordFinder= 0;

      if (useMonopoleEq)
        {
          fChordFinder = new G4ChordFinder( magFieldNC, fMinStep, fMonopoleStepper );
          if( fVerbose )  G4cout << " G4mplEquationSetup: Configured for Monopole " << G4endl;
        }
      else
        {
          fChordFinder = new G4ChordFinder( magFieldNC, fMinStep, fStepper );
          if ( fVerbose )  G4cout << " G4mplEquationSetup: Configured for electric charge only. " << G4endl;
        }

      fieldManager->SetChordFinder( fChordFinder );
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
