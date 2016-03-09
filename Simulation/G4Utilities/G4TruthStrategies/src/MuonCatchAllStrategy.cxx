/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/MuonCatchAllStrategy.h"
//#include "SimHelpers/StepHelper.h"
#include "G4Step.hh"

#include <iostream>

static MuonCatchAllStrategy muonStrategy("DMuonCatchAll");

MuonCatchAllStrategy::MuonCatchAllStrategy(const std::string& n)
  : TruthStrategy(n)
{
  //std::cout<<"MCTruth::MuonCatchAllStrategy, building strategy "<<n<<std::endl;
  //std::cout<<" * the strategy "<<n<<
  //           " is not yet associated to a volume, is inactive!!."<<std::endl;
}

bool MuonCatchAllStrategy::AnalyzeVertex(const G4Step* /*aStep*/)
{
  //static StepHelper sHelper;
  //sHelper.SetStep(aStep);
  //std::cout<<"this is MuonCatchAllStrategy::AnalyzeVertex"<<std::endl;
  //std::cout<<"nr. of secondaries in the last step "<<
  //      theTruthManager->GetNrOfSecondaries()<<std::endl;
  //std::string pName=sHelper.GetProcessName();
  //std::cout<<pName<<" in MUON with "<<theTruthManager->GetNrOfSecondaries()<<"\n";
  //if("Decay"==pName)std::cout<<"Decay in MUON"<<std::endl;
  return true;
}
