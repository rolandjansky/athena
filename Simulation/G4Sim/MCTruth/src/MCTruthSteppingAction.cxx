/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/MCTruthSteppingAction.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackRecorderSD.h"
#include "FadsActions/FadsSteppingAction.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntry.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

#include <iostream>
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"

#include <map>


using namespace FADS;

MCTruthSteppingAction::MCTruthSteppingAction():G4UserSteppingAction(),sde(0)
{
	FADS::FadsSteppingAction::GetSteppingAction()->SetMCTSteppingAction(this);
}
MCTruthSteppingAction::MCTruthSteppingAction(const MCTruthSteppingAction&):G4UserSteppingAction(),sde(0)
{
}

void MCTruthSteppingAction::AddRecordingEnvelope(std::string name, int lev,
												 std::string sdName)
{
	if (sdName.empty()) sdName=name;
	vm[name]=lev;
	FADS::SensitiveDetectorEntryT<TrackRecorderSD> ii(sdName);
	ii.Construct(sdName);
	G4SDManager *SDM=G4SDManager::GetSDMpointer();
	G4VSensitiveDetector *sd=SDM->FindSensitiveDetector(sdName);
	sdm[name]=sd;
}

void MCTruthSteppingAction::ListRecordingEnvelopes()
{
	volMapIterator it;
	for (it=vm.begin();it!=vm.end();it++)
		std::cout << " Recording Envelope -> "<<(*it).second<<std::endl;
}

void MCTruthSteppingAction::UserSteppingAction(const G4Step* aStep)
{
	if (vm.size() == 0) return;

	TrackHelper tHelp(aStep->GetTrack());
	if (tHelp.IsSecondary()) return;

	G4StepPoint *preStep=aStep->GetPreStepPoint();
	G4StepPoint *postStep=aStep->GetPostStepPoint();
	
	G4VPhysicalVolume *preVol=preStep->GetPhysicalVolume();
	G4VPhysicalVolume *postVol=postStep->GetPhysicalVolume();
	
	if (preVol==postVol) return;
	
	G4TouchableHistory *preTHist=(G4TouchableHistory *)preStep->GetTouchable();
	G4TouchableHistory *postTHist=(G4TouchableHistory *)postStep->GetTouchable();
	int nLev1=preTHist->GetHistoryDepth();
	int nLev2=postTHist->GetHistoryDepth();
	
	std::string vname1;
	volMapIterator it;
	for (it=vm.begin();it!=vm.end();it++)
	{
		int il=(*it).second;
		
		if (il<=(nLev1+1))
		{
		  vname1=preTHist->GetVolume(nLev1-il+1)->GetName();
		  if (vname1!=(*it).first) continue;
		
		  if (il<=(nLev2+1))
		  {
		    if (vname1==postTHist->GetVolume(nLev2-il+1)->GetName()) continue;
		  }

		  G4Step *vStep=const_cast<G4Step *>(aStep);
		  sdm[vname1]->Hit(vStep);
		  break;			
		}
	}

}
