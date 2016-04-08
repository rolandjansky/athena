/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "SleptonsPhysicsTool.h"
// package headers
#include "G4SElectronMinus.hh"
#include "G4SElectronPlus.hh"
#include "G4SMuonMinus.hh"
#include "G4SMuonPlus.hh"
#include "G4STauMinus.hh"
#include "G4STauPlus.hh"
// Geant4 headers
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"
#include "G4Transportation.hh"
#include "G4MuIonisation.hh"
#include "G4ProcessManager.hh"
#include "G4Version.hh"
//CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SleptonsPhysicsTool
//
// 15-05-2015 : Edoardo Farina
//-----------------------------------------------------------------------------

#if G4VERSION_NUMBER > 1009
#define PARTICLEITERATOR aParticleIterator
#else
#define PARTICLEITERATOR theParticleIterator
#endif

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SleptonsPhysicsTool::SleptonsPhysicsTool( const std::string& type,
                                          const std::string& nam,const IInterface* parent )
  : AthAlgTool ( type, nam , parent )
{
  ATH_MSG_DEBUG("SleptonsPhysicsTool "<<type<<" "<<nam);
  declareInterface< IPhysicsOptionTool >( this ) ;

  declareProperty("G4SElectronMinusMass",G4SElectronMinusMass=100.00*CLHEP::GeV,"G4SElectronMinus Mass");
  declareProperty("G4SElectronMinusWidth",G4SElectronMinusWidth=0.0*CLHEP::MeV,"G4SElectronMinus Width");
  declareProperty("G4SElectronMinusCharge",G4SElectronMinusCharge=-1.*CLHEP::eplus,"G4SElectronMinus charge");
  declareProperty("G4SElectronMinusPDGCode",G4SElectronMinusPDGCode=2000011,"G4SElectronMinus PDG CODE");
  declareProperty("G4SElectronMinusStable",G4SElectronMinusStable=true,"G4SElectronMinus Stable");
  declareProperty("G4SElectronMinusLifetime",G4SElectronMinusLifetime=-1,"G4SElectronMinus Lifetime");
  declareProperty("G4SElectronMinusShortlived",G4SElectronMinusShortlived=false,"G4SElectronMinus Shortlived");

  declareProperty("G4SElectronPlusMass",G4SElectronPlusMass= 100.0*CLHEP::GeV,"G4SElectronPlus Mass");
  declareProperty("G4SElectronPlusWidth",G4SElectronPlusWidth=0.0*CLHEP::MeV,"G4SElectronPlus Width");
  declareProperty("G4SElectronPlusCharge",G4SElectronPlusCharge=+1.*CLHEP::eplus,"G4SElectronPlus charge");
  declareProperty("G4SElectronPlusPDGCode",G4SElectronPlusPDGCode=-2000011,"G4SElectronPlus PDG CODE");
  declareProperty("G4SElectronPlusStable",G4SElectronPlusStable=true,"G4SElectronPlus Stable");
  declareProperty("G4SElectronPlusLifetime",G4SElectronPlusLifetime=-1,"G4SElectronPlus Lifetime");
  declareProperty("G4SElectronPlusShortlived",G4SElectronPlusShortlived=false,"G4SElectronPlus Shortlived");

  declareProperty("G4SMuonMinusMass",G4SMuonMinusMass= 100.0*CLHEP::GeV,"G4SMuonMinus Mass");
  declareProperty("G4SMuonMinusWidth",G4SMuonMinusWidth=0.0*CLHEP::MeV,"G4SMuonMinus Width");
  declareProperty("G4SMuonMinusCharge",G4SMuonMinusCharge=-1.*CLHEP::eplus,"G4SMuonMinus charge");
  declareProperty("G4SMuonMinusPDGCode",G4SMuonMinusPDGCode=2000013,"G4SMuonMinus PDG CODE");
  declareProperty("G4SMuonMinusStable",G4SMuonMinusStable=true,"G4SMuonMinus Stable");
  declareProperty("G4SMuonMinusLifetime",G4SMuonMinusLifetime=-1,"G4SMuonMinus Lifetime");
  declareProperty("G4SMuonMinusShortlived",G4SMuonMinusShortlived=false,"G4SMuonMinus Shortlived");

  declareProperty("G4SMuonPlusMass",G4SMuonPlusMass= 100.0*CLHEP::GeV,"G4SMuonPlus Mass");
  declareProperty("G4SMuonPlusWidth",G4SMuonPlusWidth=0.0*CLHEP::MeV,"G4SMuonPlus Width");
  declareProperty("G4SMuonPlusCharge",G4SMuonPlusCharge=1.*CLHEP::eplus,"G4SMuonPlus charge");
  declareProperty("G4SMuonPlusPDGCode",G4SMuonPlusPDGCode=-2000013,"G4SMuonPlus PDG CODE");
  declareProperty("G4SMuonPlusStable",G4SMuonPlusStable=true,"G4SMuonPlus Stable");
  declareProperty("G4SMuonPlusLifetime",G4SMuonPlusLifetime=-1,"G4SMuonPlus Lifetime");
  declareProperty("G4SMuonPlusShortlived",G4SMuonPlusShortlived=false,"G4SMuonPlus Shortlived");

  declareProperty("G4STauMinusMass",G4STauMinusMass= 100.0*CLHEP::GeV,"G4STauMinus Mass");
  declareProperty("G4STauMinusWidth",G4STauMinusWidth=0.0*CLHEP::MeV,"G4STauMinus Width");
  declareProperty("G4STauMinusCharge",G4STauMinusCharge=-1.*CLHEP::eplus,"G4STauMinus charge");
  declareProperty("G4STauMinusPDGCode",G4STauMinusPDGCode= 1000015,"G4STauMinus PDG CODE");
  declareProperty("G4STauMinusStable",G4STauMinusStable=true,"G4STauMinus Stable");
  declareProperty("G4STauMinusLifetime",G4STauMinusLifetime=-1,"G4STauMinus Lifetime");
  declareProperty("G4STauMinusShortlived",G4STauMinusShortlived=false,"G4STauMinus Shortlived");

  declareProperty("G4STauPlusMass",G4STauPlusMass= 100.0*CLHEP::GeV,"G4STauPlus Mass");
  declareProperty("G4STauPlusWidth",G4STauPlusWidth=0.0*CLHEP::MeV,"G4STauPlus Width");
  declareProperty("G4STauPlusCharge",G4STauPlusCharge=+1.*CLHEP::eplus,"G4STauPlus charge");
  declareProperty("G4STauPlusPDGCode",G4STauPlusPDGCode=-1000015,"G4STauPlus PDG CODE");
  declareProperty("G4STauPlusStable",G4STauPlusStable=true,"G4STauPlus Stable");
  declareProperty("G4STauPlusLifetime",G4STauPlusLifetime=-1,"G4STauPlus Lifetime");
  declareProperty("G4STauPlusShortlived",G4STauPlusShortlived=false,"G4STauPlus Shortlived");



}

