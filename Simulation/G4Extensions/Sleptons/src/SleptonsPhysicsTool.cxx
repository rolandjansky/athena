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
                                          const std::string& name,const IInterface* parent )
  : AthAlgTool ( type, name , parent )
  , m_theSElectronMinus(nullptr)
  , m_theSElectronPlus(nullptr)
  , m_theSMuonMinus(nullptr)
  , m_theSMuonPlus(nullptr)
  , m_theSTauMinus(nullptr)
  , m_theSTauPlus(nullptr)
{
  ATH_MSG_DEBUG("SleptonsPhysicsTool "<<type<<" "<<name);
  declareInterface< IPhysicsOptionTool >( this ) ;

  declareProperty("G4SElectronMinusMass",m_G4SElectronMinusMass=100.00*CLHEP::GeV,"G4SElectronMinus Mass");
  declareProperty("G4SElectronMinusWidth",m_G4SElectronMinusWidth=0.0*CLHEP::MeV,"G4SElectronMinus Width");
  declareProperty("G4SElectronMinusCharge",m_G4SElectronMinusCharge=-1.*CLHEP::eplus,"G4SElectronMinus charge");
  declareProperty("G4SElectronMinusPDGCode",m_G4SElectronMinusPDGCode=2000011,"G4SElectronMinus PDG CODE");
  declareProperty("G4SElectronMinusStable",m_G4SElectronMinusStable=true,"G4SElectronMinus Stable");
  declareProperty("G4SElectronMinusLifetime",m_G4SElectronMinusLifetime=-1,"G4SElectronMinus Lifetime");
  declareProperty("G4SElectronMinusShortlived",m_G4SElectronMinusShortlived=false,"G4SElectronMinus Shortlived");

  declareProperty("G4SElectronPlusMass",m_G4SElectronPlusMass= 100.0*CLHEP::GeV,"G4SElectronPlus Mass");
  declareProperty("G4SElectronPlusWidth",m_G4SElectronPlusWidth=0.0*CLHEP::MeV,"G4SElectronPlus Width");
  declareProperty("G4SElectronPlusCharge",m_G4SElectronPlusCharge=+1.*CLHEP::eplus,"G4SElectronPlus charge");
  declareProperty("G4SElectronPlusPDGCode",m_G4SElectronPlusPDGCode=-2000011,"G4SElectronPlus PDG CODE");
  declareProperty("G4SElectronPlusStable",m_G4SElectronPlusStable=true,"G4SElectronPlus Stable");
  declareProperty("G4SElectronPlusLifetime",m_G4SElectronPlusLifetime=-1,"G4SElectronPlus Lifetime");
  declareProperty("G4SElectronPlusShortlived",m_G4SElectronPlusShortlived=false,"G4SElectronPlus Shortlived");

  declareProperty("G4SMuonMinusMass",m_G4SMuonMinusMass= 100.0*CLHEP::GeV,"G4SMuonMinus Mass");
  declareProperty("G4SMuonMinusWidth",m_G4SMuonMinusWidth=0.0*CLHEP::MeV,"G4SMuonMinus Width");
  declareProperty("G4SMuonMinusCharge",m_G4SMuonMinusCharge=-1.*CLHEP::eplus,"G4SMuonMinus charge");
  declareProperty("G4SMuonMinusPDGCode",m_G4SMuonMinusPDGCode=2000013,"G4SMuonMinus PDG CODE");
  declareProperty("G4SMuonMinusStable",m_G4SMuonMinusStable=true,"G4SMuonMinus Stable");
  declareProperty("G4SMuonMinusLifetime",m_G4SMuonMinusLifetime=-1,"G4SMuonMinus Lifetime");
  declareProperty("G4SMuonMinusShortlived",m_G4SMuonMinusShortlived=false,"G4SMuonMinus Shortlived");

  declareProperty("G4SMuonPlusMass",m_G4SMuonPlusMass= 100.0*CLHEP::GeV,"G4SMuonPlus Mass");
  declareProperty("G4SMuonPlusWidth",m_G4SMuonPlusWidth=0.0*CLHEP::MeV,"G4SMuonPlus Width");
  declareProperty("G4SMuonPlusCharge",m_G4SMuonPlusCharge=1.*CLHEP::eplus,"G4SMuonPlus charge");
  declareProperty("G4SMuonPlusPDGCode",m_G4SMuonPlusPDGCode=-2000013,"G4SMuonPlus PDG CODE");
  declareProperty("G4SMuonPlusStable",m_G4SMuonPlusStable=true,"G4SMuonPlus Stable");
  declareProperty("G4SMuonPlusLifetime",m_G4SMuonPlusLifetime=-1,"G4SMuonPlus Lifetime");
  declareProperty("G4SMuonPlusShortlived",m_G4SMuonPlusShortlived=false,"G4SMuonPlus Shortlived");

  declareProperty("G4STauMinusMass",m_G4STauMinusMass= 100.0*CLHEP::GeV,"G4STauMinus Mass");
  declareProperty("G4STauMinusWidth",m_G4STauMinusWidth=0.0*CLHEP::MeV,"G4STauMinus Width");
  declareProperty("G4STauMinusCharge",m_G4STauMinusCharge=-1.*CLHEP::eplus,"G4STauMinus charge");
  declareProperty("G4STauMinusPDGCode",m_G4STauMinusPDGCode= 1000015,"G4STauMinus PDG CODE");
  declareProperty("G4STauMinusStable",m_G4STauMinusStable=true,"G4STauMinus Stable");
  declareProperty("G4STauMinusLifetime",m_G4STauMinusLifetime=-1,"G4STauMinus Lifetime");
  declareProperty("G4STauMinusShortlived",m_G4STauMinusShortlived=false,"G4STauMinus Shortlived");

  declareProperty("G4STauPlusMass",m_G4STauPlusMass= 100.0*CLHEP::GeV,"G4STauPlus Mass");
  declareProperty("G4STauPlusWidth",m_G4STauPlusWidth=0.0*CLHEP::MeV,"G4STauPlus Width");
  declareProperty("G4STauPlusCharge",m_G4STauPlusCharge=+1.*CLHEP::eplus,"G4STauPlus charge");
  declareProperty("G4STauPlusPDGCode",m_G4STauPlusPDGCode=-1000015,"G4STauPlus PDG CODE");
  declareProperty("G4STauPlusStable",m_G4STauPlusStable=true,"G4STauPlus Stable");
  declareProperty("G4STauPlusLifetime",m_G4STauPlusLifetime=-1,"G4STauPlus Lifetime");
  declareProperty("G4STauPlusShortlived",m_G4STauPlusShortlived=false,"G4STauPlus Shortlived");



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
  this->SetPhysicsName(this->name());
  return StatusCode::SUCCESS;
}


