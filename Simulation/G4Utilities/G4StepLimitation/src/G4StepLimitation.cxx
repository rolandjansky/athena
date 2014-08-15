/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4StepLimitation/G4StepLimitProcess.h"
#include "FadsPhysics/UserPhysicsDefinitionProxy.h"

#include "G4StepLimiter.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

static UserPhysicsDefinitionProxy<G4StepLimitProcess> plugin; 

G4StepLimitProcess::G4StepLimitProcess(): 
	UserPhysicsDefinition()
{	
	std::cout <<" G4StepLimiter being built "<<std::endl;
}

void G4StepLimitProcess::ConstructProcess()
{
	std::cout <<" ConstructProcess for G4StepLimiter being run"<<std::endl;
  	theParticleIterator->reset();
  	while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
    	G4ProcessManager* pmanager = particle->GetProcessManager();
		pmanager->AddProcess(new G4StepLimiter,       -1,-1,3);
    }
}

void G4StepLimitProcess::ConstructParticle()
{
	std::cout <<" ConstructParticle for G4StepLimiter being run"<<std::endl;
}
