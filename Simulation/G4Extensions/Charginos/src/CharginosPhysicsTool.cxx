/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files

// local
#include "CharginosPhysicsTool.h"
#include "AMSBCharginoPlus.hh"
#include "AMSBCharginoMinus.hh"
#include "AMSBNeutralino.hh"

// Geant4 physics lists
#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"
#include "G4hhIonisation.hh"
#include "G4Transportation.hh"
#include "G4MuIonisation.hh"
#include "G4ProcessManager.hh"
#include "G4DecayTable.hh"
#include "G4VDecayChannel.hh"
#include "G4PhaseSpaceDecayChannel.hh"

// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CharginosPhysicsTool
//
// 15-05-2015 : Edoardo Farina
//-----------------------------------------------------------------------------



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CharginosPhysicsTool::CharginosPhysicsTool( const std::string& type,
                                            const std::string& nam,const IInterface* parent )
  : AthAlgTool ( type, nam , parent )
{
  ATH_MSG_DEBUG("CharginosPhysicsTool "<<type<<" "<<nam);
  declareInterface< IPhysicsOptionTool >( this ) ;

  declareProperty("CharginoPlusMass",CharginoPlusMass= 101.0*CLHEP::GeV,"CharginoPlus Mass");
  declareProperty("CharginoPlusWidth",CharginoPlusWidth=0.0*CLHEP::MeV,"CharginoPlus Width");
  declareProperty("CharginoPlusCharge",CharginoPlusCharge=+1.*CLHEP::eplus,"CharginoPlus charge");
  declareProperty("CharginoPlusPDGCode",CharginoPlusPDGCode=1000024,"CharginoPlus PDG CODE");
  declareProperty("CharginoPlusStable",CharginoPlusStable=true,"CharginoPlus Stable");
  declareProperty("CharginoPlusLifetime",CharginoPlusLifetime=-1,"CharginoPlus Lifetime");
  declareProperty("CharginoPlusShortlived",CharginoPlusShortlived=false,"CharginoPlus Shortlived");

  declareProperty("CharginoMinusMass",CharginoMinusMass= 101.0*CLHEP::GeV,"CharginoMinus Mass");
  declareProperty("CharginoMinusWidth",CharginoMinusWidth=0.0*CLHEP::MeV,"CharginoMinus Width");
  declareProperty("CharginoMinusCharge",CharginoMinusCharge=-1.*CLHEP::eplus,"CharginoMinus charge");
  declareProperty("CharginoMinusPDGCode",CharginoMinusPDGCode=-1000024,"CharginoMinus PDG CODE");
  declareProperty("CharginoMinusStable",CharginoMinusStable=true,"CharginoMinus Stable");
  declareProperty("CharginoMinusLifetime",CharginoMinusLifetime=-1,"CharginoMinus Lifetime");
  declareProperty("CharginoMinusShortlived",CharginoMinusShortlived=false,"CharginoMinus Shortlived");

  declareProperty("NeutralinoMass",NeutralinoMass= 100.0*CLHEP::GeV,"Neutralino Mass");
  declareProperty("NeutralinoWidth",NeutralinoWidth=0.0*CLHEP::MeV,"Neutralino Width");
  declareProperty("NeutralinoCharge",NeutralinoCharge=0.*CLHEP::eplus,"Neutralino charge");
  declareProperty("NeutralinoPDGCode",NeutralinoPDGCode=1000022,"Neutralino PDG CODE");
  declareProperty("NeutralinoStable",NeutralinoStable=true,"Neutralino Stable");
  declareProperty("NeutralinoLifetime",NeutralinoLifetime=-1,"Neutralino Lifetime");
  declareProperty("NeutralinoShortlived",NeutralinoShortlived=false,"Neutralino Shortlived");

}

//=============================================================================
// Destructor
//=============================================================================

CharginosPhysicsTool::~CharginosPhysicsTool()
{

}

//=============================================================================
// Initialize
//=============================================================================
StatusCode CharginosPhysicsTool::initialize( )
{
  ATH_MSG_DEBUG("CharginosPhysicsTool::initialize( )");
  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}

CharginosPhysicsTool* CharginosPhysicsTool::GetPhysicsOption()
{
  return this;
}

void CharginosPhysicsTool::ConstructParticle()
{
  ATH_MSG_DEBUG(" ConstructParticle for the Charginos being run");

  theCharginoPlus = AMSBCharginoPlus::Definition(CharginoPlusMass, CharginoPlusWidth, CharginoPlusCharge, CharginoPlusPDGCode, CharginoPlusStable, CharginoPlusLifetime, CharginoPlusShortlived );

  theCharginoMinus = AMSBCharginoMinus::Definition(CharginoMinusMass, CharginoMinusWidth, CharginoMinusCharge, CharginoMinusPDGCode, CharginoMinusStable, CharginoMinusLifetime, CharginoMinusShortlived );

  theNeutralino = AMSBNeutralino::Definition(NeutralinoMass, NeutralinoWidth, NeutralinoCharge, NeutralinoPDGCode, NeutralinoStable, NeutralinoLifetime, NeutralinoShortlived );

}

void CharginosPhysicsTool::ConstructProcess()
{
  ATH_MSG_DEBUG(" ConstructProcess for Charginos being run");

  G4hhIonisation *theIonisation = new G4hhIonisation;
  G4Transportation *theTransportation=new G4Transportation;

  G4ProcessManager *charginoPlus= theCharginoPlus->GetProcessManager();
  G4ProcessManager *charginoMinus=theCharginoMinus->GetProcessManager();

  charginoPlus->AddProcess(theTransportation);
  charginoMinus->AddProcess(theTransportation);
  charginoPlus->AddProcess(theIonisation,-1,2,2);
  charginoMinus->AddProcess(theIonisation,-1,2,2);
}
