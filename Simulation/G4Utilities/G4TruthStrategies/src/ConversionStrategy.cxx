/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/ConversionStrategy.h"
#include "SimHelpers/StepHelper.h"
#include "G4Step.hh"
#include <iostream>

#include "CLHEP/Units/PhysicalConstants.h"

static ConversionStrategy Strategy("Conversion");

ConversionStrategy::ConversionStrategy(const std::string n):
			TruthStrategy(n)
{
	double prim_thr = 100*CLHEP::MeV;
        double seco_thr = 100*CLHEP::MeV;
	if(theTruthManager->GetTruthParameter("ConversionPrimaryMinEnergy")==0){
	  log()<<MSG::INFO<<"ConversionPrimaryMinEnergy: setting default value of 100 MeV"<<std::endl;  
	  theTruthManager->SetTruthParameter("ConversionPrimaryMinEnergy",prim_thr);
	}
	if(theTruthManager->GetTruthParameter("ConversionSecondaryMinEnergy")==0){
	  log()<<MSG::INFO<<"ConversionSecondaryMinEnergy: setting default value of 100 MeV"<<std::endl;  
	  theTruthManager->SetTruthParameter("ConversionSecondaryMinEnergy",seco_thr);
	}	
}

bool ConversionStrategy::AnalyzeVertex(const G4Step* aStep)
{
	static StepHelper sHelper;
	static double eMinPrimary=theTruthManager->GetTruthParameter("ConversionPrimaryMinEnergy");
	static double eMinSecondary=theTruthManager->GetTruthParameter("ConversionSecondaryMinEnergy");
	
	sHelper.SetStep(aStep);
	G4int pSubType = sHelper.GetProcessSubType();
	if(pSubType==fGammaConversion ||
           pSubType==fGammaConversionToMuMu ||
           pSubType==fPairProdByCharged)
	{
                double trackKEner= aStep->GetPreStepPoint()->GetMomentum().perp();
		if (trackKEner<eMinPrimary) return true;

		std::vector<G4Track*> tracks =theTruthManager->GetSecondaries();
		double secondaryMom = 0.;
		for (unsigned int i=0;i<tracks.size();i++){
	                double thisTrackKEner= tracks[i]->GetMomentum().perp();
			if (thisTrackKEner>secondaryMom) secondaryMom = thisTrackKEner;
		}
		if (secondaryMom<eMinSecondary){
		  return true;
		}
	
		// we register the vertex using TruthManager's appropriate method
		theTruthManager->SaveSecondaryVertex(0,
					     aStep->GetPostStepPoint(),
					     tracks);
	
		return true;
	
	}
	else return false;
	
}
