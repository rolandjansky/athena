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



  FieldManager* FieldManager::s_thePointer = FieldManager::GetFieldManager();


  FieldManager::FieldManager()
    : m_currentField(NULL),m_currentStepper(NULL),
      m_theEquation(NULL),m_theChordFinder(NULL),m_eqOfMotion("Default")
  {
    // default primary event action
  }


  FieldManager* FieldManager::GetFieldManager() {
    if (!s_thePointer) s_thePointer = new FieldManager();
    return s_thePointer;
  }


  void FieldManager::RegisterMagneticField(MagneticFieldMap* map) {
    m_fieldCatalog[map->GetName()] = map;
  }

  void FieldManager::InitializeField()
  {
    if (!m_currentField)
      {
        std::cout<<" Field Manager initialization: a field map"
                 <<" has not been set: running without field? "<<std::endl;
        return;
      }
    m_currentField->Create();
    if (!m_currentField->IsInitialized()) m_currentField->SetInitialValues();
    G4FieldManager* fieldMgr=G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetDetectorField(m_currentField);
    fieldMgr->CreateChordFinder(m_currentField);
    m_theChordFinder=fieldMgr->GetChordFinder();
    m_theEquation = GetEquationOfMotion();
  }

  void FieldManager::SelectMagneticField(const string& name) {
    if (m_fieldCatalog.find(name) == m_fieldCatalog.end()) {
      cout << "FieldManager Warning!!! Field " << name <<" not found!!!!" << endl;
      return;
    }
    // if (m_currentField) m_currentField->Terminate;
    m_currentField = m_fieldCatalog[name];
  }


  void FieldManager::SelectAndCreateMagneticField(const string& name) {
    if (m_fieldCatalog.find(name) == m_fieldCatalog.end())  {
      cout << "FieldManager Warning!!! Field " << name << " not found!!!!" << endl;
      return;
    }
    // if (m_currentField) m_currentField->Terminate;
    m_currentField = m_fieldCatalog[name];
    m_currentField->Create();
  }


  MagneticFieldMap* FieldManager::GetMagneticField(const string& name) {
    FieldMap::iterator it_f = m_fieldCatalog.find(name);
    if (it_f == m_fieldCatalog.end()) {
      cout << " FieldManager Warning!!! Field " << name << " not found!!!!" << endl;
      return 0;
    }
    return it_f->second;
  }

  G4Mag_EqRhs* FieldManager::GetEquationOfMotion()
  {
    if (!m_theEquation) {
      if (m_eqOfMotion=="Default") {
        m_theEquation=new G4Mag_UsualEqRhs(m_currentField);
      }
      else {
        m_theEquation=GetEqOfMotion(m_eqOfMotion);
      }
    }
    return m_theEquation;
  }


  const MagneticFieldMap* FieldManager::GetMagneticField(const string& name) const {
    FieldMap::const_iterator it_f = m_fieldCatalog.find(name);
    if (it_f == m_fieldCatalog.end()) {
      cout << " FieldManager Warning!!! Field " << name << " not found!!!!" << endl;
      return 0;
    }
    return it_f->second;
  }


  /// @todo Not very useful... returning the field list would be better
  void FieldManager::PrintFieldList() const {
    FieldMap::const_iterator it;
    cout << " List of Magnetic Field Maps available: " << endl;
    for (it = m_fieldCatalog.begin(); it != m_fieldCatalog.end(); ++it)
      cout << "\t---> " << it->first << "\t\t<---" << endl;
    cout << " <--------------------------------------->" << endl;
  }

  void FieldManager::RegisterEqOfMotion(EqOfMotionFactoryBase* eq) {
    cout << "Registering equation of motion" << eq->GetName() << endl;
    m_theEquations[eq->GetName()] = eq;
  }

  G4Mag_EqRhs* FieldManager::GetEqOfMotion(const std::string eqM) {
    if (m_theEquations.find(eqM) == m_theEquations.end())
      {
        cout << " FieldManager could not find equation of motion " << eqM << endl;
        return 0;
      }
    return m_theEquations[eqM]->Build();
  }

  void FieldManager::RegisterStepper(IntegratorStepperBase* stepper) {
    //cout << "Registering stepper " << stepper->GetName() << endl;
    m_theSteppers[stepper->GetName()] = stepper;
  }


  void FieldManager::ListSteppers() const {
    cout << " List of all Integrator Steppers available " << endl;
    for (StepperMap::const_iterator it = m_theSteppers.begin(); it != m_theSteppers.end(); ++it) {
      cout << "---- " << it->first << " ----" << endl;
    }
  }


  void FieldManager::SetCurrentStepper(const string& n) {
    if (m_theSteppers.find(n) == m_theSteppers.end()) {
      cout << "Stepper " << n << " not found! Stepper not changed" << endl;
      return;
    }
    m_currentStepperName = n;
    m_currentStepper = m_theSteppers[n]->Build();
  }


  void FieldManager::UseCurrentStepper() {
    if (m_theChordFinder && m_theChordFinder->GetIntegrationDriver()) {
      m_theChordFinder->GetIntegrationDriver()->RenewStepperAndAdjust(m_currentStepper);
    }
  }


  void FieldManager::UseStepper(const string& n) {
    /// @todo It would be good to replace most of this with a call to SetCurrentStepper...
    ///       but we can't since there is no way to report a failure. Is this function actually needed?
    if (m_theSteppers.find(n) == m_theSteppers.end())  {
      cout << "Stepper " << n << " not found! Stepper not changed" << endl;
      return;
    }
    m_currentStepperName = n;
    m_currentStepper = m_theSteppers[n]->Build();
    UseCurrentStepper();
  }

  G4MagIntegratorStepper* FieldManager::GetStepper(const std::string& n){
    if (n == m_currentStepperName) {
      cout << "FieldManager: the stepper " << n << " and m_currentStepper are the same. Return the m_currentStepper." << endl;
      return m_currentStepper;
    }
    if (m_theSteppers.find(n) == m_theSteppers.end())  {
      cout << "Stepper " << n << " not found. Return 0!!!" << endl;
      return 0;
    }
    return m_theSteppers[n]->Build();
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
