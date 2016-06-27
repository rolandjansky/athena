/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System includes
#include <iostream>
#include <string>

// Local includes
#include "AthenaStackingAction.h"

// Truth includes
#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/EventInformation.h"

// Geant4 includes
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4AntiNeutrinoTau.hh"
#include "G4Gamma.hh"


//
// Current AthenaStackingAction implementation.
// New one is below.
//

// static AthenaStackingAction stacking;

AthenaStackingAction::AthenaStackingAction(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent), p_killAllNeutrinos(false), p_stackEnergyCut(-1){

  declareProperty("KillAllNeutrinos",p_killAllNeutrinos);
  declareProperty("KillLowEPhotons",p_stackEnergyCut);
}

G4ClassificationOfNewTrack AthenaStackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
  // Neutrinos turned off at job options level
  G4ParticleDefinition &particleType = *(aTrack->GetDefinition() );
  if (  p_killAllNeutrinos                                               &&
        ( &particleType == G4NeutrinoE::NeutrinoEDefinition()              ||
          &particleType == G4AntiNeutrinoE::AntiNeutrinoEDefinition()      ||
          &particleType == G4NeutrinoMu::NeutrinoMuDefinition()            ||
          &particleType == G4AntiNeutrinoMu::AntiNeutrinoMuDefinition()    ||
          &particleType == G4NeutrinoTau::NeutrinoTauDefinition()          ||
          &particleType == G4AntiNeutrinoTau::AntiNeutrinoTauDefinition()  ) ){
    return fKill;
  }

  if ( &particleType == G4Gamma::Gamma() &&
       aTrack->GetTotalEnergy() < 0.00005 ){ // Safe cut for ultra-low-energy photons
    return fKill;
  }


  G4Track *mutableTrack=const_cast<G4Track*>(aTrack);

  G4Event *ev=G4EventManager::GetEventManager()->GetNonconstCurrentEvent();
  EventInformation* eventInfo __attribute__ ((unused)) =
    static_cast<EventInformation*>( ev->GetUserInformation() );

  int pID=aTrack->GetParentID();
  if (!pID)  // this is a primary particle.
    {
      const G4PrimaryParticle *pp=nullptr;
      PrimaryParticleInformation *ppi=nullptr;

      const G4DynamicParticle *dp = aTrack->GetDynamicParticle();
      if (dp)
        {
          pp=(const G4PrimaryParticle *)dp->GetPrimaryParticle();
        }
      if (pp)
        {
          ppi = dynamic_cast<PrimaryParticleInformation*> (pp->GetUserInformation());
        }
      if (ppi)
        {
          const HepMC::GenParticle *part=ppi->GetHepMCParticle();
          bool hasISFParticle = ppi->GetISFParticle()!=nullptr;
          if (!hasISFParticle)
            {
              // don't do anything
              if (part)
                {
                  // OK, we got back to HepMC
                  TrackInformation *ti=new TrackInformation(part);
                  ti->SetRegenerationNr(0);
                  // regNr=0 and classify=Primary are default values anyway
                  mutableTrack->SetUserInformation(ti);
                  ti->SetClassification(Primary);
                }
              else if (ppi->GetParticleBarcode()>=0)
                {
                  // PrimaryParticleInformation should at least provide a barcode
                  TrackBarcodeInfo *bi = new TrackBarcodeInfo(ppi->GetParticleBarcode());
                  mutableTrack->SetUserInformation(bi);
                }
            } // no ISFParticle attached
        } // has PrimaryParticleInformation
    }
  else    // secondary track: see if it must be saved
    {
      // Time cut for particles stacking after a certain time...
      if (  p_stackEnergyCut > 0 &&
            &particleType == G4Gamma::Gamma() &&
            aTrack->GetTotalEnergy() < p_stackEnergyCut )
        {
          return fKill;
        }

    }

  return fUrgent;
}


StatusCode AthenaStackingAction::queryInterface(const InterfaceID& riid, void** ppvInterface)
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


