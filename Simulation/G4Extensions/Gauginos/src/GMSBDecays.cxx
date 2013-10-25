/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Gauginos/GMSBDecays.hh"
#include "Gauginos/GMSBNeutralino.hh"
#include "Gauginos/GMSBGravitino.hh"
#include "FadsPhysics/UserPhysicsDefinitionProxy.h"

#include "G4Transportation.hh"
#include "G4ProcessManager.hh"
#include "G4DecayTable.hh"
#include "G4VDecayChannel.hh"
#include "G4VDecayChannel.hh"
#include "G4PhaseSpaceDecayChannel.hh"

static UserPhysicsDefinitionProxy<GMSBDecays> plugin; 

GMSBDecays::GMSBDecays(): 
	UserPhysicsDefinition()
{	
	std::cout <<" physics list for Gauginos being built "<<std::endl;
}

void GMSBDecays::ConstructProcess()
{
	std::cout <<" ConstructProcess for the Gauginos being run"<<std::endl;
}

void GMSBDecays::ConstructParticle()
{
	std::cout <<" ConstructParticle for the Gauginos being run"<<std::endl;
	GMSBNeutralino::Definition();
	GMSBGravitino::Definition();
}
