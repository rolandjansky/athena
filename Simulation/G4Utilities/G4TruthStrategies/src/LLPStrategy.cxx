/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/LLPStrategy.h"
#include "SimHelpers/StepHelper.h"
#include "MCTruth/EventInformation.h"
#include "G4Step.hh"
#include "G4ProcessType.hh"
#include "G4VProcess.hh"
#include <iostream>

static LLPStrategy Strategy("LLP");

LLPStrategy::LLPStrategy(const std::string n)
  : TruthStrategy(n)
{;}

bool LLPStrategy::AnalyzeVertex(const G4Step* aStep)
{
  static StepHelper sHelper;

  sHelper.SetStep(aStep);
  G4int pSubType = sHelper.GetProcessSubType();
  if( (pSubType>200 && pSubType<299) ){ // All kinds of decay processes are included here...

    // Check if this is a sparticle - if not, return
    int pdg = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
    if ( !isSUSYParticle( abs(pdg) ) ) return false;

    // we register the vertex using TruthManager's appropriate method
    std::vector<G4Track*> tracks =theTruthManager->GetSecondaries();
    theTruthManager->SaveSecondaryVertex(0,aStep->GetPostStepPoint(),tracks);
    std::cout<<"ACHLLP: saved a truth vertex for pdg "<<pdg<<std::endl;
    return true;
  }

  G4ProcessType pType=sHelper.GetProcess()->GetProcessType();
  if(fUserDefined==pType){//save all interactions for user-defined processes, like rhadron interactions
    G4TrackStatus tStatus=aStep->GetTrack()->GetTrackStatus();
    std::vector<G4Track*> tracks =theTruthManager->GetSecondaries();

    if (tStatus==fAlive) theTruthManager->SaveSecondaryVertex(aStep->GetTrack(), aStep->GetPostStepPoint(), tracks);
    else theTruthManager->SaveSecondaryVertex(0, aStep->GetPostStepPoint(), tracks);

    int pdg = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
    std::cout<<"ACHLLP: saved a truth interaction fUserDefined for pdg "<<pdg<<std::endl;
    return true;
  }
  
  return false;	
}

bool LLPStrategy::isSUSYParticle(const int id) const
{
  return ( (id>1000000 && id<1000007) || (id>1000010 && id<1000017) || 
           (id>2000000 && id<2000007) || (id>2000010 && id<2000017) || // squarks and sleptons
           (id>1000020 && id<1000026) || (id>1000034 && id<1000040) || // higgses etc 
           id==1000512 || id==1000522 || id==1000991 || id==1000993 || // R-hadrons
           id==1000612 || id==1000622 || id==1000632 || id==1000642 || id==1000652 || id==1005211 ||
           id==1006113 || id==1006211 || id==1006213 || id==1006223 || id==1006311 ||
           id==1006313 || id==1006321 || id==1006323 || id==1006333 ||
           id==1009111 || id==1009113 || id==1009211 || id==1009213 || id==1009311 ||
           id==1009313 || id==1009321 || id==1009323 || id==1009223 || id==1009333 ||
           id==1092112 || id==1091114 || id==1092114 || id==1092212 || id==1092214 || id==1092224 ||
           id==1093114 || id==1093122 || id==1093214 || id==1093224 || id==1093314 || id==1093324 || id==1093334 );
}

