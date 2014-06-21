/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/BremsstrahlungStrategy.h"
#include "SimHelpers/StepHelper.h"
#include "G4Step.hh"
#include "G4Track.hh"
#include <iostream>
#include <vector>

#include "CLHEP/Units/PhysicalConstants.h"

static BremsstrahlungStrategy Strategy("Bremsstrahlung");

BremsstrahlungStrategy::BremsstrahlungStrategy(const std::string n):
			TruthStrategy(n)
{
	double prim_thr = 100*CLHEP::MeV;
	double seco_thr = 100*CLHEP::MeV;

	// don't overwrite the values already in the strategy manager
	if(theTruthManager->GetTruthParameter("BremsPrimaryMinEnergy")==0){
	  log()<<MSG::INFO<<"BremsPrimaryMinEnergy: setting default value of 100 MeV"<<std::endl;  
	  theTruthManager->SetTruthParameter("BremsPrimaryMinEnergy",prim_thr);
	}
	if(theTruthManager->GetTruthParameter("BremsSecondaryMinEnergy")==0){
	  log()<<MSG::INFO<<"BremsSecondaryMinEnergy: setting default value of 100 MeV"<<std::endl;  
	  theTruthManager->SetTruthParameter("BremsSecondaryMinEnergy",seco_thr);
	}
}

bool BremsstrahlungStrategy::AnalyzeVertex(const G4Step* aStep)
{

	
	static StepHelper sHelper;
	static double eMinPrimary=theTruthManager->GetTruthParameter("BremsPrimaryMinEnergy");
	static double eMinSecondary=theTruthManager->GetTruthParameter("BremsSecondaryMinEnergy");
	sHelper.SetStep(aStep);
        G4int pSubType = sHelper.GetProcessSubType();

        if(pSubType==fBremsstrahlung)
	{
		double trackKEner= aStep->GetPreStepPoint()->GetMomentum().perp();
		if (trackKEner<eMinPrimary) return true;

 		std::vector<G4Track*> tracks =theTruthManager->GetSecondaries();
		if (tracks.size() != 1) 
		{	
			int trackToUse=tracks.size()-1;
			G4Track* temp=tracks[trackToUse];
			tracks.clear();
			tracks.push_back(temp);
		}
		if (tracks.size()!=1) log()<<MSG::WARNING<<" BremsstrahlungStrategy: something is VERY wrong then!"<<std::endl;
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
