/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsField/IntegratorStepperBase.h"
#include "FadsField/IntegratorStepperFactory.h"

#include "G4CashKarpRKF45.hh"
static FADS::IntegratorStepperFactory<G4CashKarpRKF45> s1("CashKarpRKF45");

#include "G4ClassicalRK4.hh"
static FADS::IntegratorStepperFactory<G4ClassicalRK4> s2("ClassicalRK4");

#include "G4SimpleHeum.hh"
static FADS::IntegratorStepperFactory<G4SimpleHeum> s3("SimpleHeum");

#include "G4SimpleRunge.hh"
static FADS::IntegratorStepperFactory<G4SimpleRunge> s4("SimpleRunge");

#include "G4HelixExplicitEuler.hh"
static FADS::IntegratorStepperFactory<G4HelixExplicitEuler> s5("HelixExplicitEuler");

#include "G4HelixImplicitEuler.hh"
static FADS::IntegratorStepperFactory<G4HelixImplicitEuler> s6("HelixImplicitEuler");

#include "G4HelixSimpleRunge.hh"
static FADS::IntegratorStepperFactory<G4HelixSimpleRunge> s7("HelixSimpleRunge");

#include "G4AtlasRK4.hh"
static FADS::IntegratorStepperFactory<G4AtlasRK4> s8("AtlasRK4");

#include "G4NystromRK4.hh"
static FADS::IntegratorStepperFactory<G4NystromRK4> s9("NystromRK4");
