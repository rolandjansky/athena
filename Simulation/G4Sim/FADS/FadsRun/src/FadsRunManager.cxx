/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "FadsRun/FadsRunManager.h"
//#include "FadsRun/FadsRunManagerMessenger.h"
//#include "FadsRun/PhysicsMessenger.h"
#include "FadsActions/FadsEventAction.h"
#include "FadsActions/FadsRunAction.h"
#include "FadsActions/FadsSteppingAction.h"
#include "FadsKinematics/GeneratorCenter.h"
#include "FadsPhysics/DummyPhysicsList.h"

#include "G4Event.hh"

namespace FADS {

FadsRunManager::FadsRunManager(): G4RunManager()
{
	pl=0;
	SetUserAction(FadsRunAction::GetRunAction());
	SetUserAction(FadsEventAction::GetEventAction());
	SetUserAction(FadsSteppingAction::GetSteppingAction());
}

FadsRunManager::~FadsRunManager()
{
	exit(0);
	if (pl)
	{
		if (pl->GetPhysicsList())
		{
			SetUserInitialization(pl->GetPhysicsList());
		}
		else
		{
			SetUserInitialization(new DummyPhysicsList);
		}
	}
	else SetUserInitialization(new DummyPhysicsList);
}

void FadsRunManager::InitializeGeometry()
{
	geometryInitialized=true;
}
void FadsRunManager::SetPhysicsList(FadsPhysicsList *p)
{
	pl=p;
}
void FadsRunManager::InitializePhysics()
{
	if (pl)
	{
		if(verboseLevel>1) G4cout << "physicsList->Construct() start." << G4endl;
		pl->Construct();
	}
	else 
	{
                G4Exception("FadsRunManager", "UndefinedPhysicsList", FatalException, "PhysicsList is not defined");		
	}
	physicsInitialized = true;
}   
         
G4Event* FadsRunManager::GenerateEvent(G4int i_event)
{
  if (verboseLevel>0) {
    std::cout << "*** process #" << i_event << " event..." 
	      << std::endl;
  }
  G4Event* anEvent = new G4Event(i_event);
  
  if(storeRandomNumberStatus) {
    G4String fileN = randomNumberStatusDir + "currentEvent.rndm"; 
    CLHEP::HepRandom::saveEngineStatus(fileN);
  }  

  GeneratorCenter::GetGeneratorCenter()->GenerateAnEvent(anEvent);
  return anEvent;
}

}	// end namespace
