/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "QuirkPhysicsTool.h"

// package headers
#include "Quirk.h"
#include "InfracolorForce.h"
#include "QuirkTransportation.h"
#include "QuirkWatcher.h"
//#include "TestProcess.h"
#include "QuirkStackingAction.h"
#include "DebugSteppingAction.h"

// Geant4 headers
#include "globals.hh"
#include "G4ProcessManager.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

// STL headers
#include <iostream>
#include <fstream>


//-----------------------------------------------------------------------------
// Implementation file for class : QuirksPhysicsTool
//
// 15-05-2015 : Edoardo Farina
//-----------------------------------------------------------------------------
#include "G4Version.hh"
#if G4VERSION_NUMBER > 1009
#define PARTICLEITERATOR aParticleIterator
#else
#define PARTICLEITERATOR theParticleIterator
#endif

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
QuirksPhysicsTool::QuirksPhysicsTool( const std::string& type,
                                          const std::string& nam,const IInterface* parent )
  : base_class ( type, nam , parent )
{
 std::ifstream f;
    f.open("quirks_setup.txt");
    f >> m_mass;                m_mass *= CLHEP::MeV;
    f >> m_charge;              m_charge *= CLHEP::eplus;
    f >> m_pdgid;
    f >> m_stringForce;         m_stringForce *= CLHEP::MeV/CLHEP::mm;
    f >> m_firstStringLength;   m_firstStringLength *= CLHEP::mm;
    f >> m_maxBoost;
    f >> m_maxMergeT;           m_maxMergeT *= CLHEP::mm;
    f >> m_maxMergeMag;         m_maxMergeMag *= CLHEP::mm;
    f >> m_enableDebug;
    if (m_enableDebug) {
        f >> m_debugStep;           m_debugStep *= CLHEP::mm;
        f >> m_numDebugSteps;
    }
    f.close();
}

//=============================================================================
// Destructor
//=============================================================================

QuirksPhysicsTool::~QuirksPhysicsTool()
{

}

//=============================================================================
// Initialize
//=============================================================================
StatusCode QuirksPhysicsTool::initialize( )
{
  ATH_MSG_DEBUG("QuirksPhysicsTool initialize( )");
  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}


QuirksPhysicsTool* QuirksPhysicsTool::GetPhysicsOption()
{
  return this;
}

void QuirksPhysicsTool::ConstructParticle()
{
  ATH_MSG_DEBUG("ConstructParticle for the Quirks being run");

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

void QuirksPhysicsTool::ConstructProcess()
{
  ATH_MSG_DEBUG(" ConstructProcess for Quirks being run");
    PARTICLEITERATOR->reset();
    while( (*PARTICLEITERATOR)() ){
        G4ParticleDefinition* particle = PARTICLEITERATOR->value();
        if (dynamic_cast<Quirk*>(particle) != 0) {
            G4ProcessManager* pmanager = particle->GetProcessManager();
            //G4cout << "# processes = " << pmanager->GetProcessListLength() << G4endl;
            //for (int i = 0; i < pmanager->GetProcessList()->size(); i++) {
            //    G4cout << "process i: " << (*pmanager->GetProcessList())[i]->GetProcessName() << G4endl;
            //}
            while (pmanager->GetProcessListLength() != 0) pmanager->RemoveProcess(0);
            G4VProcess* aTransportation = new QuirkTransportation;
            pmanager->AddProcess(aTransportation);
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
    ATH_MSG_WARNING("This tool is interfering with the normal G4Atlas workflow, by forcing a differentcustom StackingAction instead of the default one. This is acceptable only as a temporary solution, and should be fixed asap.");
    runManager->SetUserAction(new QuirkStackingAction(defaultStackingAction));

    // ADS this must be done on the python side
 //f (m_enableDebug) {
 //   DebugSteppingAction* debugger = new DebugSteppingAction(m_debugStep, m_numDebugSteps);
    //#//f QUIRKS_STANDALONE
 //   runManager->SetUserAction(debugger);
    //#//
 //   FADS::FadsSteppingAction::GetSteppingAction()->RegisterAction(debugger);
    //#//f
 //
}