//
// New AthenaStackingAction implementation for multi-threading.
//

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  AthenaStackingAction::AthenaStackingAction(const Config& config)
    : m_config(config)
  {}

  //---------------------------------------------------------------------------
  // Classify a new track
  //---------------------------------------------------------------------------
  G4ClassificationOfNewTrack
  AthenaStackingAction::classifyNewTrack(const G4Track* track)
  {
    // Kill neutrinos if enabled
    if(m_config.killAllNeutrinos && isNeutrino(track)) {
      return fKill;
    }

    // Kill super-low-E photons
    const double safeCut = 0.00005;
    double totalE = track->GetTotalEnergy();
    if(isGamma(track) && totalE < safeCut) {
      return fKill;
    }

    // TODO: this terrible, evil code should REALLY be changed!!!
    G4Track* mutableTrack = const_cast<G4Track*> (track);

    // TODO: Why is this here? Can I remove it?
    G4Event* ev = G4EventManager::GetEventManager()->GetNonconstCurrentEvent();
    EventInformation* eventInfo __attribute__ ((unused)) =
      static_cast<EventInformation*> (ev->GetUserInformation());

    // Handle primary particles
    bool isPrimary = (track->GetParentID() == 0);
    if(isPrimary) {

      // Extract the PrimaryParticleInformation
      const G4PrimaryParticle* pp = nullptr;
      PrimaryParticleInformation* ppi = nullptr;
      const G4DynamicParticle* dp = track->GetDynamicParticle();
      if(dp) pp = dp->GetPrimaryParticle();
      if(pp) {
        ppi = dynamic_cast<PrimaryParticleInformation*>
          ( pp->GetUserInformation() );
      }

      // Fill some information for this track
      if(ppi) {
        const HepMC::GenParticle* part = ppi->GetHepMCParticle();
        bool hasISFParticle = ppi->GetISFParticle()!=nullptr;
        if (!hasISFParticle)
          {
            // don't do anything
            if(part)
              {
                // OK, we got back to HepMC
                TrackInformation* ti = new TrackInformation(part); // Hmm, who owns this?
                ti->SetRegenerationNr(0);
                // regNr=0 and classify=Primary are default values anyway
                mutableTrack->SetUserInformation(ti);
                ti->SetClassification(Primary);
              }
            // What does this condition mean?
            else if(ppi->GetParticleBarcode() >= 0)
              {
                // PrimaryParticleInformation should at least provide a barcode
                TrackBarcodeInfo* bi =
                  new TrackBarcodeInfo( ppi->GetParticleBarcode() );
                mutableTrack->SetUserInformation(bi);
              }
          } // no ISFParticle attached
      } // has PrimaryParticleInformation
    }
    // Secondary track; decide whether to save or kill
    else if( isGamma(track) &&
             m_config.photonEnergyCut > 0 &&
             totalE < m_config.photonEnergyCut )
      {
        return fKill;
      }

    return fUrgent;
  }

  //---------------------------------------------------------------------------
  // New tracking stack
  //---------------------------------------------------------------------------
  void AthenaStackingAction::newStage()
  {
  }

  //---------------------------------------------------------------------------
  // Prepare stacking for new event
  //---------------------------------------------------------------------------
  void AthenaStackingAction::prepareNewEvent()
  {
  }

  //---------------------------------------------------------------------------
  // Identify track definition
  //---------------------------------------------------------------------------
  bool AthenaStackingAction::isNeutrino(const G4Track* track) const
  {
    auto particleDef = track->GetParticleDefinition();
    return (particleDef == G4NeutrinoE::NeutrinoEDefinition()           ||
            particleDef == G4AntiNeutrinoE::AntiNeutrinoEDefinition()   ||
            particleDef == G4NeutrinoMu::NeutrinoMuDefinition()         ||
            particleDef == G4AntiNeutrinoMu::AntiNeutrinoMuDefinition() ||
            particleDef == G4NeutrinoTau::NeutrinoTauDefinition()       ||
            particleDef == G4AntiNeutrinoTau::AntiNeutrinoTauDefinition());
  }
  //---------------------------------------------------------------------------
  bool AthenaStackingAction::isGamma(const G4Track* track) const
  {
    return track->GetParticleDefinition() == G4Gamma::Gamma();
  }

} // namespace G4UA
