/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISFFluxRecorder.h"

#include "G4Event.hh"
#include "G4GeometryManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParallelWorldScoringProcess.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4RegionStore.hh"
#include "G4Run.hh"
#include "G4ScoringManager.hh"
#include "G4StateManager.hh"
#include "G4TransportationManager.hh"
#include "G4UImanager.hh"
#include "G4UserRunAction.hh"
#include "G4Version.hh"

void ISFFluxRecorder::InitializeFluxRecording()
{
  G4UImanager *ui = G4UImanager::GetUIpointer();
  ui->ApplyCommand("/run/setCutForAGivenParticle proton 0 mm");

  G4ScoringManager* ScM = G4ScoringManager::GetScoringManagerIfExist();

  if(!ScM) { return; }

  ui->ApplyCommand("/score/create/cylinderMesh cylMesh_1");
  //                        R  Z(-24 to 24)
  ui->ApplyCommand("/score/mesh/cylinderSize 12. 24. m");
  //                iR iZ
  //ui->ApplyCommand("/score/mesh/nBin 1 1 1");
  ui->ApplyCommand("/score/mesh/nBin 120 480 1");

  ui->ApplyCommand("/score/quantity/energyDeposit eDep");

  ui->ApplyCommand("/score/quantity/cellFlux CF_photon");
  ui->ApplyCommand("/score/filter/particle photonFilter gamma");
  // above 2 line crete tally for cell flux for gamma

  ui->ApplyCommand("/score/quantity/cellFlux CF_neutron");
  ui->ApplyCommand("/score/filter/particle neutronFilter neutron");

  ui->ApplyCommand("/score/quantity/cellFlux CF_HEneutron");
  ui->ApplyCommand("/score/filter/particleWithKineticEnergy HEneutronFilter 20 7000000 MeV neutron");

  ui->ApplyCommand("/score/quantity/doseDeposit dose");

  ui->ApplyCommand("/score/close");
  ui->ApplyCommand("/score/list");

  G4int nPar = ScM->GetNumberOfMesh();

  if(nPar<1) { return; }

  G4ParticleTable::G4PTblDicIterator* particleIterator
    = G4ParticleTable::GetParticleTable()->GetIterator();

  for(G4int iw=0;iw<nPar;iw++) {
    G4VScoringMesh* mesh = ScM->GetMesh(iw);
    G4VPhysicalVolume* pWorld
      = G4TransportationManager::GetTransportationManager()
      ->IsWorldExisting(ScM->GetWorldName(iw));
      if(!pWorld) {
        pWorld = G4TransportationManager::GetTransportationManager()
          ->GetParallelWorld(ScM->GetWorldName(iw));
        pWorld->SetName(ScM->GetWorldName(iw));

        G4ParallelWorldScoringProcess* theParallelWorldScoringProcess
          = new G4ParallelWorldScoringProcess(ScM->GetWorldName(iw));
        theParallelWorldScoringProcess->SetParallelWorld(ScM->GetWorldName(iw));

        particleIterator->reset();
        while( (*particleIterator)() ) {
          G4ParticleDefinition* particle = particleIterator->value();
          G4ProcessManager* pmanager = particle->GetProcessManager();
          if(pmanager) {
            pmanager->AddProcess(theParallelWorldScoringProcess);
            pmanager->SetProcessOrderingToLast(theParallelWorldScoringProcess, idxAtRest);
            pmanager->SetProcessOrderingToSecond(theParallelWorldScoringProcess, idxAlongStep);
            pmanager->SetProcessOrderingToLast(theParallelWorldScoringProcess, idxPostStep);
          }
        }
      }

      mesh->Construct(pWorld);
  }
  return;
}

void ISFFluxRecorder::RecordFlux(const G4Event* currentEvent)
{
  G4ScoringManager* ScM = G4ScoringManager::GetScoringManagerIfExist();
  if(ScM) {
    G4int nPar = ScM->GetNumberOfMesh();
    G4HCofThisEvent* HCE = currentEvent->GetHCofThisEvent();
    if(HCE && nPar>0) {
      G4int nColl = HCE->GetCapacity();
      for(G4int i=0;i<nColl;i++) {
        G4VHitsCollection* HC = HCE->GetHC(i);
        if(HC) { ScM->Accumulate(HC); }
      }
    }
  }
  return;
}

void ISFFluxRecorder::WriteFluxInformation()
{
  G4UImanager *ui=G4UImanager::GetUIpointer();
  ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 eDep edep.txt");
  ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 CF_neutron neutron.txt");
  ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 CF_HEneutron HEneutron.txt");
  ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 CF_photon photon.txt");
  ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 dose dose.txt");
  return;
}
