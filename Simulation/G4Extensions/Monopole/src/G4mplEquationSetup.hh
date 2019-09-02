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
// $Id: $
//
// G4mplEquationSetup is responsible for switching between two different
// equation of motions, one for monopoles and another for the rest of the
// particles.
//
// =======================================================================
// Modified: 19 May 2019, M. Bandieramonte: introduced MT mode. The class
// 	     was a Singleton and it was not thread-safe.
//           Added the #ifdef G4MULTITHREADED directive to handle
//           the multithreaded case. One instance of the class will be created
//           per each thread and stored in a tbb::concurrent_unordered_map that
//           is hashed with the threadID number.
//
// Modified: 28 August 2013, W. Taylor: adapted for ATLAS
// Created:  23 May 2013, J. Apostolakis
//            Adapted from G4MonopoleFieldSetup by B. Bozsogi
// =======================================================================

#ifndef MONOPOLE_G4mplEquationSetup_H
#define MONOPOLE_G4mplEquationSetup_H

#include <thread>
#include "G4Types.hh"
#ifdef G4MULTITHREADED
#  include "tbb/concurrent_unordered_map.h"
#endif

// Geant4 headers
#include "G4MagneticField.hh"

class G4FieldManager;
class G4ChordFinder;
class G4Mag_UsualEqRhs;
class G4MagIntegratorStepper;
class G4mplEqMagElectricField;
class G4MagneticField;

class G4mplEquationSetup
{
public:

  void InitialiseForField(G4FieldManager* fieldManager );

  void SwitchStepperAndChordFinder(G4bool val, G4FieldManager* fieldManager );
  // Switch between mode for 'Monopole' and ordinary propagation
  //  0 - ordinary
  //  1 - monopole

  static G4mplEquationSetup* GetInstance();

  ~G4mplEquationSetup() ;

private: 

#ifdef G4MULTITHREADED
     // Thread-to-EquationSetup concurrent map type
     using ESThreadMap_t = tbb::concurrent_unordered_map< std::thread::id, G4mplEquationSetup*, std::hash<std::thread::id> >;
     // Concurrent map of EquationsSetup, one for each thread
     static ESThreadMap_t m_ESThreadMap;
     //@brief Search inside m_ESThreadMap the element with the current threadID 
     // and return it or return a null pointer if the element is not found
     static G4mplEquationSetup* getES();
     // @brief Insert the current ES in m_ESThreadMap and 
     // associate it with the current threadID
     static G4mplEquationSetup* setES();
 #endif
     //

  G4mplEquationSetup();

  //
  // Invariants - constant during tracking
  // ----------

  // Objects owned
  G4Mag_UsualEqRhs*       fEquation ;
  G4mplEqMagElectricField*     fMonopoleEquation ;
  G4MagIntegratorStepper* fMonopoleStepper ;

  // Parameters - invariant during stepping
  G4double                fMinStep ;

  //  Objects typically not owned
  G4MagIntegratorStepper* fStepper ;
  G4bool                  fCreatedOrdinaryStepper; // If set, created stepper.

  G4bool                      fVerbose;
  //
  // State - changed during tracking
  // -----

  //  The chord finder must be changed during tracking
  G4ChordFinder*          fChordFinder ;
};

#endif
