/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "G4StepLimitationTool.h"

// G4 headers
#include "G4StepLimiter.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Version.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : G4StepLimitationTool
//
// 03-08-2015 : Edoardo Farina
//-----------------------------------------------------------------------------

#if G4VERSION_NUMBER > 1009
#define PARTICLEITERATOR aParticleIterator
#else
#define PARTICLEITERATOR theParticleIterator
#endif

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4StepLimitationTool::G4StepLimitationTool( const std::string& type,
                                            const std::string& nam,const IInterface* parent )
  : base_class ( type, nam , parent )
{
}

//=============================================================================
// Destructor
//=============================================================================

G4StepLimitationTool::~G4StepLimitationTool()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode G4StepLimitationTool::initialize( )
{
  ATH_MSG_VERBOSE("G4StepLimitationTool initialize(  )");

  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}


G4VPhysicsConstructor* G4StepLimitationTool::GetPhysicsOption()
{
  return this;
}

void G4StepLimitationTool::ConstructParticle()
{



}

void G4StepLimitationTool::ConstructProcess()
{

  ATH_MSG_DEBUG(" ConstructProcess for G4StepLimiter being run");
  PARTICLEITERATOR->reset();
  while( (*PARTICLEITERATOR)() )
    {
      G4ParticleDefinition* particle = PARTICLEITERATOR->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      pmanager->AddProcess(new G4StepLimiter,       -1,-1,3);
    }
}
