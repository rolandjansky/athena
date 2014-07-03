/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/CosmicTRSD.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

#include "G4VSolid.hh"
#include "MCTruth/TrackHelper.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

static  FADS::SensitiveDetectorEntryT<CosmicTRSD> ttrksd("CosmicTRSD");

CosmicTRSD::CosmicTRSD(std::string name): FADS::FadsSensitiveDetector(name),trackRecordCollection(0)
{
}
CosmicTRSD::~CosmicTRSD()
{
}
void CosmicTRSD::Initialize(G4HCofThisEvent*) 
{
	trackRecordCollection = new TrackRecordCollection(GetName());
}
G4bool CosmicTRSD::ProcessHits(G4Step* aStep,G4TouchableHistory* )
{

	G4StepPoint *preStep=aStep->GetPreStepPoint();
	G4StepPoint *postStep=aStep->GetPostStepPoint();
	G4VPhysicalVolume *preVol=preStep->GetPhysicalVolume();
	G4VPhysicalVolume *postVol=postStep->GetPhysicalVolume();

	G4TouchableHistory* touchHist = (G4TouchableHistory*)preStep->GetTouchable();

	if (preVol==postVol) return false;

	int pdgcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
	std::string pname = aStep->GetTrack()->GetDefinition()->GetParticleName();
	G4ThreeVector pos=postStep->GetPosition();
	G4ThreeVector mom=postStep->GetMomentum();

	// need to get the local position
	const G4AffineTransform trans = aStep->GetTrack()->GetTouchable()->GetHistory()->GetTopTransform(); // trans from global to local

	G4ThreeVector localPos=trans.TransformPoint(pos);
	G4ThreeVector localMom=mom;
	trans.ApplyAxisTransform(localMom); // rotate momentum as needed

	G4VSolid* shape= touchHist->GetSolid();
	G4ThreeVector normal=shape->SurfaceNormal(localPos); 

	// check if particle is entering. if it is extiting don't record it.	
	if(normal.dot(localPos)>=0.) return false;

	//double ener=aStep->GetTrack()->GetKineticEnergy();
	double ener=postStep->GetTotalEnergy();
	
	double time=postStep->GetGlobalTime();

	//create the TimedTrackRecord

	if(pname=="geantino") pdgcode=999;
	TrackHelper trHelp(aStep->GetTrack());
	int barcode = trHelp.GetBarcode();
	TrackRecord *rec=new TrackRecord(pdgcode,ener,localMom,localPos,time,barcode,preVol->GetName());
	trackRecordCollection->Insert(rec);

	return true;
}
void CosmicTRSD::EndOfEvent(G4HCofThisEvent* ) 
{
	theHelper.ExportCollection<TrackRecordCollection>(trackRecordCollection);
}
