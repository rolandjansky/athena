/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4FastSimulation/FastSimProcessDef.h"

#include "G4FastSimulationManagerProcess.hh"
#include "globals.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleDefinition.hh"

//For Message Service:
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <iostream>

#include "FadsPhysics/UserPhysicsDefinitionProxy.h"
static UserPhysicsDefinitionProxy<FastSimProcessDef> plugin;

FastSimProcessDef::FastSimProcessDef(): UserPhysicsDefinition()
{
}

void FastSimProcessDef::ConstructProcess()
{
  ISvcLocator * svcLocator = Gaudi::svcLocator(); 
  IMessageSvc * msgSvc = 0;
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  if (status.isFailure()) 
    std::cerr << "TRT_TR_Process::FastSimProcessDef : Couldn't locate message service!" << std::endl;
  MsgStream log(msgSvc, "G4FastSimProcessDef");

  log << MSG::DEBUG << " This is FastSimProcessDef::ConstructProcess()" << endreq;

  G4FastSimulationManagerProcess* 
    theFastSimulationManagerProcess = new G4FastSimulationManagerProcess();
  
  theParticleIterator->reset();
  
  while( (*theParticleIterator)() )
  {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    pmanager->AddProcess(theFastSimulationManagerProcess, -1, 1, 1);
  }
}
