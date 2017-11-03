/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsField/FadsFieldManager.h"
#include "FadsField/FieldManagerStore.h"
#include "FadsField/FieldManager.h"
#include "FadsField/MagneticFieldMap.h"

#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Region.hh"
#include "G4MagIntegratorStepper.hh"

#include "FadsField/RegionInformation.h"
#include "FadsField/FieldIntParameters.h"

namespace FADS {


  FadsFieldManager::FadsFieldManager(std::string name):G4FieldManager(),m_name(name)
  {
    FieldManagerStore* fms=FieldManagerStore::GetFMStore();
    fms->RegisterFieldManager(this);

    m_defaultParameters = new FADS::FieldIntParameters();
    m_previousTrack = NULL;
    m_previousRegion = NULL;
  }


  void FadsFieldManager::SetField(std::string /*fName*/)
  {
    //  FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();

    // step one - get the field map, create it, initialize it
    //   MagneticFieldMap* magField=fm->GetMagneticField(fName);

    //     if (!magField) return;

    //  G4MagneticField* theMap=magField->Create(fName);
    //  if (!magField->IsInitialized()) magField->SetInitialValues();
    //  std::cout<<"Field initialization completed!"<<std::endl;

    //      G4Mag_EqRhs* eqMotion = fm->GetEquationOfMotion();
    //  fStepperDispatcher = new FADS::StepperDispatcher(eqMotion);

    //  G4double minStep = 0.001*CLHEP::mm;
    //  std::cout<<"Making a chord finder with the stepper dispatcher!"<<std::endl;

    //      G4ChordFinder* chordFinder = new G4ChordFinder(theMap,minStep,fStepperDispatcher);

    //  SetDetectorField(theMap);
    //  SetChordFinder(chordFinder);

    //  std::cout<<"Construction of FadsFieldManager complete!!"<<std::endl;
  }

  std::string FadsFieldManager::GetName() const
  {
    return m_name;
  }

  void FadsFieldManager::SetStepper(G4MagIntegratorStepper* stepper)
  {
    if (stepper == 0)
      {
        std::cout<<"FadsFieldManager: Can't set NULL stepper. Nothing changed." <<std::endl;
        return;
      }

    G4ChordFinder * theChordFinder = this->GetChordFinder();

    if (theChordFinder)
      {
        if (theChordFinder->GetIntegrationDriver())
          {
            theChordFinder->GetIntegrationDriver()->RenewStepperAndAdjust(stepper);
          }
        else
          {
            std::cout<<"FadsFieldManager: theChordFinder=" << theChordFinder <<
              " theChordFinder->GetIntegrationDriver()=" <<
              theChordFinder->GetIntegrationDriver() <<
              ". Something is wrong. " <<std::endl;
          }
      }
    else
      {
        std::cout<<"FadsFieldManager: theChordFinder=NULL. Something is wrong. " <<std::endl;
      }
    return;
  }

}
