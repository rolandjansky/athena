/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef G4UserActions_LooperThresholdSet_H
#define G4UserActions_LooperThresholdSet_H

#include "G4UserRunAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

#include "G4Run.hh"
#include "G4ParticleDefinition.hh"
#include "G4Transportation.hh"
#include "G4CoupledTransportation.hh"
#include "G4SystemOfUnits.hh"

namespace G4UA
{

class LooperThresholdSet : public AthMessaging, public G4UserRunAction
{
public:

struct Config
{
        double WarningEnergy = 100.0 * CLHEP::MeV;
        double ImportantEnergy = 250.0 * CLHEP::MeV;
        int NumberOfTrials = 10;
};

LooperThresholdSet( const Config& config );
virtual void BeginOfRunAction( const G4Run* ) override;

private:

Config m_config;

void ChangeLooperParameters( const G4ParticleDefinition* particleDef );
std::pair<G4Transportation*, G4CoupledTransportation*> findTransportation( const G4ParticleDefinition* particleDef );
};   // class LooperThresholdSet

} // namespace G4UA

#endif
