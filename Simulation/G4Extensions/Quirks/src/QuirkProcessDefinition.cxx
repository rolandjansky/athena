/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>

#include "globals.hh"
#include "G4ProcessManager.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

#include "Quirks/Quirk.h"
#include "Quirks/InfracolorForce.h"
#include "Quirks/QuirkTransportation.h"
#include "Quirks/QuirkWatcher.h"
//#include "Quirks/TestProcess.h"
#include "Quirks/QuirkStackingAction.h"
#include "Quirks/QuirkProcessDefinition.h"
#include "Quirks/DebugSteppingAction.h"

#ifndef QUIRKS_STANDALONE
#include "FadsPhysics/UserPhysicsDefinitionProxy.h"
static UserPhysicsDefinitionProxy<QuirkProcessDefinition> plugin;
#include "FadsActions/FadsSteppingAction.h"
#endif

QuirkProcessDefinition::QuirkProcessDefinition() {
    std::ifstream f;
    f.open("quirks_setup.txt");
    f >> m_mass;                m_mass *= MeV;
    f >> m_charge;              m_charge *= eplus;
    f >> m_pdgid;
    f >> m_stringForce;         m_stringForce *= MeV/mm;
    f >> m_firstStringLength;   m_firstStringLength *= mm;
    f >> m_maxBoost;
    f >> m_maxMergeT;           m_maxMergeT *= mm;
    f >> m_maxMergeMag;         m_maxMergeMag *= mm;
    f >> m_enableDebug;
    if (m_enableDebug) {
        f >> m_debugStep;           m_debugStep *= mm;
        f >> m_numDebugSteps;
    }
    f.close();
}

void QuirkProcessDefinition::ConstructParticle() {
    Quirk* q1 = new Quirk(
        /*name*/ "quirk+",    m_mass,               /*width*/ 0.0,       +m_charge,
        /*2*spin*/ 1,         /*parity*/ 0,         /*C-conjugation*/ 0,
        /*2*Isospin*/ 0,      /*2*Isospin3*/ 0,     /*G-parity*/ 0,
        /*type*/ "lepton",    /*lepton number*/ +1, /*baryon number*/ 0, +m_pdgid,
        /*stable*/ true,      /*lifetime*/ -1.0,    /*decay table*/ 0,
        /*shortlived*/ false
    );
    new Quirk(
        /*name*/ "quirk-",    m_mass,               /*width*/ 0.0,       -m_charge,
        /*2*spin*/ 1,         /*parity*/ 0,         /*C-conjugation*/ 0,
        /*2*Isospin*/ 0,      /*2*Isospin3*/ 0,     /*G-parity*/ 0,
        /*type*/ "lepton",    /*lepton number*/ -1, /*baryon number*/ 0, -m_pdgid,
        /*stable*/ true,      /*lifetime*/ -1.0,    /*decay table*/ 0,
        /*shortlived*/ false
    );

    InfracolorForce& s = q1->GetStringIn();
    s.SetStringForce(m_stringForce);
    s.SetFirstStringLength(m_firstStringLength);
    s.SetMaxBoost(m_maxBoost);
    s.SetMaxMergeT(m_maxMergeT);
    s.SetMaxMergeMag(m_maxMergeMag);
}

void QuirkProcessDefinition::ConstructProcess() {
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
        G4ParticleDefinition* particle = theParticleIterator->value();
        if (dynamic_cast<Quirk*>(particle) != 0) {
            G4ProcessManager* pmanager = particle->GetProcessManager();
            //G4cout << "# processes = " << pmanager->GetProcessListLength() << G4endl;
            //for (int i = 0; i < pmanager->GetProcessList()->size(); i++) {
            //    G4cout << "process i: " << (*pmanager->GetProcessList())[i]->GetProcessName() << G4endl;
            //}
            while (pmanager->GetProcessListLength() != 0) pmanager->RemoveProcess(0);
            G4VProcess* aTransportation = new QuirkTransportation;
            pmanager->AddProcess(aTransportation,           -1, 0, 0);
            pmanager->SetProcessOrderingToFirst(aTransportation, idxAlongStep);
            pmanager->SetProcessOrderingToFirst(aTransportation, idxPostStep);
            pmanager->AddProcess(new G4hMultipleScattering,-1, 1, 1);
            pmanager->AddProcess(new G4hIonisation,        -1, 2, 2);
            pmanager->AddProcess(new G4hBremsstrahlung,    -1, 3, 3);
            pmanager->AddProcess(new G4hPairProduction,    -1, 4, 4);
            //pmanager->AddProcess(new TestProcess,           -1, 1, 1);
            pmanager->AddProcess(new QuirkWatcher,          -1,-1, 5);
            //G4cout << "# processes = " << pmanager->GetProcessListLength() << G4endl;
            //for (int i = 0; i < pmanager->GetProcessList()->size(); i++) {
            //    G4cout << "process i: " << (*pmanager->GetProcessList())[i]->GetProcessName() << G4endl;
            //}
         }
    }
    G4RunManager* runManager = G4RunManager::GetRunManager();
    runManager->SetNumberOfAdditionalWaitingStacks(1);
    G4UserStackingAction* defaultStackingAction = G4EventManager::GetEventManager()->GetUserStackingAction();
    runManager->SetUserAction(new QuirkStackingAction(defaultStackingAction));

    if (m_enableDebug) {
        DebugSteppingAction* debugger = new DebugSteppingAction(m_debugStep, m_numDebugSteps);
#ifdef QUIRKS_STANDALONE
        runManager->SetUserAction(debugger);
#else
        FADS::FadsSteppingAction::GetSteppingAction()->RegisterAction(debugger);
#endif
    }
}

void QuirkProcessDefinition::SetCuts() {
}
