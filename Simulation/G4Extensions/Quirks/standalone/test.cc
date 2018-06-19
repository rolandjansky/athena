/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4RunManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Quirks/TestDetectorConstruction.hh"
#include "Quirks/ExN03PhysicsList.hh"
#include "Quirks/TestPrimaryGeneratorAction.hh"

int main(int argc, char** argv) {
    G4RunManager* runManager = new G4RunManager;
    G4VUserDetectorConstruction* detector = new TestDetectorConstruction;
    runManager->SetUserInitialization(detector);
    G4VUserPhysicsList* physics = new ExN03PhysicsList;
    runManager->SetUserInitialization(physics);
    G4VUserPrimaryGeneratorAction* gen_action = new TestPrimaryGeneratorAction;
    runManager->SetUserAction(gen_action);

    runManager->Initialize();

    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    ui->SessionStart();

    delete ui;
    delete runManager;

    return 0;
}
