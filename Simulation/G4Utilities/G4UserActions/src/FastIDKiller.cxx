/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/FastIDKiller.h"

#include <iostream>

#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4Step.hh"
#include "G4TrackVector.hh"

FastIDKiller::FastIDKiller(const std::string& type, const std::string& name, const IInterface* parent):
  UserActionBase(type,name,parent),
  m_energyCut(100000000.),
  m_killCount(0),
  m_init(false),
  m_idR(1150.),
  m_idZ(3490.)
{
  declareProperty("EnergyCut", m_energyCut);
  declareProperty("R",m_idR);
  declareProperty("Z",m_idZ);
}


void FastIDKiller::BeginOfRun(const G4Run*)
{
  ATH_MSG_INFO( "Including the Fast Inner Detector Killer." << std::endl
                << "\t This piece of code will kill all particles leaving the" << std::endl
                << "\t inner detector region (which should be defined in your" << std::endl
                << "\t job options) except those satisfying certain criteria." << std::endl
                << "\t (e/gamma will not be killed above " << m_energyCut/1000. << " GeV." );
  doInit();
}


StatusCode FastIDKiller::finalize()
{
  ATH_MSG_INFO( "Fast Inner Detector Killer killed " << m_killCount << " particles during this run." );
  return StatusCode::SUCCESS;
}


void FastIDKiller::doInit()
{
  //FIXME need a nice way of getting the maximum size of the ID envelope in R and Z.
  // EnvelopeGeometryManager *gm=EnvelopeGeometryManager::GetGeometryManager();
  // if(m_idR==0.)
  //   m_idR = gm->IdetOuterRadius();
  // if(m_idZ==0.)
  //   m_idZ = gm->IdetMaxZ();
  ATH_MSG_INFO( "Fast ID Killer initialized with radius " << m_idR << " and Z " << m_idZ);

  m_init=true;
}


void FastIDKiller::Step(const G4Step* aStep)
{
  if (msgLvl(MSG::VERBOSE)){
    ATH_MSG_DEBUG( " ===================================================== " );
    ATH_MSG_DEBUG( " Writing out Information for debugging: " );
    ATH_MSG_DEBUG( " Track-Pointer: " << aStep->GetTrack() );
    ATH_MSG_DEBUG( " GetDefinition is in the next line " );
    ATH_MSG_DEBUG( aStep->GetTrack()->GetDefinition() );
    ATH_MSG_DEBUG( " still going on ? " );
    ATH_MSG_DEBUG( " ===================================================== " );
  }

  // First ignore muons
  if (G4MuonPlus::MuonPlusDefinition() == aStep->GetTrack()->GetDefinition() ||
      G4MuonMinus::MuonMinusDefinition() == aStep->GetTrack()->GetDefinition() ) return;

  // Now we check if the particle is outside the Z or R edges of the inner detector envelope
  if (msgLvl(MSG::VERBOSE)){
    ATH_MSG_VERBOSE( " Checking pointers ... " );

    if ( !aStep->GetPostStepPoint() ||
         !aStep->GetPreStepPoint() )
      {
        ATH_MSG_ERROR( " One of the pointers was null!  This should never happen!!!" );
        throw "Null position pointer";
      }
  }

  if ( aStep->GetPreStepPoint()->GetPosition().rho() > m_idR ||
       aStep->GetPreStepPoint()->GetPosition().z() > m_idZ ||
       aStep->GetPreStepPoint()->GetPosition().z() < -m_idZ
       )
    {
      return; // We started outside the ID envelope
    }

  if ( aStep->GetPostStepPoint()->GetPosition().rho() < m_idR &&
       aStep->GetPostStepPoint()->GetPosition().z() < m_idZ &&
       aStep->GetPostStepPoint()->GetPosition().z() > -m_idZ
       )
    {
      return; // We finished inside the ID envelope
    }

  // Otherwise we have a non-muon that started inside and ended outside the ID envelope!  KILL IT!

  ATH_MSG_VERBOSE( " We have a " << aStep->GetTrack()->GetDefinition()->GetParticleName()
                   << " going from " << std::endl
                   << " ----> " << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion()->GetName()
                   << " to "
                   << aStep->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion()->GetName()
                   << std::endl
                   << " with attributes: " << std::endl
                   << " Energy: " << aStep->GetTrack()->GetTotalEnergy()
                   << " Eta: " << aStep->GetTrack()->GetMomentum ().getEta()
                   << " Pt: " << aStep->GetTrack()->GetMomentum ().perp()
                   << " and at coordinates: " << std::endl
                   << " R: " << aStep->GetPostStepPoint()->GetPosition().rho()
                   << " Z: " << aStep->GetPostStepPoint()->GetPosition().z()
                   << " Phi: " << aStep->GetPostStepPoint()->GetPosition().phi() );

  // Ignore electrons above a certain energy
  //  at some point it might be interesting to see what effect this has on other particles (eg pi0)
  if ( ( G4Electron::ElectronDefinition() == aStep->GetTrack()->GetDefinition() ||
         G4Positron::PositronDefinition() == aStep->GetTrack()->GetDefinition() ||
         G4Gamma::GammaDefinition() == aStep->GetTrack()->GetDefinition() ) &&
       m_energyCut < aStep->GetTrack()->GetTotalEnergy() ) return;

  if (msgLvl(MSG::DEBUG)){
    std::string name = aStep->GetTrack()->GetDefinition()->GetParticleName();
    ATH_MSG_DEBUG( " -------> The particle passed. It is a " << name << " and will be killed ! " );
  }

  // Otherwise kill the particle now
  KillParticle( aStep );
}


void FastIDKiller::KillParticle(const G4Step* aStep)
{
  aStep->GetTrack()->SetTrackStatus(fStopAndKill);
  const G4TrackVector *tv = aStep->GetSecondary();
  for (unsigned int i=0;i<tv->size();i++){
    if ( (*tv)[i]->GetPosition().rho() < m_idR &&
         (*tv)[i]->GetPosition().z() < m_idZ &&
         (*tv)[i]->GetPosition().z() > -m_idZ
         ) continue;
    (*tv)[i]->SetTrackStatus(fStopAndKill);
  }
  m_killCount++;
}


StatusCode FastIDKiller::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
