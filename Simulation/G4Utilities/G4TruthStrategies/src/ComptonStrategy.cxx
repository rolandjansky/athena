/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/ComptonStrategy.h"
#include "SimHelpers/StepHelper.h"
#include "G4Step.hh"
#include "G4Track.hh"
#include <iostream>
#include <vector>

#include "CLHEP/Units/PhysicalConstants.h"

static ComptonStrategy Strategy("Compton");

ComptonStrategy::ComptonStrategy(const std::string n):
			TruthStrategy(n)
{
	double prim_thr = 100*CLHEP::MeV;
	double seco_thr = 100*CLHEP::MeV;

	// don't overwrite the values already in the strategy manager
	if(theTruthManager->GetTruthParameter("ComptonPrimaryMinEnergy")==0){
	  log()<<MSG::INFO<<"ComptonPrimaryMinEnergy: setting default value of 100 MeV"<<std::endl;  
	  theTruthManager->SetTruthParameter("ComptonPrimaryMinEnergy",prim_thr);
	}
	if(theTruthManager->GetTruthParameter("ComptonSecondaryMinEnergy")==0){
	  log()<<MSG::INFO<<"ComptonSecondaryMinEnergy: setting default value of 100 MeV"<<std::endl;  
	  theTruthManager->SetTruthParameter("ComptonSecondaryMinEnergy",seco_thr);
	}
}

bool ComptonStrategy::AnalyzeVertex(const G4Step* aStep)
{

	static StepHelper sHelper;
	static double eMinPrimary=theTruthManager->GetTruthParameter("ComptonPrimaryMinEnergy");
	static double eMinSecondary=theTruthManager->GetTruthParameter("ComptonSecondaryMinEnergy");
	sHelper.SetStep(aStep);
	G4int pSubType = sHelper.GetProcessSubType();

	if(pSubType==fComptonScattering)
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
		if (tracks.size()!=1) log()<<MSG::WARNING<<" ComptonStrategy: something is VERY wrong then!"<<std::endl;
                trackKEner= tracks[0]->GetMomentum().perp();
		if (trackKEner<eMinSecondary) 
		{
			return true;
		}
		
		// we register the vertex using TruthManager's appropriate method
		theTruthManager->SaveSecondaryVertex(aStep->GetTrack(),
					             aStep->GetPostStepPoint(),
						     tracks);
		return true;
	}
	else return false;
}
