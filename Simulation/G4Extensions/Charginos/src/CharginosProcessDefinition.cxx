/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Charginos/CharginosProcessDefinition.h"
#include "Charginos/AMSBCharginoPlus.hh"
#include "Charginos/AMSBCharginoMinus.hh"
#include "Charginos/AMSBNeutralino.hh"
#include "FadsPhysics/UserPhysicsDefinitionProxy.h"

#include "G4hhIonisation.hh"

#include "G4Transportation.hh"
#include "G4MuIonisation.hh"
#include "G4ProcessManager.hh"

#include "G4DecayTable.hh"
#include "G4VDecayChannel.hh"
#include "G4PhaseSpaceDecayChannel.hh"

static UserPhysicsDefinitionProxy<CharginosProcessDefinition> plugin;

CharginosProcessDefinition::CharginosProcessDefinition():
  UserPhysicsDefinition(),
  theCharginoMinus(0), theCharginoPlus(0), theNeutralino(0)
{
	std::cout <<" physics list for the Charginos being built "<<std::endl;
}

void CharginosProcessDefinition::ConstructProcess()
{
    std::cout <<" ConstructProcess for Charginos being run"<<std::endl;

    G4hhIonisation* theIonisation = new G4hhIonisation;
    G4Transportation *theTransportation=new G4Transportation;

    G4ProcessManager *charginoPlus= theCharginoPlus->GetProcessManager();
    G4ProcessManager *charginoMinus=theCharginoMinus->GetProcessManager();

    charginoPlus->AddProcess(theTransportation);
    charginoMinus->AddProcess(theTransportation);
    charginoPlus->AddProcess(theIonisation,-1,2,2);
    charginoMinus->AddProcess(theIonisation,-1,2,2);
}

void CharginosProcessDefinition::ConstructParticle()
{
  std::cout <<" ConstructParticle for the Charginos being run"<<std::endl;

  theCharginoPlus = AMSBCharginoPlus::Definition();
  theCharginoMinus = AMSBCharginoMinus::Definition();
  theNeutralino = AMSBNeutralino::Definition();
}
