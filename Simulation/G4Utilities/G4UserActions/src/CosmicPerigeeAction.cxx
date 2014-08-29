/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/CosmicPerigeeAction.h"
#include <iostream>
#include "MCTruth/TrackRecorderSD.h"
#include "MCTruth/TrackHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

static CosmicPerigeeAction cpa("CosmicPerigeeAction");

void CosmicPerigeeAction::BeginOfEventAction(const G4Event*)
{
        //trackRecordCollection = new TrackRecordCollection("CosmicPerigee");
        trackRecordCollection = theHelper.RetrieveNonconstCollection<TrackRecordCollection>("CosmicPerigee");
        EnvelopeGeometryManager *gm=EnvelopeGeometryManager::GetGeometryManager();
        m_idR = gm->IdetOuterRadius();
        m_idZ = gm->IdetMaxZ();
}
void CosmicPerigeeAction::EndOfEventAction(const G4Event*)
{
        //theHelper.ExportCollection<TrackRecordCollection>(trackRecordCollection);
  if (!theProperties["allowMods"].empty()) {
    if (theProperties["allowMods"] == "0")
      theHelper.SetConstCollection(trackRecordCollection);
  }
  else {
    std::cout<<"allowMods not set as a property in CosmicPerigeeAction!"<<std::endl;
  }
}
void CosmicPerigeeAction::BeginOfRunAction(const G4Run*)
{
        perigeeRecord = new TrackRecorderSD("CosmicPerigee");
}
void CosmicPerigeeAction::EndOfRunAction(const G4Run*)
{;}
void CosmicPerigeeAction::SteppingAction(const G4Step* aStep)
{
        static double pMinPrimary=100*CLHEP::MeV;
        static bool hasBeenSaved = false;

        // See if this is a new track
        if (aStep->GetPreStepPoint()->GetStepStatus() == fUndefined) hasBeenSaved = false;

        // See if we've already saved it
        if (hasBeenSaved) return;

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
        if (aStep->GetTrack()->GetMomentum().mag() < pMinPrimary) return;

        // First order approximation of the answer to "is this the perigee"
        // if it is now moving away from the perigee and has not been saved, save it
        // That is true if mom dot pos > 0
        if ( aStep->GetTrack()->GetMomentum().x() * aStep->GetPostStepPoint()->GetPosition().x() +
             aStep->GetTrack()->GetMomentum().y() * aStep->GetPostStepPoint()->GetPosition().y() < 0) return;

        // Save the vertex...
        hasBeenSaved = true;

        // Decide whether to save the prestep or poststep point - which one is closer to Perigee?
        G4StepPoint *theStep = aStep->GetPreStepPoint();
        if ( fabs( aStep->GetPreStepPoint()->GetMomentumDirection().x() * aStep->GetPreStepPoint()->GetPosition().x() +
                   aStep->GetPreStepPoint()->GetMomentumDirection().y() * aStep->GetPreStepPoint()->GetPosition().y() ) >
             fabs( aStep->GetPostStepPoint()->GetMomentumDirection().x() * aStep->GetPostStepPoint()->GetPosition().x() +
                   aStep->GetPostStepPoint()->GetMomentumDirection().y() * aStep->GetPostStepPoint()->GetPosition().y() ) ){
          // Using the post step point
          theStep = aStep->GetPostStepPoint();
        }

        int pdgcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
        double ener = theStep->GetTotalEnergy();
        G4ThreeVector pos = theStep->GetPosition();
        G4ThreeVector mom = theStep->GetMomentum();
        double time       = theStep->GetGlobalTime();
        G4VPhysicalVolume *preVol = theStep->GetPhysicalVolume();

        if (aStep->GetTrack()->GetDefinition() == G4Geantino::Definition() ) pdgcode=999;
        if (aStep->GetTrack()->GetDefinition() == G4ChargedGeantino::Definition() ) pdgcode=998;

        //create the TimedTrackRecord
        TrackHelper trHelp(aStep->GetTrack());
        int barcode = trHelp.GetBarcode();
        TrackRecord *rec=new TrackRecord(pdgcode,ener,mom,pos,time,barcode,preVol->GetName());
        trackRecordCollection->Insert(rec);
}
