/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DUMMYEQUATION_H
#define DUMMYEQUATION_H

#include "G4EquationOfMotion.hh"
#include "G4UniformMagField.hh"
#include "G4Version.hh"
class DummyEquation : public G4EquationOfMotion {
public:
    DummyEquation();
    virtual ~DummyEquation() {}
    virtual void EvaluateRhsGivenB(const G4double[], const G4double[3], G4double[]) const {}
#if G4VERSION_NUMBER > 1009
  virtual void SetChargeMomentumMass(G4ChargeState, G4double, G4double) {}
#else
  virtual void SetChargeMomentumMass(G4double, G4double, G4double) {}
#endif
private:
    static G4UniformMagField s_dummyField;
};

#endif
