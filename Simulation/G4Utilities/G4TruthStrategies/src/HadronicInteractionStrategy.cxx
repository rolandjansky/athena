/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/HadronicInteractionStrategy.h"
#include "SimHelpers/StepHelper.h"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4ProcessType.hh"
#include "G4VProcess.hh"
#include <iostream>
#include <vector>
#include <string>

#include "CLHEP/Units/PhysicalConstants.h"

static HadronicInteractionStrategy Strategy("HadronicInteraction");

HadronicInteractionStrategy::HadronicInteractionStrategy(const std::string n):
			TruthStrategy(n)
{
	double prim_thr = 100*CLHEP::MeV;
	if(theTruthManager->GetTruthParameter("HadronicPrimaryMinEnergy")==0){
	  log()<<MSG::INFO<<"HadronicPrimaryMinEnergy: setting default value of 100 MeV"<<std::endl;
	  theTruthManager->SetTruthParameter("HadronicPrimaryMinEnergy",prim_thr);
	}
        double seco_thr = 100*CLHEP::MeV;
        if(theTruthManager->GetTruthParameter("HadronicSecondaryMinEnergy")==0){
          log()<<MSG::INFO<<"HadronicSecondaryMinEnergy: setting default value of 100 MeV"<<std::endl;  
          theTruthManager->SetTruthParameter("HadronicSecondaryMinEnergy",seco_thr);
        }
}

bool HadronicInteractionStrategy::AnalyzeVertex(const G4Step* aStep)
{
	static double eMinPrimary=theTruthManager->GetTruthParameter("HadronicPrimaryMinEnergy");
	static double eMinSecondary=theTruthManager->GetTruthParameter("HadronicSecondaryMinEnergy");
	
 	static StepHelper sHelper;
 	sHelper.SetStep(aStep);
	G4ProcessType pType=sHelper.GetProcess()->GetProcessType();
	if(fHadronic==pType)
	{

	        double trackKEner= aStep->GetPreStepPoint()->GetMomentum().perp();
	        double secondaryMom = 0.;
	        if (trackKEner<eMinPrimary){
			// Check for de-excitation
	                std::vector<G4Track*> tracks =theTruthManager->GetSecondaries();
	                for (unsigned int i=0;i<tracks.size();i++){
	                        double thisTrackKEner= tracks[i]->GetMomentum().perp();
	                        if (thisTrackKEner>secondaryMom)secondaryMom=thisTrackKEner;
	                }
	                if (secondaryMom<eMinSecondary){
	                	return true;
	                }
		}
		G4TrackStatus tStatus=aStep->GetTrack()->GetTrackStatus();
		std::vector<G4Track*> tracks =theTruthManager->GetSecondaries();
		if (tStatus==fAlive)
			theTruthManager->SaveSecondaryVertex(aStep->GetTrack(),
 						             aStep->GetPostStepPoint(),
 							     	 tracks);
		else
			theTruthManager->SaveSecondaryVertex(0,
 						             aStep->GetPostStepPoint(),
 							     	 tracks);
		return true;
	}
		
	return false;
}
