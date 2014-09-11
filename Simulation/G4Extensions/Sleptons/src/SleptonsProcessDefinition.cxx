/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Sleptons/SleptonsProcessDefinition.h"

#include "Sleptons/G4SElectronMinus.hh"
#include "Sleptons/G4SElectronPlus.hh"
#include "Sleptons/G4SMuonMinus.hh"
#include "Sleptons/G4SMuonPlus.hh"
#include "Sleptons/G4STauMinus.hh"
#include "Sleptons/G4STauPlus.hh"

#include "FadsPhysics/UserPhysicsDefinitionProxy.h"

#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"

#include "G4Transportation.hh"
#include "G4MuIonisation.hh"
#include "G4ProcessManager.hh"

static UserPhysicsDefinitionProxy<SleptonsProcessDefinition> plugin;

SleptonsProcessDefinition::SleptonsProcessDefinition()
  : UserPhysicsDefinition(),
    theSElectronMinus(0), theSElectronPlus(0), theSMuonMinus(0), theSMuonPlus(0), theSTauMinus(0), theSTauPlus(0)
{
  std::cout <<" physics list for the Sleptons being built "<<std::endl;
}

void SleptonsProcessDefinition::ConstructProcess()
{
  std::cout <<" ConstructProcess for Sleptons being run"<<std::endl;
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    int pdgcode=std::abs(particle->GetPDGEncoding());
    if (pdgcode==2000011 || pdgcode==2000013 || pdgcode==1000015)
    {
  	  G4ProcessManager* proc=particle->GetProcessManager();
	  proc->AddProcess(new G4hMultipleScattering,-1,1,1);
	  proc->AddProcess(new G4hIonisation,-1,2,2);
    }
  }


}

void SleptonsProcessDefinition::ConstructParticle()
{
  std::cout << " ConstructParticle for the Sleptons being run" << std::endl;

  theSElectronMinus = G4SElectronMinus::Definition();
  theSElectronPlus  = G4SElectronPlus::Definition();

  theSMuonMinus     = G4SMuonMinus::Definition();
  theSMuonPlus      = G4SMuonPlus::Definition();

  theSTauMinus=G4STauMinus::Definition();
  theSTauPlus=G4STauPlus::Definition();
}
