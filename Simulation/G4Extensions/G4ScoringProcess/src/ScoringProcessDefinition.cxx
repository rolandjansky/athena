/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4ParallelWorldScoringProcess.hh"
#include "G4ScoringProcess/ScoringProcessDefinition.h"
#include "G4ProcessManager.hh"

#include "FadsPhysics/UserPhysicsDefinitionProxy.h"
static UserPhysicsDefinitionProxy<ScoringProcessDefinition> proxy;

ScoringProcessDefinition::ScoringProcessDefinition(): 
  UserPhysicsDefinition()
{	
  std::cout <<" physics list for the Scoring process being built "<<std::endl;
}

void ScoringProcessDefinition::ConstructParticle()
{

}


void ScoringProcessDefinition::ConstructProcess()
{
  // Add parallel world scoring process
  std::cout<<" ScoringProcessDefinition::ConstructProcess()"<<std::endl;

  G4ParallelWorldScoringProcess* theParallelWorldScoringProcess 
      = new G4ParallelWorldScoringProcess("ParaWorldScoringProc");
  theParallelWorldScoringProcess->SetParallelWorld("Scoring");

  theParticleIterator->reset();
  while( (*theParticleIterator)() )
  {
    G4ParticleDefinition* particle = theParticleIterator->value();
    if (!particle->IsShortLived())
    {
      G4ProcessManager* pmanager = particle->GetProcessManager();
      pmanager->AddProcess(theParallelWorldScoringProcess);
      pmanager->SetProcessOrderingToLast(theParallelWorldScoringProcess, idxAtRest);
      pmanager->SetProcessOrdering(theParallelWorldScoringProcess, idxAlongStep, 1);
      pmanager->SetProcessOrderingToLast(theParallelWorldScoringProcess, idxPostStep);
    }
  }
}
