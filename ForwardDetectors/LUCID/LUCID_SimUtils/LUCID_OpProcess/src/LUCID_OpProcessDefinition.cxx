/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_OpProcess/LUCID_OpProcessDefinition.h"

#include "FadsPhysics/UserPhysicsDefinition.h"
#include "FadsPhysics/UserPhysicsDefinitionProxy.h"

#include "G4VProcess.hh"
#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include <iostream>
#include "globals.hh"

static UserPhysicsDefinitionProxy<LUCID_OpProcessDefinition> plg;

void LUCID_OpProcessDefinition::ConstructProcess() {

  IMessageSvc* msgSvc = NULL;
  StatusCode   status = Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  
  MsgStream log(msgSvc, "LUCID_OpProcessDefinition");
  
  if (status.isFailure()) log << MSG::FATAL << "Couldn't locate message service!" << endreq;
  else                    log << MSG::DEBUG << "Message service located."         << endreq;
  
  log << MSG::DEBUG << " ConstructProcess()" << endreq;

  G4Cerenkov*          theCerenkovProcess      = new G4Cerenkov         ("Cerenkov");
  G4Scintillation*     theScintillationProcess = new G4Scintillation    ("Scintillation");
  G4OpAbsorption*      theOpAbsorptionProcess  = new G4OpAbsorption     ();
  G4OpRayleigh*        theOpRayleighProcess    = new G4OpRayleigh       ();
  G4OpBoundaryProcess* theOpBoundaryProcess    = new G4OpBoundaryProcess();
  
  theCerenkovProcess->SetVerboseLevel(0);
  theScintillationProcess->SetVerboseLevel(0);
  theOpAbsorptionProcess->SetVerboseLevel(0);
  theOpRayleighProcess->SetVerboseLevel(0);
  theOpBoundaryProcess->SetVerboseLevel(0);
  
  G4int MaxNumPhotons = 300;

  theCerenkovProcess->SetTrackSecondariesFirst(true);
  theCerenkovProcess->SetMaxNumPhotonsPerStep(MaxNumPhotons);

  theScintillationProcess->SetTrackSecondariesFirst(true);
  theScintillationProcess->SetScintillationYieldFactor(1.);

  //theOpBoundaryProcess->SetModel(unified); //Obsolete method with no effect - removed in G4 9.6
  
  theParticleIterator->reset();

  while ((*theParticleIterator)()) {

    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager*     pmanager = particle->GetProcessManager();
    G4String          particleName = particle->GetParticleName();

    if (theCerenkovProcess->IsApplicable(*particle)) {

      G4cout << " Adding CerenkovProcess to: " << particleName << G4endl;

      pmanager->AddProcess        (theCerenkovProcess);
      pmanager->SetProcessOrdering(theCerenkovProcess, idxPostStep);
    }

    if (theScintillationProcess->IsApplicable(*particle)) {

      G4cout << " Adding ScintillationProcess to: " << particleName << G4endl;

      pmanager->AddProcess              (theScintillationProcess);
      pmanager->SetProcessOrderingToLast(theScintillationProcess, idxAtRest);
      pmanager->SetProcessOrderingToLast(theScintillationProcess, idxPostStep);
    }

    if (particleName == "opticalphoton") {

      G4cout << " Adding OpAbsorptionProcess, OpRayleighProcess and OpBoundaryProcess to opticalphoton " << G4endl;

      pmanager->AddDiscreteProcess(theOpAbsorptionProcess);
      pmanager->AddDiscreteProcess(theOpRayleighProcess);
      pmanager->AddDiscreteProcess(theOpBoundaryProcess);
    }
  }
}
