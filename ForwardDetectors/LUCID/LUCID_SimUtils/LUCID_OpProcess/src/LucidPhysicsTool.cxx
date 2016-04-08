/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// local
#include "LucidPhysicsTool.h"

#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"
#include "G4VProcess.hh"
#include "G4Version.hh"


#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"

#if G4VERSION_NUMBER > 1009
#define PARTICLEITERATOR aParticleIterator
#else
#define PARTICLEITERATOR theParticleIterator
#endif

//-----------------------------------------------------------------------------
// Implementation file for class : LucidPhysicsTool
//
// 18-05-2015 : Edoardo Farina
//-----------------------------------------------------------------------------



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LucidPhysicsTool::LucidPhysicsTool( const std::string& type,
                                    const std::string& nam,const IInterface* parent )
  : G4VPhysicsConstructor(nam), AthAlgTool ( type, nam , parent )
{
  ATH_MSG_INFO("LucidPhysicsTool "<<type<<" "<<nam);
  declareInterface< IPhysicsOptionTool >( this ) ;
}

//=============================================================================
// Destructor
//=============================================================================

LucidPhysicsTool::~LucidPhysicsTool()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode LucidPhysicsTool::initialize( )
{
  ATH_MSG_INFO("LucidPhysicsTool initialize( )");
  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}

G4VPhysicsConstructor* LucidPhysicsTool::GetPhysicsOption()
{
  return this;
}


void LucidPhysicsTool::ConstructParticle()
{

}
void LucidPhysicsTool::ConstructProcess()
{
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


  PARTICLEITERATOR->reset();

  while ((*PARTICLEITERATOR)())
    {

      G4ParticleDefinition* particle = PARTICLEITERATOR->value();
      G4ProcessManager*     pmanager = particle->GetProcessManager();
      G4String          particleName = particle->GetParticleName();

      if (theCerenkovProcess->IsApplicable(*particle)) {
        ATH_MSG_DEBUG( " Adding CerenkovProcess to: " << particleName );
        pmanager->AddProcess        (theCerenkovProcess);
        pmanager->SetProcessOrdering(theCerenkovProcess, idxPostStep);
      }

      if (theScintillationProcess->IsApplicable(*particle)) {
        ATH_MSG_DEBUG( " Adding ScintillationProcess to: " << particleName );
        pmanager->AddProcess              (theScintillationProcess);
        pmanager->SetProcessOrderingToLast(theScintillationProcess, idxAtRest);
        pmanager->SetProcessOrderingToLast(theScintillationProcess, idxPostStep);
      }

      if (particleName == "opticalphoton") {

        ATH_MSG_DEBUG(" Adding OpAbsorptionProcess, OpRayleighProcess and OpBoundaryProcess to opticalphoton " );
        pmanager->AddDiscreteProcess(theOpAbsorptionProcess);
        pmanager->AddDiscreteProcess(theOpRayleighProcess);
        pmanager->AddDiscreteProcess(theOpBoundaryProcess);
      }
    }

}
