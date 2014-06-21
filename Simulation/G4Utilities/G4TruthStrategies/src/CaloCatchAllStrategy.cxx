/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/CaloCatchAllStrategy.h"
#include "SimHelpers/StepHelper.h"
#include "G4Step.hh"
#include <iostream>

#include "CLHEP/Units/PhysicalConstants.h"

static CaloCatchAllStrategy caloStrategy("CALOCatchAll");

CaloCatchAllStrategy::CaloCatchAllStrategy(const std::string n):
			TruthStrategy(n)
{

	if(theTruthManager->GetTruthParameter("CaloMuBremsPrimaryMinEnergy")==0){
	  log()<<MSG::INFO<<"CaloMuBremsPrimaryMinEnergy: setting default value of 1GeV"<<std::endl;
	  theTruthManager->SetTruthParameter("CaloMuBremsPrimaryMinEnergy", 1000.);
	}
	if(theTruthManager->GetTruthParameter("CaloMuBremsSecondaryMinEnergy")==0){
	  log()<<MSG::INFO<<"CaloMuBremsPrimaryMinEnergy: setting default value of 500MeV"<<std::endl;
	  theTruthManager->SetTruthParameter("CaloMuBremsSecondaryMinEnergy", 500.);
	}
	if(theTruthManager->GetTruthParameter("CaloDecayPrimaryMinEnergy")==0){
	  log()<<MSG::INFO<<"CaloDecayPrimaryMinEnergy: setting default value of 1GeV"<<std::endl;
	  theTruthManager->SetTruthParameter("CaloDecayPrimaryMinEnergy",1000.);
	}
}

bool CaloCatchAllStrategy::AnalyzeVertex(const G4Step* aStep)
{
	static StepHelper sHelper;
	sHelper.SetStep(aStep);
	
	G4int pSubType = sHelper.GetProcessSubType();
	G4int PDGcode = sHelper.ParticlePDGCode();
	if((PDGcode==13||PDGcode==-13) && pSubType==fBremsstrahlung)
	{
		
		static double eMuBremsMinPrimary=theTruthManager->GetTruthParameter("CaloMuBremsPrimaryMinEnergy");
		static double eMuBremsMinSecondary=theTruthManager->GetTruthParameter("CaloMuBremsSecondaryMinEnergy");
		
		//primary tracks
	        double trackKEner=aStep->GetPreStepPoint()->GetKineticEnergy()/CLHEP::MeV;
		if (trackKEner<eMuBremsMinPrimary){
	                return true;
	        }
		//secondary tracks
		std::vector<G4Track*> tracks =theTruthManager->GetSecondaries();
		if (tracks.size() != 1)
		{
			int trackToUse=tracks.size()-1;
			G4Track* temp=tracks[trackToUse];
			tracks.clear();
			tracks.push_back(temp);
		}
		if (tracks.size()!=1) log()<<MSG::WARNING<<" CaloCatchAllStrategy: something is VERY wrong then!"<<std::endl;
		trackKEner=tracks[0]->GetKineticEnergy();
		if (trackKEner<eMuBremsMinSecondary) return true;

		// we register the vertex using TruthManager's appropriate method
		theTruthManager->SaveSecondaryVertex(aStep->GetTrack(),
			aStep->GetPostStepPoint(),tracks);
		return true;
	}//MuBrems
	else return false;
	
	return true;
}
