/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//class header
#include "G4UserActions/CosmicPerigeeAction.h"
//athena headers
#include "CxxUtils/make_unique.h"
#include "MCTruth/TrackHelper.h"
//Geant4 headers
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
//STL headers
#include <iostream>

namespace G4UA
{

  //---------------------------------------------------------------------------
  CosmicPerigeeAction::CosmicPerigeeAction(const Config& config)
    : m_config(config)
    , m_trackRecordCollection("CosmicPerigee")
    , m_hasBeenSaved(false)
    , m_idZ(3490.) // ID maximum Z coordiate by default.
    , m_idR(1150.) // ID outer radius by default.
  {
  }

  //---------------------------------------------------------------------------
  void CosmicPerigeeAction::beginOfEvent(const G4Event*)
  {
    if (!m_trackRecordCollection.isValid()) {
      m_trackRecordCollection = CxxUtils::make_unique<TrackRecordCollection>(
          m_trackRecordCollection.name());
    }

    // @todo need a nice way of getting the maximum size of the ID
    // envelope in R and Z.
    //m_idR = gm->IdetOuterRadius();
    //m_idZ = gm->IdetMaxZ();
  }

  //---------------------------------------------------------------------------
  void CosmicPerigeeAction::endOfEvent(const G4Event*)
  {
  }

  //---------------------------------------------------------------------------
  void CosmicPerigeeAction::preTracking(const G4Track*)
  {
    // reset the field
    m_hasBeenSaved = false;
  }

  //---------------------------------------------------------------------------
  void CosmicPerigeeAction::processStep(const G4Step* aStep)
  {
    // See if this is a new track
    if (aStep->GetPreStepPoint()->GetStepStatus() == fUndefined)
      m_hasBeenSaved = false;

    // See if we've already saved it
    if (m_hasBeenSaved) return;

    // Only save muons or tracks in the ID
    if (aStep->GetTrack()->GetDefinition() != G4MuonPlus::Definition() &&
        aStep->GetTrack()->GetDefinition() != G4MuonMinus::Definition() &&
        ( aStep->GetPostStepPoint()->GetPosition().rho() > m_idR ||
          aStep->GetPostStepPoint()->GetPosition().z() > m_idZ   ||
          aStep->GetPostStepPoint()->GetPosition().z() < -m_idZ   ) ){
      // Both not a muon and not in the ID
      return;
    }

    // Check momentum
    if (aStep->GetTrack()->GetMomentum().mag() < m_config.pMinPrimary) return;

    // First order approximation of the answer to "is this the perigee"
    // if it is now moving away from the perigee and has not been saved, save it
    // That is true if mom dot pos > 0
    if ( aStep->GetTrack()->GetMomentum().x() * aStep->GetPostStepPoint()->GetPosition().x() +
         aStep->GetTrack()->GetMomentum().y() * aStep->GetPostStepPoint()->GetPosition().y() < 0) return;

    // Save the vertex...
    m_hasBeenSaved = true;

    // Decide whether to save the prestep or poststep point
    // - which one is closer to Perigee?
    G4StepPoint* preStep = aStep->GetPreStepPoint();
    G4StepPoint* postStep = aStep->GetPostStepPoint();
    G4StepPoint* theStep = preStep;
    if ( fabs( preStep->GetMomentumDirection().x() * preStep->GetPosition().x() +
               preStep->GetMomentumDirection().y() * preStep->GetPosition().y() ) >
         fabs( postStep->GetMomentumDirection().x() * postStep->GetPosition().x() +
               postStep->GetMomentumDirection().y() * postStep->GetPosition().y() ) )
    {
      // Using the post step point
      theStep = postStep;
    }

    // Could the following code be optimized?
    // There seems to be a bit of object copying.
    int pdgcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
    double ener = theStep->GetTotalEnergy();
    G4ThreeVector pos = theStep->GetPosition();
    G4ThreeVector mom = theStep->GetMomentum();
    double time       = theStep->GetGlobalTime();
    G4VPhysicalVolume *preVol = theStep->GetPhysicalVolume();

    if (aStep->GetTrack()->GetDefinition() == G4Geantino::Definition() ) pdgcode=999;
    if (aStep->GetTrack()->GetDefinition() == G4ChargedGeantino::Definition() ) pdgcode=998;

    // Create the TimedTrackRecord
    TrackHelper trHelp(aStep->GetTrack());
    int barcode = trHelp.GetBarcode();
    m_trackRecordCollection->Emplace(pdgcode, ener, mom, pos, time, barcode,
                                     preVol->GetName());
  }

} // namespace G4UA
