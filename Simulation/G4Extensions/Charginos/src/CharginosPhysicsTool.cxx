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
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"
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
                                            const std::string& name, const IInterface* parent )
  : base_class ( type, name , parent )
  , m_theCharginoMinus(nullptr)
  , m_theCharginoPlus(nullptr)
  , m_theNeutralino(nullptr)
{
  declareProperty("CharginoPlusMass",m_CharginoPlusMass= 101.0*CLHEP::GeV,"CharginoPlus Mass");
  declareProperty("CharginoPlusWidth",m_CharginoPlusWidth=0.0*CLHEP::MeV,"CharginoPlus Width");
  declareProperty("CharginoPlusCharge",m_CharginoPlusCharge=+1.*CLHEP::eplus,"CharginoPlus charge");
  declareProperty("CharginoPlusPDGCode",m_CharginoPlusPDGCode=1000024,"CharginoPlus PDG CODE");
  declareProperty("CharginoPlusStable",m_CharginoPlusStable=true,"CharginoPlus Stable");
  declareProperty("CharginoPlusLifetime",m_CharginoPlusLifetime=-1,"CharginoPlus Lifetime");
  declareProperty("CharginoPlusShortlived",m_CharginoPlusShortlived=false,"CharginoPlus Shortlived");

  declareProperty("CharginoMinusMass",m_CharginoMinusMass= 101.0*CLHEP::GeV,"CharginoMinus Mass");
  declareProperty("CharginoMinusWidth",m_CharginoMinusWidth=0.0*CLHEP::MeV,"CharginoMinus Width");
  declareProperty("CharginoMinusCharge",m_CharginoMinusCharge=-1.*CLHEP::eplus,"CharginoMinus charge");
  declareProperty("CharginoMinusPDGCode",m_CharginoMinusPDGCode=-1000024,"CharginoMinus PDG CODE");
  declareProperty("CharginoMinusStable",m_CharginoMinusStable=true,"CharginoMinus Stable");
  declareProperty("CharginoMinusLifetime",m_CharginoMinusLifetime=-1,"CharginoMinus Lifetime");
  declareProperty("CharginoMinusShortlived",m_CharginoMinusShortlived=false,"CharginoMinus Shortlived");

  declareProperty("NeutralinoMass",m_NeutralinoMass= 100.0*CLHEP::GeV,"Neutralino Mass");
  declareProperty("NeutralinoWidth",m_NeutralinoWidth=0.0*CLHEP::MeV,"Neutralino Width");
  declareProperty("NeutralinoCharge",m_NeutralinoCharge=0.*CLHEP::eplus,"Neutralino charge");
  declareProperty("NeutralinoPDGCode",m_NeutralinoPDGCode=1000022,"Neutralino PDG CODE");
  declareProperty("NeutralinoStable",m_NeutralinoStable=true,"Neutralino Stable");
  declareProperty("NeutralinoLifetime",m_NeutralinoLifetime=-1,"Neutralino Lifetime");
  declareProperty("NeutralinoShortlived",m_NeutralinoShortlived=false,"Neutralino Shortlived");

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
  this->SetPhysicsName(this->name());
  return StatusCode::SUCCESS;
}

CharginosPhysicsTool* CharginosPhysicsTool::GetPhysicsOption()
{
  return this;
}

void CharginosPhysicsTool::ConstructParticle()
{
  ATH_MSG_DEBUG("ConstructParticle for the Charginos being run");

  m_theCharginoPlus = AMSBCharginoPlus::Definition(m_CharginoPlusMass, m_CharginoPlusWidth, m_CharginoPlusCharge, m_CharginoPlusPDGCode, m_CharginoPlusStable, m_CharginoPlusLifetime, m_CharginoPlusShortlived );

  m_theCharginoMinus = AMSBCharginoMinus::Definition(m_CharginoMinusMass, m_CharginoMinusWidth, m_CharginoMinusCharge, m_CharginoMinusPDGCode, m_CharginoMinusStable, m_CharginoMinusLifetime, m_CharginoMinusShortlived );

  m_theNeutralino = AMSBNeutralino::Definition(m_NeutralinoMass, m_NeutralinoWidth, m_NeutralinoCharge, m_NeutralinoPDGCode, m_NeutralinoStable, m_NeutralinoLifetime, m_NeutralinoShortlived );

}

void CharginosPhysicsTool::ConstructProcess()
{
  ATH_MSG_DEBUG("ConstructProcess for Charginos being run");

  G4ProcessManager *charginoPlus = m_theCharginoPlus->GetProcessManager();
  G4ProcessManager *charginoMinus = m_theCharginoMinus->GetProcessManager();

  charginoPlus->AddProcess(new G4hMultipleScattering,-1,1,1);
  charginoMinus->AddProcess(new G4hMultipleScattering,-1,1,1);
  charginoPlus->AddProcess(new G4hIonisation,-1,2,2);
  charginoMinus->AddProcess(new G4hIonisation,-1,2,2);
}