SleptonsPhysicsTool* SleptonsPhysicsTool::GetPhysicsOption()
{
  return this;
}

void SleptonsPhysicsTool::ConstructParticle()
{
  ATH_MSG_DEBUG("ConstructParticle for the Sleptons being run");


  m_theSElectronMinus = G4SElectronMinus::Definition(m_G4SElectronMinusMass, m_G4SElectronMinusWidth, m_G4SElectronMinusCharge, m_G4SElectronMinusPDGCode, m_G4SElectronMinusStable, m_G4SElectronMinusLifetime, m_G4SElectronMinusShortlived );

  m_theSElectronPlus  = G4SElectronPlus::Definition(m_G4SElectronPlusMass, m_G4SElectronPlusWidth, m_G4SElectronPlusCharge, m_G4SElectronPlusPDGCode, m_G4SElectronPlusStable, m_G4SElectronPlusLifetime, m_G4SElectronPlusShortlived );

  m_theSMuonMinus     = G4SMuonMinus::Definition(m_G4SMuonMinusMass, m_G4SMuonMinusWidth, m_G4SMuonMinusCharge, m_G4SMuonMinusPDGCode, m_G4SMuonMinusStable, m_G4SMuonMinusLifetime, m_G4SMuonMinusShortlived );

  m_theSMuonPlus      = G4SMuonPlus::Definition(m_G4SMuonPlusMass, m_G4SMuonPlusWidth, m_G4SMuonPlusCharge, m_G4SMuonPlusPDGCode, m_G4SMuonPlusStable, m_G4SMuonPlusLifetime, m_G4SMuonPlusShortlived );

  m_theSTauMinus=G4STauMinus::Definition(m_G4STauMinusMass, m_G4STauMinusWidth, m_G4STauMinusCharge, m_G4STauMinusPDGCode, m_G4STauMinusStable, m_G4STauMinusLifetime, m_G4STauMinusShortlived );

  m_theSTauPlus=G4STauPlus::Definition(m_G4STauPlusMass, m_G4STauPlusWidth, m_G4STauPlusCharge, m_G4STauPlusPDGCode, m_G4STauPlusStable, m_G4STauPlusLifetime, m_G4STauPlusShortlived );

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
