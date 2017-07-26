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
  AthenaStackingAction::ClassifyNewTrack(const G4Track* track)
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
