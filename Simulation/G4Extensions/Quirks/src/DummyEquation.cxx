/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DummyEquation.h"

DummyEquation::DummyEquation() : G4EquationOfMotion(&s_dummyField) {}

G4UniformMagField DummyEquation::s_dummyField(0, 0, 0);
