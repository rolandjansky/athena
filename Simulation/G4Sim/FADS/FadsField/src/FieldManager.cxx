/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsField/FieldManager.h"
#include "FadsField/MagneticFieldMap.h"
#include "FadsField/IntegratorStepperBase.h"
#include "FadsField/IntegratorStepperFactory.h"
#include "FadsField/EqOfMotionFactoryBase.h"
#include "FadsField/EqOfMotionFactory.h"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4Mag_EqRhs.hh"

using namespace std;

namespace FADS {



  FieldManager* FieldManager::thePointer = FieldManager::GetFieldManager();


  FieldManager::FieldManager()
    : currentField(NULL),currentStepper(NULL),
      theEquation(NULL),theChordFinder(NULL),eqOfMotion("Default")
  {
    // default primary event action
  }


  FieldManager* FieldManager::GetFieldManager() {
    if (!thePointer) thePointer = new FieldManager();
    return thePointer;
  }


  void FieldManager::RegisterMagneticField(MagneticFieldMap* map) {
    fieldCatalog[map->GetName()] = map;
  }

  void FieldManager::InitializeField()
  {
    if (!currentField)
      {
        std::cout<<" Field Manager initialization: a field map"
                 <<" has not been set: running without field? "<<std::endl;
        return;
      }
    currentField->Create();
    if (!currentField->IsInitialized()) currentField->SetInitialValues();
    G4FieldManager* fieldMgr=G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetDetectorField(currentField);
    fieldMgr->CreateChordFinder(currentField);
    theChordFinder=fieldMgr->GetChordFinder();
    theEquation = GetEquationOfMotion();
  }

  void FieldManager::SelectMagneticField(const string& name) {
    if (fieldCatalog.find(name) == fieldCatalog.end()) {
      cout << "FieldManager Warning!!! Field " << name <<" not found!!!!" << endl;
      return;
    }
    // if (currentField) currentField->Terminate;
    currentField = fieldCatalog[name];
  }


  void FieldManager::SelectAndCreateMagneticField(const string& name) {
    if (fieldCatalog.find(name) == fieldCatalog.end())  {
      cout << "FieldManager Warning!!! Field " << name << " not found!!!!" << endl;
      return;
    }
    // if (currentField) currentField->Terminate;
    currentField = fieldCatalog[name];
    currentField->Create();
  }


  MagneticFieldMap* FieldManager::GetMagneticField(const string& name) {
    FieldMap::iterator it_f = fieldCatalog.find(name);
    if (it_f == fieldCatalog.end()) {
      cout << " FieldManager Warning!!! Field " << name << " not found!!!!" << endl;
      return 0;
    }
    return it_f->second;
  }

  G4Mag_EqRhs* FieldManager::GetEquationOfMotion()
  {
    if (!theEquation) {
      if (eqOfMotion=="Default") {
        theEquation=new G4Mag_UsualEqRhs(currentField);
      }
      else {
        theEquation=GetEqOfMotion(eqOfMotion);
      }
    }
    return theEquation;
  }


  const MagneticFieldMap* FieldManager::GetMagneticField(const string& name) const {
    FieldMap::const_iterator it_f = fieldCatalog.find(name);
    if (it_f == fieldCatalog.end()) {
      cout << " FieldManager Warning!!! Field " << name << " not found!!!!" << endl;
      return 0;
    }
    return it_f->second;
  }


  /// @todo Not very useful... returning the field list would be better
  void FieldManager::PrintFieldList() const {
    FieldMap::const_iterator it;
    cout << " List of Magnetic Field Maps available: " << endl;
    for (it = fieldCatalog.begin(); it != fieldCatalog.end(); ++it)
      cout << "\t---> " << it->first << "\t\t<---" << endl;
    cout << " <--------------------------------------->" << endl;
  }

  void FieldManager::RegisterEqOfMotion(EqOfMotionFactoryBase* eq) {
    cout << "Registering equation of motion" << eq->GetName() << endl;
    theEquations[eq->GetName()] = eq;
  }

  G4Mag_EqRhs* FieldManager::GetEqOfMotion(const std::string eqM) {
    if (theEquations.find(eqM) == theEquations.end())
      {
        cout << " FieldManager could not find equation of motion " << eqM << endl;
        return 0;
      }
    return theEquations[eqM]->Build();
  }

  void FieldManager::RegisterStepper(IntegratorStepperBase* stepper) {
    cout << "Registering stepper " << stepper->GetName() << endl;
    theSteppers[stepper->GetName()] = stepper;
  }


  void FieldManager::ListSteppers() const {
    cout << " List of all Integrator Steppers available " << endl;
    for (StepperMap::const_iterator it = theSteppers.begin(); it != theSteppers.end(); ++it) {
      cout << "---- " << it->first << " ----" << endl;
    }
  }


  void FieldManager::SetCurrentStepper(const string& n) {
    if (theSteppers.find(n) == theSteppers.end()) {
      cout << "Stepper " << n << " not found! Stepper not changed" << endl;
      return;
    }
    currentStepperName = n;
    currentStepper = theSteppers[n]->Build();
  }


  void FieldManager::UseCurrentStepper() {
    if (theChordFinder && theChordFinder->GetIntegrationDriver()) {
      theChordFinder->GetIntegrationDriver()->RenewStepperAndAdjust(currentStepper);
    }
  }


  void FieldManager::UseStepper(const string& n) {
    /// @todo It would be good to replace most of this with a call to SetCurrentStepper...
    ///       but we can't since there is no way to report a failure. Is this function actually needed?
    if (theSteppers.find(n) == theSteppers.end())  {
      cout << "Stepper " << n << " not found! Stepper not changed" << endl;
      return;
    }
    currentStepperName = n;
    currentStepper = theSteppers[n]->Build();
    UseCurrentStepper();
  }

  G4MagIntegratorStepper* FieldManager::GetStepper(const std::string& n){
    if (n == currentStepperName) {
      cout << "FieldManager: the stepper " << n << " and currentStepper are the same. Return the currentStepper." << endl;
      return currentStepper;
    }
    if (theSteppers.find(n) == theSteppers.end())  {
      cout << "Stepper " << n << " not found. Return 0!!!" << endl;
      return 0;
    }
    return theSteppers[n]->Build();
  }

  void FieldManager::SetDeltaOneStep(double value) {
    G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetDeltaOneStep(value);
  }


  void FieldManager::SetDeltaIntersection(double value) {
    G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetDeltaIntersection(value);
  }


  void FieldManager::SetMinimumEpsilonStep(double value) {
    G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetMinimumEpsilonStep(value);
  }


  void FieldManager::SetMaximumEpsilonStep(double value) {
    G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetMaximumEpsilonStep(value);
  }


}
