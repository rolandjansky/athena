/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/IonizationStrategy.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/TrackHelper.h"
#include "SimHelpers/StepHelper.h"
#include "G4Step.hh"
#include "G4Track.hh"
#include <iostream>
#include <vector>

#include "CLHEP/Units/PhysicalConstants.h"

static IonizationStrategy Strategy("Ionization");

IonizationStrategy::IonizationStrategy(const std::string n):
			TruthStrategy(n)
{
	
	double prim_thr = 100*CLHEP::MeV;
        double seco_thr = 100*CLHEP::MeV;
	if(theTruthManager->GetTruthParameter("IonizationPrimaryMinEnergy")==0){
	  log()<<MSG::INFO<<"IonizationPrimaryMinEnergy: setting default value of 100 MeV"<<std::endl;
	  theTruthManager->SetTruthParameter("IonizationPrimaryMinEnergy",prim_thr);
	}
	if(theTruthManager->GetTruthParameter("IonizationSecondaryMinEnergy")==0){
	  log()<<MSG::INFO<<"IonizationSecondaryMinEnergy: setting default value of 100 MeV"<<std::endl;
	  theTruthManager->SetTruthParameter("IonizationSecondaryMinEnergy",seco_thr);
	}
}

bool IonizationStrategy::AnalyzeVertex(const G4Step* aStep)
{
	
	static StepHelper sHelper;
	static double eMinPrimary=theTruthManager->GetTruthParameter("IonizationPrimaryMinEnergy");
	static double eMinSecondary=theTruthManager->GetTruthParameter("IonizationSecondaryMinEnergy");
	
        sHelper.SetStep(aStep);
	G4int pSubType = sHelper.GetProcessSubType();
	if(pSubType==fIonisation ||
           pSubType==fPhotoElectricEffect)
	{
			
                double trackKEner= aStep->GetPreStepPoint()->GetMomentum().perp();
		if (trackKEner<eMinPrimary) return true;
		
		std::vector<G4Track*> tracks =theTruthManager->GetSecondaries();
		if (tracks.size() > 1) 
		{	
			int trackToUse=tracks.size()-1;
			G4Track* temp=tracks[trackToUse];
			tracks.clear();
			tracks.push_back(temp);
		}
		if (tracks.size()!=1) log()<<MSG::WARNING<<" IonizationStrategy: something is VERY wrong then!"<<std::endl;
                trackKEner= tracks[0]->GetMomentum().perp();
		if (trackKEner<eMinSecondary) return true;
		
		// we register the vertex using TruthManager's appropriate method
		theTruthManager->SaveSecondaryVertex(aStep->GetTrack(),
					             aStep->GetPostStepPoint(),
						     tracks);
		return true;
	}
	
	else return false;
}
