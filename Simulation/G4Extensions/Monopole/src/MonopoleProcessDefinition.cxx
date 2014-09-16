/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Monopole/CustomMonopole.h"
#include "Monopole/CustomMonopoleFactory.h"
#include "Monopole/MonopoleProcessDefinition.h"
#include "FadsPhysics/UserPhysicsDefinitionProxy.h"

#include "Monopole/G4mplAtlasTransportation.h"
//  #include "G4hhIonisation.hh"

//#include "Monopole/G4mplIonisation.hh"
#include "Monopole/G4mplAtlasIonisation.hh"
#include "G4Transportation.hh"
//#include "G4MultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4ProcessManager.hh"
#include "G4hIonisation.hh"

static UserPhysicsDefinitionProxy<MonopoleProcessDefinition> plugin; 

MonopoleProcessDefinition::MonopoleProcessDefinition(): 
	UserPhysicsDefinition()
{	
	std::cout <<"!!! physics list for the Monopole being built!!! "<<std::endl;
}

void MonopoleProcessDefinition::ConstructProcess()
{
    std::cout <<"!!! ConstructProcess for Monopole being run!!!"<<std::endl;
	
  // Add standard EM Processes for gamma
  // G4cout << "G4EmMonopoleBuilder::ConstructProcess" << G4endl;

    theParticleIterator->reset();
    
    while((*theParticleIterator)())
      {
	//	G4ParticleDefinition* particle = theParticleIterator->value();
	//	CustomMonopole* particle = theParticleIterator->value();
// 	G4cout << "Particle::" << theParticleIterator->value()->GetParticleName() << "::" << G4endl;
	CustomMonopole* particle = dynamic_cast<CustomMonopole*>(theParticleIterator->value());
	if(CustomMonopoleFactory::isCustomMonopole(particle))
	  {
	    G4cout << particle->GetParticleName() << " is Custom" << G4endl;
	    
	    G4ProcessManager* pmanager = particle->GetProcessManager();
	    
	    G4double magnCharge = particle->MagneticCharge();

	    //	    if (magnCharge != 0.0){
	      pmanager->RemoveProcess(0);
	      
	      //std::cout <<"SB: !!! ConstructProcess for Monopole: G4mplAtlasTransportation!!!"<<std::endl;
	      pmanager->AddProcess(new G4mplAtlasTransportation(particle), -1, 0, 0);
	      
	      //std::cout <<"SB: !!! ConstructProcess for Monopole: G4mplAtlasIonisation!!!"<<std::endl;	    
	      //G4mplAtlasIonisation* mplIon = new G4mplAtlasIonisation(magnCharge, G4String("mplAtlasIonisation"));
	      //mplIon->PrintInfo();
	      //	    pmanager->AddProcess(mplIon,  -1, 1, 1);
	      
	      if (magnCharge != 0.0){
		pmanager->AddProcess(new G4mplAtlasIonisation(magnCharge, G4String("mplAtlasIonisation")), -1, 1, 1);
		//std::cout <<"SB: !!! ConstructProcess for Monopole: G4mplAtlasIonisation done!!!"<<std::endl;
	      }
	      //	    }

	    if(particle->GetPDGCharge() != 0.0) {
	      pmanager->AddProcess(new G4hIonisation(),  -1, 2, 2);
	    }
	    pmanager->DumpInfo();
	    //	    std::cout <<"SB: ConstructProcess for Monopole finished!!!"<<std::endl;
	    
	  }
      }
    
//     CustomMonopole* mpl = CustomMonopole::MonopoleDefinition();
//     G4ProcessManager* pmanager = mpl->GetProcessManager();
//     pmanager->RemoveProcess(0);

//     G4double magnCharge = mpl->MagneticCharge();

//     std::cout <<"SB: !!! ConstructProcess for Monopole: G4mplAtlasTransportation!!!"<<std::endl;
//     pmanager->AddProcess(new G4mplAtlasTransportation(mpl), -1, 0, 0);

//     std::cout <<"SB: !!! ConstructProcess for Monopole: G4mplAtlasIonisation!!!"<<std::endl;
    
//     G4mplAtlasIonisation* mplIon = new G4mplAtlasIonisation(magnCharge, G4String("mplAtlasIonisation"));
    
//     std::cout <<"SB: !!! ConstructProcess for Monopole: G4mplAtlasIonisation PrintInfo!!!"<<std::endl;
//     mplIon->PrintInfo();
//     std::cout <<"SB: !!! ConstructProcess for Monopole: G4mplAtlasIonisation PrintInfo done!!!"<<std::endl;
    
//     pmanager->AddProcess(mplIon,  -1, 1, 1);
//     std::cout <<"SB: !!! ConstructProcess for Monopole: G4mplAtlasIonisation done!!!"<<std::endl;


//     if(mpl->GetPDGCharge() != 0.0) {
//       pmanager->AddProcess(new G4hIonisation(),  -1, 2, 2);
//     }
//     pmanager->DumpInfo();
    std::cout <<"!!! ConstructProcess for Monopole finished!!!"<<std::endl;
}

void MonopoleProcessDefinition::ConstructParticle()
{
    std::cout <<"!!! ConstructParticle for the Monopole being run!!!"<<std::endl;
    //    CustomMonopole* mpl __attribute__ ((unused)) = CustomMonopole::MonopoleDefinition();
    CustomMonopoleFactory::loadCustomMonopoles();
}
