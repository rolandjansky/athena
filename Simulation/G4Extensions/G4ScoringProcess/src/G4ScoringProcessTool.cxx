/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "G4ScoringProcessTool.h"
// package headers
#include "G4ParallelWorldScoringProcess.hh"
#include "G4ProcessManager.hh"
// Geant4 headers
#include "G4ProcessManager.hh"
#include "G4Version.hh"
//CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"


//-----------------------------------------------------------------------------
// Implementation file for class : G4ScoringProcessTool
//
// 26-10-2015 : Edoardo Farina
//-----------------------------------------------------------------------------

#if G4VERSION_NUMBER > 1009
#define PARTICLEITERATOR aParticleIterator
#else
#define PARTICLEITERATOR theParticleIterator
#endif

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4ScoringProcessTool::G4ScoringProcessTool( const std::string& type,
                                          const std::string& nam,const IInterface* parent )
  : AthAlgTool ( type, nam , parent )
{
  ATH_MSG_DEBUG("G4ScoringProcessTool "<<type<<" "<<nam);
  declareInterface< IPhysicsOptionTool >( this ) ;

}

//=============================================================================
// Destructor
//=============================================================================

G4ScoringProcessTool::~G4ScoringProcessTool()
{

}

//=============================================================================
// Initialize
//=============================================================================
StatusCode G4ScoringProcessTool::initialize( )
{
  ATH_MSG_DEBUG("G4ScoringProcessTool initialize( )");
  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}


G4ScoringProcessTool* G4ScoringProcessTool::GetPhysicsOption()
{
  return this;
}

void G4ScoringProcessTool::ConstructParticle()
{

}

void G4ScoringProcessTool::ConstructProcess()
{
   // Add parallel world scoring process
  ATH_MSG_DEBUG(" ScoringProcessDefinition::ConstructProcess()");

  G4ParallelWorldScoringProcess* theParallelWorldScoringProcess 
      = new G4ParallelWorldScoringProcess("ParaWorldScoringProc");
  theParallelWorldScoringProcess->SetParallelWorld("Scoring");

  PARTICLEITERATOR->reset();
  while( (*PARTICLEITERATOR)() )
  {
    G4ParticleDefinition* particle = PARTICLEITERATOR->value();
    if (!particle->IsShortLived())
    {
      G4ProcessManager* pmanager = particle->GetProcessManager();
      pmanager->AddProcess(theParallelWorldScoringProcess);
      pmanager->SetProcessOrderingToLast(theParallelWorldScoringProcess, idxAtRest);
      pmanager->SetProcessOrdering(theParallelWorldScoringProcess, idxAlongStep, 1);
      pmanager->SetProcessOrderingToLast(theParallelWorldScoringProcess, idxPostStep);
    }
  }
}