//=============================================================================
// Destructor
//=============================================================================

SleptonsPhysicsTool::~SleptonsPhysicsTool()
{

}

//=============================================================================
// Initialize
//=============================================================================
StatusCode SleptonsPhysicsTool::initialize( )
{
  ATH_MSG_DEBUG("SleptonsPhysicsTool initialize( )");
  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}


SleptonsPhysicsTool* SleptonsPhysicsTool::GetPhysicsOption()
{
  return this;
}

void SleptonsPhysicsTool::ConstructParticle()
{
  ATH_MSG_DEBUG("ConstructParticle for the Sleptons being run");


  theSElectronMinus = G4SElectronMinus::Definition(G4SElectronMinusMass, G4SElectronMinusWidth, G4SElectronMinusCharge, G4SElectronMinusPDGCode, G4SElectronMinusStable, G4SElectronMinusLifetime, G4SElectronMinusShortlived );

  theSElectronPlus  = G4SElectronPlus::Definition(G4SElectronPlusMass, G4SElectronPlusWidth, G4SElectronPlusCharge, G4SElectronPlusPDGCode, G4SElectronPlusStable, G4SElectronPlusLifetime, G4SElectronPlusShortlived );

  theSMuonMinus     = G4SMuonMinus::Definition(G4SMuonMinusMass, G4SMuonMinusWidth, G4SMuonMinusCharge, G4SMuonMinusPDGCode, G4SMuonMinusStable, G4SMuonMinusLifetime, G4SMuonMinusShortlived );

  theSMuonPlus      = G4SMuonPlus::Definition(G4SMuonPlusMass, G4SMuonPlusWidth, G4SMuonPlusCharge, G4SMuonPlusPDGCode, G4SMuonPlusStable, G4SMuonPlusLifetime, G4SMuonPlusShortlived );

  theSTauMinus=G4STauMinus::Definition(G4STauMinusMass, G4STauMinusWidth, G4STauMinusCharge, G4STauMinusPDGCode, G4STauMinusStable, G4STauMinusLifetime, G4STauMinusShortlived );

  theSTauPlus=G4STauPlus::Definition(G4STauPlusMass, G4STauPlusWidth, G4STauPlusCharge, G4STauPlusPDGCode, G4STauPlusStable, G4STauPlusLifetime, G4STauPlusShortlived );

}

void SleptonsPhysicsTool::ConstructProcess()
{
  ATH_MSG_DEBUG(" ConstructProcess for Sleptons being run");
  PARTICLEITERATOR->reset();
  while( (*PARTICLEITERATOR)() ){
    G4ParticleDefinition* particle = PARTICLEITERATOR->value();
    int pdgcode=std::abs(particle->GetPDGEncoding());
    if (pdgcode==2000011 || pdgcode==2000013 || pdgcode==1000015)
      {
        G4ProcessManager* proc=particle->GetProcessManager();
        proc->AddProcess(new G4hMultipleScattering,-1,1,1);
        proc->AddProcess(new G4hIonisation,-1,2,2);
      }
  }
}
