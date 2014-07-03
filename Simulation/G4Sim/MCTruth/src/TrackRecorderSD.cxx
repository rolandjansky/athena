/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TrackRecorderSD.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

#include "G4VSolid.hh"
#include "MCTruth/TrackHelper.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

static  FADS::SensitiveDetectorEntryT<TrackRecorderSD> ttrksd("TrackRecorderSD");

TrackRecorderSD::TrackRecorderSD(std::string name): FADS::FadsSensitiveDetector(name),trackRecordCollection(0)
{
}
TrackRecorderSD::~TrackRecorderSD()
{
}
void TrackRecorderSD::Initialize(G4HCofThisEvent*) 
{
        //trackRecordCollection = new TrackRecordCollection(GetName());
	trackRecordCollection = theHelper.RetrieveNonconstCollection<TrackRecordCollection>(GetName());
}
G4bool TrackRecorderSD::ProcessHits(G4Step* aStep,G4TouchableHistory* )
{

	G4StepPoint *preStep=aStep->GetPreStepPoint();
	G4StepPoint *postStep=aStep->GetPostStepPoint();
	G4VPhysicalVolume *preVol=preStep->GetPhysicalVolume();

	
	int pdgcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
	std::string pname = aStep->GetTrack()->GetDefinition()->GetParticleName();
	G4ThreeVector pos=postStep->GetPosition();
	G4ThreeVector mom=postStep->GetMomentum();

	double ener=postStep->GetTotalEnergy();	
	double time=postStep->GetGlobalTime();

	//create the TimedTrackRecord

	if(pname=="geantino") pdgcode=999;
	TrackHelper trHelp(aStep->GetTrack());
	int barcode = trHelp.GetBarcode();
	TrackRecord *rec=new TrackRecord(pdgcode,ener,mom,pos,time,barcode,preVol->GetName());
	trackRecordCollection->Insert(rec);

	return true;
}
void TrackRecorderSD::EndOfEvent(G4HCofThisEvent* ) 
{
    if (!m_allowMods) {
	  theHelper.SetConstCollection(trackRecordCollection);
	}
}
