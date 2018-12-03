/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "SleptonsPhysicsTool.h"
// package headers
#include "G4SElectronLMinus.hh"
#include "G4SElectronLPlus.hh"
#include "G4SMuonLMinus.hh"
#include "G4SMuonLPlus.hh"
#include "G4STau1Minus.hh"
#include "G4STau1Plus.hh"
#include "G4SElectronRMinus.hh"
#include "G4SElectronRPlus.hh"
#include "G4SMuonRMinus.hh"
#include "G4SMuonRPlus.hh"
#include "G4STau2Minus.hh"
#include "G4STau2Plus.hh"
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
  : base_class ( type, name , parent )
  , m_theSElectronLMinus(nullptr)
  , m_theSElectronLPlus(nullptr)
  , m_theSMuonLMinus(nullptr)
  , m_theSMuonLPlus(nullptr)
  , m_theSTau1Minus(nullptr)
  , m_theSTau1Plus(nullptr)
  , m_theSElectronRMinus(nullptr)
  , m_theSElectronRPlus(nullptr)
  , m_theSMuonRMinus(nullptr)
  , m_theSMuonRPlus(nullptr)
  , m_theSTau2Minus(nullptr)
  , m_theSTau2Plus(nullptr)
{
  declareProperty("G4SElectronLMinusMass",m_G4SElectronLMinusMass=100.00*CLHEP::GeV,"G4SElectronLMinus Mass");
  declareProperty("G4SElectronLMinusWidth",m_G4SElectronLMinusWidth=0.0*CLHEP::MeV,"G4SElectronLMinus Width");
  declareProperty("G4SElectronLMinusCharge",m_G4SElectronLMinusCharge=-1.*CLHEP::eplus,"G4SElectronLMinus charge");
  declareProperty("G4SElectronLMinusPDGCode",m_G4SElectronLMinusPDGCode=2000011,"G4SElectronLMinus PDG CODE");
  declareProperty("G4SElectronLMinusStable",m_G4SElectronLMinusStable=true,"G4SElectronLMinus Stable");
  declareProperty("G4SElectronLMinusLifetime",m_G4SElectronLMinusLifetime=-1,"G4SElectronLMinus Lifetime");
  declareProperty("G4SElectronLMinusShortlived",m_G4SElectronLMinusShortlived=false,"G4SElectronLMinus Shortlived");

  declareProperty("G4SElectronLPlusMass",m_G4SElectronLPlusMass= 100.0*CLHEP::GeV,"G4SElectronLPlus Mass");
  declareProperty("G4SElectronLPlusWidth",m_G4SElectronLPlusWidth=0.0*CLHEP::MeV,"G4SElectronLPlus Width");
  declareProperty("G4SElectronLPlusCharge",m_G4SElectronLPlusCharge=+1.*CLHEP::eplus,"G4SElectronLPlus charge");
  declareProperty("G4SElectronLPlusPDGCode",m_G4SElectronLPlusPDGCode=-2000011,"G4SElectronLPlus PDG CODE");
  declareProperty("G4SElectronLPlusStable",m_G4SElectronLPlusStable=true,"G4SElectronLPlus Stable");
  declareProperty("G4SElectronLPlusLifetime",m_G4SElectronLPlusLifetime=-1,"G4SElectronLPlus Lifetime");
  declareProperty("G4SElectronLPlusShortlived",m_G4SElectronLPlusShortlived=false,"G4SElectronLPlus Shortlived");

  declareProperty("G4SMuonLMinusMass",m_G4SMuonLMinusMass= 100.0*CLHEP::GeV,"G4SMuonLMinus Mass");
  declareProperty("G4SMuonLMinusWidth",m_G4SMuonLMinusWidth=0.0*CLHEP::MeV,"G4SMuonLMinus Width");
  declareProperty("G4SMuonLMinusCharge",m_G4SMuonLMinusCharge=-1.*CLHEP::eplus,"G4SMuonLMinus charge");
  declareProperty("G4SMuonLMinusPDGCode",m_G4SMuonLMinusPDGCode=2000013,"G4SMuonLMinus PDG CODE");
  declareProperty("G4SMuonLMinusStable",m_G4SMuonLMinusStable=true,"G4SMuonLMinus Stable");
  declareProperty("G4SMuonLMinusLifetime",m_G4SMuonLMinusLifetime=-1,"G4SMuonLMinus Lifetime");
  declareProperty("G4SMuonLMinusShortlived",m_G4SMuonLMinusShortlived=false,"G4SMuonLMinus Shortlived");

  declareProperty("G4SMuonLPlusMass",m_G4SMuonLPlusMass= 100.0*CLHEP::GeV,"G4SMuonLPlus Mass");
  declareProperty("G4SMuonLPlusWidth",m_G4SMuonLPlusWidth=0.0*CLHEP::MeV,"G4SMuonLPlus Width");
  declareProperty("G4SMuonLPlusCharge",m_G4SMuonLPlusCharge=1.*CLHEP::eplus,"G4SMuonLPlus charge");
  declareProperty("G4SMuonLPlusPDGCode",m_G4SMuonLPlusPDGCode=-2000013,"G4SMuonLPlus PDG CODE");
  declareProperty("G4SMuonLPlusStable",m_G4SMuonLPlusStable=true,"G4SMuonLPlus Stable");
  declareProperty("G4SMuonLPlusLifetime",m_G4SMuonLPlusLifetime=-1,"G4SMuonLPlus Lifetime");
  declareProperty("G4SMuonLPlusShortlived",m_G4SMuonLPlusShortlived=false,"G4SMuonLPlus Shortlived");

  declareProperty("G4STau1MinusMass",m_G4STau1MinusMass= 100.0*CLHEP::GeV,"G4STau1Minus Mass");
  declareProperty("G4STau1MinusWidth",m_G4STau1MinusWidth=0.0*CLHEP::MeV,"G4STau1Minus Width");
  declareProperty("G4STau1MinusCharge",m_G4STau1MinusCharge=-1.*CLHEP::eplus,"G4STau1Minus charge");
  declareProperty("G4STau1MinusPDGCode",m_G4STau1MinusPDGCode= 1000015,"G4STau1Minus PDG CODE");
  declareProperty("G4STau1MinusStable",m_G4STau1MinusStable=true,"G4STau1Minus Stable");
  declareProperty("G4STau1MinusLifetime",m_G4STau1MinusLifetime=-1,"G4STau1Minus Lifetime");
  declareProperty("G4STau1MinusShortlived",m_G4STau1MinusShortlived=false,"G4STau1Minus Shortlived");

  declareProperty("G4STau1PlusMass",m_G4STau1PlusMass= 100.0*CLHEP::GeV,"G4STau1Plus Mass");
  declareProperty("G4STau1PlusWidth",m_G4STau1PlusWidth=0.0*CLHEP::MeV,"G4STau1Plus Width");
  declareProperty("G4STau1PlusCharge",m_G4STau1PlusCharge=+1.*CLHEP::eplus,"G4STau1Plus charge");
  declareProperty("G4STau1PlusPDGCode",m_G4STau1PlusPDGCode=-1000015,"G4STau1Plus PDG CODE");
  declareProperty("G4STau1PlusStable",m_G4STau1PlusStable=true,"G4STau1Plus Stable");
  declareProperty("G4STau1PlusLifetime",m_G4STau1PlusLifetime=-1,"G4STau1Plus Lifetime");
  declareProperty("G4STau1PlusShortlived",m_G4STau1PlusShortlived=false,"G4STau1Plus Shortlived");

  declareProperty("G4SElectronRMinusMass",m_G4SElectronRMinusMass=100.00*CLHEP::GeV,"G4SElectronRMinus Mass");
  declareProperty("G4SElectronRMinusWidth",m_G4SElectronRMinusWidth=0.0*CLHEP::MeV,"G4SElectronRMinus Width");
  declareProperty("G4SElectronRMinusCharge",m_G4SElectronRMinusCharge=-1.*CLHEP::eplus,"G4SElectronRMinus charge");
  declareProperty("G4SElectronRMinusPDGCode",m_G4SElectronRMinusPDGCode=2000011,"G4SElectronRMinus PDG CODE");
  declareProperty("G4SElectronRMinusStable",m_G4SElectronRMinusStable=true,"G4SElectronRMinus Stable");
  declareProperty("G4SElectronRMinusLifetime",m_G4SElectronRMinusLifetime=-1,"G4SElectronRMinus Lifetime");
  declareProperty("G4SElectronRMinusShortlived",m_G4SElectronRMinusShortlived=false,"G4SElectronRMinus Shortlived");

  declareProperty("G4SElectronRPlusMass",m_G4SElectronRPlusMass= 100.0*CLHEP::GeV,"G4SElectronRPlus Mass");
  declareProperty("G4SElectronRPlusWidth",m_G4SElectronRPlusWidth=0.0*CLHEP::MeV,"G4SElectronRPlus Width");
  declareProperty("G4SElectronRPlusCharge",m_G4SElectronRPlusCharge=+1.*CLHEP::eplus,"G4SElectronRPlus charge");
  declareProperty("G4SElectronRPlusPDGCode",m_G4SElectronRPlusPDGCode=-2000011,"G4SElectronRPlus PDG CODE");
  declareProperty("G4SElectronRPlusStable",m_G4SElectronRPlusStable=true,"G4SElectronRPlus Stable");
  declareProperty("G4SElectronRPlusLifetime",m_G4SElectronRPlusLifetime=-1,"G4SElectronRPlus Lifetime");
  declareProperty("G4SElectronRPlusShortlived",m_G4SElectronRPlusShortlived=false,"G4SElectronRPlus Shortlived");

  declareProperty("G4SMuonRMinusMass",m_G4SMuonRMinusMass= 100.0*CLHEP::GeV,"G4SMuonRMinus Mass");
  declareProperty("G4SMuonRMinusWidth",m_G4SMuonRMinusWidth=0.0*CLHEP::MeV,"G4SMuonRMinus Width");
  declareProperty("G4SMuonRMinusCharge",m_G4SMuonRMinusCharge=-1.*CLHEP::eplus,"G4SMuonRMinus charge");
  declareProperty("G4SMuonRMinusPDGCode",m_G4SMuonRMinusPDGCode=2000013,"G4SMuonRMinus PDG CODE");
  declareProperty("G4SMuonRMinusStable",m_G4SMuonRMinusStable=true,"G4SMuonRMinus Stable");
  declareProperty("G4SMuonRMinusLifetime",m_G4SMuonRMinusLifetime=-1,"G4SMuonRMinus Lifetime");
  declareProperty("G4SMuonRMinusShortlived",m_G4SMuonRMinusShortlived=false,"G4SMuonRMinus Shortlived");

  declareProperty("G4SMuonRPlusMass",m_G4SMuonRPlusMass= 100.0*CLHEP::GeV,"G4SMuonRPlus Mass");
  declareProperty("G4SMuonRPlusWidth",m_G4SMuonRPlusWidth=0.0*CLHEP::MeV,"G4SMuonRPlus Width");
  declareProperty("G4SMuonRPlusCharge",m_G4SMuonRPlusCharge=1.*CLHEP::eplus,"G4SMuonRPlus charge");
  declareProperty("G4SMuonRPlusPDGCode",m_G4SMuonRPlusPDGCode=-2000013,"G4SMuonRPlus PDG CODE");
  declareProperty("G4SMuonRPlusStable",m_G4SMuonRPlusStable=true,"G4SMuonRPlus Stable");
  declareProperty("G4SMuonRPlusLifetime",m_G4SMuonRPlusLifetime=-1,"G4SMuonRPlus Lifetime");
  declareProperty("G4SMuonRPlusShortlived",m_G4SMuonRPlusShortlived=false,"G4SMuonRPlus Shortlived");

  declareProperty("G4STau2MinusMass",m_G4STau2MinusMass= 100.0*CLHEP::GeV,"G4STau2Minus Mass");
  declareProperty("G4STau2MinusWidth",m_G4STau2MinusWidth=0.0*CLHEP::MeV,"G4STau2Minus Width");
  declareProperty("G4STau2MinusCharge",m_G4STau2MinusCharge=-1.*CLHEP::eplus,"G4STau2Minus charge");
  declareProperty("G4STau2MinusPDGCode",m_G4STau2MinusPDGCode= 1000015,"G4STau2Minus PDG CODE");
  declareProperty("G4STau2MinusStable",m_G4STau2MinusStable=true,"G4STau2Minus Stable");
  declareProperty("G4STau2MinusLifetime",m_G4STau2MinusLifetime=-1,"G4STau2Minus Lifetime");
  declareProperty("G4STau2MinusShortlived",m_G4STau2MinusShortlived=false,"G4STau2Minus Shortlived");

  declareProperty("G4STau2PlusMass",m_G4STau2PlusMass= 100.0*CLHEP::GeV,"G4STau2Plus Mass");
  declareProperty("G4STau2PlusWidth",m_G4STau2PlusWidth=0.0*CLHEP::MeV,"G4STau2Plus Width");
  declareProperty("G4STau2PlusCharge",m_G4STau2PlusCharge=+1.*CLHEP::eplus,"G4STau2Plus charge");
  declareProperty("G4STau2PlusPDGCode",m_G4STau2PlusPDGCode=-1000015,"G4STau2Plus PDG CODE");
  declareProperty("G4STau2PlusStable",m_G4STau2PlusStable=true,"G4STau2Plus Stable");
  declareProperty("G4STau2PlusLifetime",m_G4STau2PlusLifetime=-1,"G4STau2Plus Lifetime");
  declareProperty("G4STau2PlusShortlived",m_G4STau2PlusShortlived=false,"G4STau2Plus Shortlived");
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

  if (m_G4SElectronRMinusMass>0. && std::abs(m_G4SElectronRMinusPDGCode)>=2000000){
    m_theSElectronRMinus = G4SElectronRMinus::Definition(m_G4SElectronRMinusMass, m_G4SElectronRMinusWidth, m_G4SElectronRMinusCharge, m_G4SElectronRMinusPDGCode, m_G4SElectronRMinusStable, m_G4SElectronRMinusLifetime, m_G4SElectronRMinusShortlived );
  }

  if (m_G4SElectronRPlusMass>0. && std::abs(m_G4SElectronRPlusPDGCode)>=2000000){
    m_theSElectronRPlus  = G4SElectronRPlus::Definition(m_G4SElectronRPlusMass, m_G4SElectronRPlusWidth, m_G4SElectronRPlusCharge, m_G4SElectronRPlusPDGCode, m_G4SElectronRPlusStable, m_G4SElectronRPlusLifetime, m_G4SElectronRPlusShortlived );
  }

  if (m_G4SMuonRMinusMass>0. && std::abs(m_G4SMuonRMinusPDGCode)>=2000000){
    m_theSMuonRMinus     = G4SMuonRMinus::Definition(m_G4SMuonRMinusMass, m_G4SMuonRMinusWidth, m_G4SMuonRMinusCharge, m_G4SMuonRMinusPDGCode, m_G4SMuonRMinusStable, m_G4SMuonRMinusLifetime, m_G4SMuonRMinusShortlived );
  }

  if (m_G4SMuonRPlusMass>0. && std::abs(m_G4SMuonRPlusPDGCode)>=2000000){
    m_theSMuonRPlus      = G4SMuonRPlus::Definition(m_G4SMuonRPlusMass, m_G4SMuonRPlusWidth, m_G4SMuonRPlusCharge, m_G4SMuonRPlusPDGCode, m_G4SMuonRPlusStable, m_G4SMuonRPlusLifetime, m_G4SMuonRPlusShortlived );
  }

  if (m_G4STau1MinusMass>0. && std::abs(m_G4STau1MinusPDGCode)<2000000){
    m_theSTau1Minus=G4STau1Minus::Definition(m_G4STau1MinusMass, m_G4STau1MinusWidth, m_G4STau1MinusCharge, m_G4STau1MinusPDGCode, m_G4STau1MinusStable, m_G4STau1MinusLifetime, m_G4STau1MinusShortlived );
  }

  if (m_G4STau1PlusMass>0. && std::abs(m_G4STau1PlusPDGCode)<2000000){
    m_theSTau1Plus=G4STau1Plus::Definition(m_G4STau1PlusMass, m_G4STau1PlusWidth, m_G4STau1PlusCharge, m_G4STau1PlusPDGCode, m_G4STau1PlusStable, m_G4STau1PlusLifetime, m_G4STau1PlusShortlived );
  }

  if (m_G4SElectronLMinusMass>0. && std::abs(m_G4SElectronLMinusPDGCode)<2000000){
    m_theSElectronLMinus = G4SElectronLMinus::Definition(m_G4SElectronLMinusMass, m_G4SElectronLMinusWidth, m_G4SElectronLMinusCharge, m_G4SElectronLMinusPDGCode, m_G4SElectronLMinusStable, m_G4SElectronLMinusLifetime, m_G4SElectronLMinusShortlived );
  }

  if (m_G4SElectronLPlusMass>0. && std::abs(m_G4SElectronLPlusPDGCode)<2000000){
    m_theSElectronLPlus  = G4SElectronLPlus::Definition(m_G4SElectronLPlusMass, m_G4SElectronLPlusWidth, m_G4SElectronLPlusCharge, m_G4SElectronLPlusPDGCode, m_G4SElectronLPlusStable, m_G4SElectronLPlusLifetime, m_G4SElectronLPlusShortlived );
  }

  if (m_G4SMuonLMinusMass>0. && std::abs(m_G4SMuonLMinusPDGCode)<2000000){
    m_theSMuonLMinus     = G4SMuonLMinus::Definition(m_G4SMuonLMinusMass, m_G4SMuonLMinusWidth, m_G4SMuonLMinusCharge, m_G4SMuonLMinusPDGCode, m_G4SMuonLMinusStable, m_G4SMuonLMinusLifetime, m_G4SMuonLMinusShortlived );
  }

  if (m_G4SMuonLPlusMass>0. && std::abs(m_G4SMuonLPlusPDGCode)<2000000){
    m_theSMuonLPlus      = G4SMuonLPlus::Definition(m_G4SMuonLPlusMass, m_G4SMuonLPlusWidth, m_G4SMuonLPlusCharge, m_G4SMuonLPlusPDGCode, m_G4SMuonLPlusStable, m_G4SMuonLPlusLifetime, m_G4SMuonLPlusShortlived );
  }

  if (m_G4STau2MinusMass>0. && std::abs(m_G4STau2MinusPDGCode)>=2000000){
    m_theSTau2Minus=G4STau2Minus::Definition(m_G4STau2MinusMass, m_G4STau2MinusWidth, m_G4STau2MinusCharge, m_G4STau2MinusPDGCode, m_G4STau2MinusStable, m_G4STau2MinusLifetime, m_G4STau2MinusShortlived );
  }

  if (m_G4STau2PlusMass>0. && std::abs(m_G4STau2PlusPDGCode)>=2000000){
    m_theSTau2Plus=G4STau2Plus::Definition(m_G4STau2PlusMass, m_G4STau2PlusWidth, m_G4STau2PlusCharge, m_G4STau2PlusPDGCode, m_G4STau2PlusStable, m_G4STau2PlusLifetime, m_G4STau2PlusShortlived );
  }

}

void SleptonsPhysicsTool::ConstructProcess()
{
  ATH_MSG_DEBUG(" ConstructProcess for Sleptons being run");
  PARTICLEITERATOR->reset();
  while( (*PARTICLEITERATOR)() ){
    G4ParticleDefinition* particle = PARTICLEITERATOR->value();
    int pdgcode=std::abs(particle->GetPDGEncoding());
    if (pdgcode==2000011 || pdgcode==2000013 || pdgcode==1000015 || pdgcode==1000011 || pdgcode==1000013 || pdgcode==2000015)
      {
        G4ProcessManager* proc=particle->GetProcessManager();
        proc->AddProcess(new G4hMultipleScattering,-1,1,1);
        proc->AddProcess(new G4hIonisation,-1,2,2);
      }
  }
}
