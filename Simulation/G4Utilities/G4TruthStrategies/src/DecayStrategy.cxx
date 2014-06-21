/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/DecayStrategy.h"
#include "SimHelpers/StepHelper.h"
//#include "MCTruth/EventInformation.h"
//#include "HepMC/GenParticle.h"
#include "G4Step.hh"
#include <iostream>

#include "CLHEP/Units/PhysicalConstants.h"

static DecayStrategy Strategy("Decay");

DecayStrategy::DecayStrategy(const std::string n):
			TruthStrategy(n)
{
	double prim_thr = 100*CLHEP::MeV;
	double seco_thr = 100*CLHEP::MeV;
	if(theTruthManager->GetTruthParameter("DecayPrimaryMinEnergy")==0){
	  log()<<MSG::INFO<<"DecayPrimaryMinEnergy: setting default value of 100 MeV"<<std::endl;
	  theTruthManager->SetTruthParameter("DecayPrimaryMinEnergy",prim_thr);
	}
	if(theTruthManager->GetTruthParameter("DecaySecondaryMinEnergy")==0){
	  log()<<MSG::INFO<<"DecaySecondaryMinEnergy: setting default value of 100 MeV"<<std::endl;
	  theTruthManager->SetTruthParameter("DecaySecondaryMinEnergy",seco_thr);
	}
}

bool DecayStrategy::AnalyzeVertex(const G4Step* aStep)
{
	
	static StepHelper sHelper;
	static double eMinPrimary=theTruthManager->GetTruthParameter("DecayPrimaryMinEnergy");
	static double  eMinSecondary=theTruthManager->GetTruthParameter("DecaySecondaryMinEnergy");
	
        sHelper.SetStep(aStep);
	G4int pSubType = sHelper.GetProcessSubType();
	if( (pSubType>200 && pSubType<299) || // All kinds of decay processes are included here..................
            pSubType==fAnnihilation ||
            pSubType==fAnnihilationToMuMu ||
            pSubType==fAnnihilationToHadrons )
	{
//		EventInformation* eventInfo=theTruthManager->GetEventInformation();
				
                double trackKEner= aStep->GetPreStepPoint()->GetMomentum().perp();
		
		if (trackKEner<eMinPrimary) return true;
		
		double secondaryMom = 0.;
		std::vector<G4Track*> tracks =theTruthManager->GetSecondaries();
                for (unsigned int i=0;i<tracks.size();i++){
                        double thisTrackKEner= tracks[i]->GetMomentum().perp();
                        if (thisTrackKEner>secondaryMom) secondaryMom = thisTrackKEner;
                }
                if (secondaryMom<eMinSecondary) return true;
	
		// we register the vertex using TruthManager's appropriate method
		theTruthManager->SaveSecondaryVertex(0,
					     	     aStep->GetPostStepPoint(),
					     	     tracks);
	
		return true;
	}
	else return false;
	
}
