/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FieldManager_H
#define FieldManager_H

#include <string>
#include <map>

class G4MagIntegratorStepper;
class G4Mag_EqRhs;
class G4ChordFinder;
#include "G4MagneticField.hh"
#include "FadsField/MagneticFieldMap.h"

namespace FADS {


  class MagneticFieldMap;
  class IntegratorStepperBase;
  class EqOfMotionFactoryBase;

  typedef std::map<std::string, MagneticFieldMap*, std::less<std::string> > FieldMap;
  typedef std::map<std::string, IntegratorStepperBase*, std::less<std::string> > StepperMap;
  typedef std::map<std::string, EqOfMotionFactoryBase*, std::less<std::string> > EquationMap;

  class FieldManager {
  private:
    static FieldManager* s_thePointer;
    FieldManager();

  public:
    static FieldManager* GetFieldManager();

    void RegisterMagneticField(MagneticFieldMap* map);
    void SelectMagneticField(const std::string& name);
    void SelectAndCreateMagneticField(const std::string& name);
    MagneticFieldMap* GetMagneticField(const std::string& name);
    const MagneticFieldMap* GetMagneticField(const std::string& name) const;
    void InitializeField();
    void PrintFieldList() const;

    void RegisterEqOfMotion(EqOfMotionFactoryBase*);
    G4Mag_EqRhs* GetEqOfMotion(const std::string);

    void RegisterStepper(IntegratorStepperBase* stepper);
    void ListSteppers() const;
    const StepperMap Steppers() const { return m_theSteppers; }
    std::string CurrentStepper() const { return m_currentStepperName; }
    void SetCurrentStepper(const std::string& name);
    void UseCurrentStepper();
    void UseStepper(const std::string& name);
    G4MagIntegratorStepper* GetStepper(const std::string&); 
    G4Mag_EqRhs *GetEquationOfMotion();

    static void SetDeltaOneStep(double);
    static void SetDeltaIntersection(double);
    static void SetMinimumEpsilonStep(double);
    static void SetMaximumEpsilonStep(double);
    void EquationOfMotion(const std::string s) {m_eqOfMotion=s;}
    std::string EquationOfMotion() const {return m_eqOfMotion;}
    G4MagneticField* GetMagneticFieldMap() {return m_currentField;}

    G4MagIntegratorStepper* GetCurrentStepper() {return m_currentStepper;}

  private:
    FieldMap m_fieldCatalog;
    StepperMap m_theSteppers;
    EquationMap m_theEquations;
    MagneticFieldMap* m_currentField;
    G4MagIntegratorStepper* m_currentStepper;
    std::string m_currentStepperName;
    G4Mag_EqRhs* m_theEquation;
    G4ChordFinder* m_theChordFinder;
    std::string m_eqOfMotion;
  };

}

#endif
