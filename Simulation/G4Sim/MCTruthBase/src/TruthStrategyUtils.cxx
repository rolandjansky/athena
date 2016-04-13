/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "MCTruthBase/TruthStrategyUtils.h"

// Geant4 includes
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

// Simulation infrastructure
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackHelper.h"

// HepMC EDM
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

// Framework utilities
#include "CxxUtils/make_unique.h"


namespace TruthStrategyUtils
{

  //---------------------------------------------------------------------------
  /// Convert step-point into a new vertex.
  //---------------------------------------------------------------------------
  std::unique_ptr<HepMC::GenVertex> stepPoint2Vertex(const G4StepPoint* stepPoint)
  {
    const G4ThreeVector& pos = stepPoint->GetPosition();
    const auto time = stepPoint->GetGlobalTime()*CLHEP::c_light;
    return CxxUtils::make_unique<HepMC::GenVertex>
      ( CLHEP::HepLorentzVector(pos.x(), pos.y(), pos.z(), time) );
  }

  //---------------------------------------------------------------------------
  /// Convert track into new particle.
  //---------------------------------------------------------------------------
  std::unique_ptr<HepMC::GenParticle> track2Particle(const G4Track* track)
  {
    const G4ThreeVector& mom = track->GetMomentum();
    const auto energy = track->GetTotalEnergy();
    const int status = 1;
    const auto pdg = track->GetDefinition()->GetPDGEncoding();
    return CxxUtils::make_unique<HepMC::GenParticle>
      ( CLHEP::HepLorentzVector(mom.x(), mom.y(), mom.z(), energy), pdg, status );
  }

  //---------------------------------------------------------------------------
  // Construct and save a truth vertex from list of secondaries
  //---------------------------------------------------------------------------
  void saveSecondaryVertex(G4Track* primaryTrack, G4StepPoint* stepPoint,
                           const std::vector<const G4Track*>& secondaries)
  {
    // TODO: do we need some MsgStream debug printouts?

    // TODO: somehow need to reduce lookups to the EventInformation.
    auto eventInformation = static_cast<EventInformation*>
      ( G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation() );

    // Construct the vertex first
    auto vtx = stepPoint2Vertex(stepPoint);
    //int vbcode = eventInformation->SecondaryVertexBarCode();
    vtx->suggest_barcode( eventInformation->SecondaryVertexBarCode() );
    // Vertex ID is the process ID offset by 1000
    vtx->set_id( stepPoint->GetProcessDefinedStep()->GetProcessSubType() + 1000 );

    // Add the input particle
    auto parIn = eventInformation->GetCurrentlyTraced();
    vtx->add_particle_in(parIn);

    // Handle the "primary" if it is non-null
    if (primaryTrack) {
      TrackHelper tHelper(primaryTrack);
      TrackInformation* tInfo = tHelper.GetTrackInformation();
      int regenerationNr = tInfo->GetRegenerationNr();
      regenerationNr++;
      tInfo->SetRegenerationNr(regenerationNr);
      if (tHelper.IsPrimary()) tInfo->SetClassification(RegeneratedPrimary);
      auto p1 = track2Particle(primaryTrack);
      eventInformation->SetCurrentlyTraced( p1.get() );
      p1->suggest_barcode( regenerationNr*1000000 + tHelper.GetBarcode() );
      // eventually we add p1 in the list of the outgoing particles
      vtx->add_particle_out( p1.release() ); // takes raw ptr ownership
    }

    // Now we handle all of the secondary tracks
    // TODO: consider throwing an exception instead of using an assert here.
    assert( secondaries.size() > 0 );
    for (auto secondary : secondaries) {
      auto p2 = track2Particle(secondary);
      p2->suggest_barcode( eventInformation->SecondaryParticleBarCode() );

      // Create and fill new TrackInformation
      auto trackInfo = CxxUtils::make_unique<TrackInformation>( p2.get() );
      trackInfo->SetRegenerationNr(0);
      trackInfo->SetClassification(RegisteredSecondary);

      // Some special handling for the ISF. This is expensive!
      // TODO: is there a better way to do this??
      auto vTrackInfo =
        dynamic_cast<VTrackInformation*>( secondary->GetUserInformation() );
      if (vTrackInfo) {
        const ISF::ISFParticle* parent = trackInfo->GetBaseISFParticle();
        trackInfo->SetBaseISFParticle(parent);
      }

      // Attach the user info
      // NOTE: Use of const_cast is merely a TEMPORARY HACK until the user info
      // becomes a mutable member of G4Track. Patch building is in progress.
      // TODO: update this once we have a new ATLAS patch of Geant4.
      auto nonConstSecondary = const_cast<G4Track*>(secondary);
      nonConstSecondary->SetUserInformation( trackInfo.release() );
      //secondary->SetUserInformation( trackInfo.release() );

      // Assign the particle to the vertex
      vtx->add_particle_out( p2.release() ); // raw ptr ownership
    }

    // Finally, add the new vertex to the event
    HepMC::GenEvent* genEvent = eventInformation->GetHepMCEvent();
    genEvent->add_vertex( vtx.release() ); // raw ptr ownership
  }

} // namespace TruthStrategyUtils
