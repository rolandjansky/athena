/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "DarkSectorPhysicsTool.h"
// package headers
#include "DarkSectorLSP.hh"
// Geant4 physics lists
#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"
#include "G4ParticleDefinition.hh"
#include "G4Transportation.hh"
#include "G4MuIonisation.hh"
// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DarkSectorPhysicsTool
//
// 17-02-2017 : ADA
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DarkSectorPhysicsTool::DarkSectorPhysicsTool( const std::string& type,
                                              const std::string& nam,const IInterface* parent )
  : AthAlgTool ( type, nam , parent )
{
  ATH_MSG_DEBUG("DarkSectorPhysicsTool "<<type<<" "<<nam);
  declareInterface< IPhysicsOptionTool >( this ) ;
  
  declareProperty("DarkSectorLSPMass",DarkSectorLSPMass=2000.*CLHEP::GeV,"Dark Sector LSP Mass");

}

//=============================================================================
// Destructor
//=============================================================================

DarkSectorPhysicsTool::~DarkSectorPhysicsTool()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode DarkSectorPhysicsTool::initialize( )
{
  ATH_MSG_DEBUG("DarkSectorPhysicsTool initialize(  )");

  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}


DarkSectorPhysicsTool* DarkSectorPhysicsTool::GetPhysicsOption()
{
  return this;
}

void DarkSectorPhysicsTool::ConstructParticle()
{
  ATH_MSG_DEBUG("Create particle of DarkSector" );

  DarkSectorLSP::Definition(DarkSectorLSPMass,0,0,3000015,1,-1,0);
}

void DarkSectorPhysicsTool::ConstructProcess()
{
  ATH_MSG_DEBUG(" Construct Process for the Dark Sector being run");
}
