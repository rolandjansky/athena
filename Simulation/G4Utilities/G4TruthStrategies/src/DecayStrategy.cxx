/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/DecayStrategy.h"

#include "MCTruthBase/TruthStrategyUtils.h"

#include "G4Step.hh"
#include "G4VProcess.hh"
#include "G4EmProcessSubType.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <iostream>

static DecayStrategy Strategy("Decay");

DecayStrategy::DecayStrategy(const std::string& n)
  : TruthStrategy(n)
{
  double prim_thr = 100*CLHEP::MeV;
  double seco_thr = 100*CLHEP::MeV;
  if(theTruthManager->GetTruthParameter("DecayPrimaryMinEnergy")==0){
    log() << MSG::INFO << "DecayPrimaryMinEnergy: setting default value of 100 MeV"
          << endmsg;
    theTruthManager->SetTruthParameter("DecayPrimaryMinEnergy", prim_thr);
  }
  if(theTruthManager->GetTruthParameter("DecaySecondaryMinEnergy")==0){
    log() << MSG::INFO << "DecaySecondaryMinEnergy: setting default value of 100 MeV"
          << endmsg;
    theTruthManager->SetTruthParameter("DecaySecondaryMinEnergy", seco_thr);
  }
}

bool DecayStrategy::AnalyzeVertex(const G4Step* aStep)
{
  static double eMinPrimary =
    theTruthManager->GetTruthParameter("DecayPrimaryMinEnergy");
  static double eMinSecondary =
    theTruthManager->GetTruthParameter("DecaySecondaryMinEnergy");

  // Thread-safety issue
  //static StepHelper sHelper;
  //sHelper.SetStep(aStep);
  //G4int pSubType = sHelper.GetProcessSubType();

  G4int pSubType =
    aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType();

  // All kinds of decay processes are included here..................
  if( (pSubType>200 && pSubType<299) ||
      pSubType==fAnnihilation ||
      pSubType==fAnnihilationToMuMu ||
      pSubType==fAnnihilationToHadrons )
  {
    double trackKEner = aStep->GetPreStepPoint()->GetMomentum().perp();
    if (trackKEner < eMinPrimary) return true;

    // Get the list of secondaries in the current step
    const auto& tracks = *aStep->GetSecondaryInCurrentStep();

    // Find the highest momentum secondary track
    double maxSecMom = 0.;
    for (auto track : tracks) {
      double thisTrackKEner = track->GetMomentum().perp();
      if (thisTrackKEner > maxSecMom) maxSecMom = thisTrackKEner;
    }
    if (maxSecMom < eMinSecondary) return true;

    // Save this vertex and secondaries
    TruthStrategyUtils::saveSecondaryVertex(nullptr, aStep->GetPostStepPoint(), tracks);

    return true;
  }
  else return false;

}
