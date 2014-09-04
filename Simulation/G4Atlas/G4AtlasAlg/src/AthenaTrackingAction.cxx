/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasAlg/AthenaTrackingAction.h"
#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TruthStrategyManager.h"
#include <iostream>

#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"
#include "G4EventManager.hh"
#include "MCTruth/TrackHelper.h"
#include "FadsActions/FadsTrackingAction.h"
#include "MCTruth/AtlasTrajectory.h"

// static AthenaTrackingAction ta;

void AthenaTrackingAction::PreUserTrackingAction(const G4Track* inTrack)
{	
	static int ilevel=TruthStrategyManager::GetStrategyManager()->
					GetSecondarySavingLevel();
//	std::cout<<" this is AthenaTrackingAction::PreUserTrackingAction"<<std::endl;
	G4Track* inT = const_cast<G4Track*> (inTrack);
	TrackHelper trackHelper(inT);
	if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary())
	{
		HepMC::GenParticle* part=const_cast<HepMC::GenParticle*> (
					trackHelper.GetTrackInformation()->
						GetHepMCParticle());
		EventInformation *eventInfo=TruthStrategyManager::GetStrategyManager()->
					GetEventInformation();
		if (trackHelper.IsPrimary()) eventInfo->SetCurrentPrimary(part);
		eventInfo->SetCurrentlyTraced(part);
	}
 	if (trackHelper.IsPrimary() || 
	    (trackHelper.IsRegisteredSecondary()&&ilevel>1) ||
	    (trackHelper.IsSecondary()&&ilevel>2)) 
 	{		
		G4VTrajectory *temp=new AtlasTrajectory(inTrack);
		FADS::FadsTrackingAction::GetTrackingAction()->SetTraj(temp);		
	}
}


void AthenaTrackingAction::PostUserTrackingAction(const G4Track* /*outTrack*/)
{
//	std::cout <<"AthenaTrackingAction::PostUserTrackingAction"<<std::endl;
	FADS::FadsTrackingAction::GetTrackingAction()->ResetTraj();
}
