/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "GauginosPhysicsTool.h"
// package headers
#include "GMSBNeutralino.hh"
#include "GMSBGravitino.hh"
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
// Implementation file for class : GauginosPhysicsTool
//
// 15-05-2015 : Edoardo Farina
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GauginosPhysicsTool::GauginosPhysicsTool( const std::string& type,
                                          const std::string& nam,const IInterface* parent )
  : base_class ( type, nam , parent )
{
  declareProperty("GravitinoMass",GravitinoMass=0.108E-04*CLHEP::GeV,"Gravitino Mass");
  declareProperty("GravitinoWidth",GravitinoWidth=0.*CLHEP::GeV,"Gravitino Width");
  declareProperty("GravitinoCharge",GravitinoCharge=0,"Gravitino charge");
  declareProperty("GravitinoPDGCode",GravitinoPDGCode=1000039,"Gravitino PDG CODE");
  declareProperty("GravitinoLifetime",GravitinoLifetime=-1 ,"Gravitino Lifetime");
  declareProperty("GravitinoStable",GravitinoStable=true ,"Gravitino Stable");
  declareProperty("GravitinoShortlived",GravitinoShortlived=false ,"Gravitino Shortlived");

  declareProperty("NeutralinoMass",NeutralinoMass=118.848*CLHEP::GeV,"Neutralino Mass");
  declareProperty("NeutralinoWidth",NeutralinoWidth=0.*CLHEP::GeV,"Neutralino Width");
  declareProperty("NeutralinoCharge",NeutralinoCharge=0,"Neutralino charge");
  declareProperty("NeutralinoPDGCode",NeutralinoPDGCode=1000022,"Neutralino PDG CODE");
  declareProperty("NeutralinoStable",NeutralinoStable=true,"Neutralino Stable");
  declareProperty("NeutralinoLifetime",NeutralinoLifetime=-1,"Neutralino Lifetime");
  declareProperty("NeutralinoShortlived",NeutralinoShortlived=false,"Neutralino Shortlived");

}

//=============================================================================
// Destructor
//=============================================================================

GauginosPhysicsTool::~GauginosPhysicsTool()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode GauginosPhysicsTool::initialize( )
{
  ATH_MSG_DEBUG("GauginosPhysicsTool initialize(  )");

  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}


GauginosPhysicsTool* GauginosPhysicsTool::GetPhysicsOption()
{
  return this;
}

void GauginosPhysicsTool::ConstructParticle()
{
  ATH_MSG_DEBUG("Create particle of Gauginos" );

  GMSBNeutralino::Definition(NeutralinoMass, NeutralinoWidth, NeutralinoCharge, NeutralinoPDGCode, NeutralinoStable, NeutralinoLifetime, NeutralinoShortlived );

  GMSBGravitino::Definition(GravitinoMass, GravitinoWidth, GravitinoCharge, GravitinoPDGCode, GravitinoStable, GravitinoLifetime, NeutralinoShortlived);

}

void GauginosPhysicsTool::ConstructProcess()
{
  ATH_MSG_DEBUG(" Construct Process for the Gauginos being run");
}
