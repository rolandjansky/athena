/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/HadronicInteractionStrategy.h"

#include "MCTruthBase/TruthStrategyUtils.h"
//#include "SimHelpers/StepHelper.h"

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

HadronicInteractionStrategy::HadronicInteractionStrategy(const std::string& n)
  : TruthStrategy(n)
{
  double prim_thr = 100*CLHEP::MeV;
  if(theTruthManager->GetTruthParameter("HadronicPrimaryMinEnergy")==0){
    log() << MSG::INFO << "HadronicPrimaryMinEnergy: "
          << "setting default value of 100 MeV" << endmsg;
    theTruthManager->SetTruthParameter("HadronicPrimaryMinEnergy", prim_thr);
  }
  double seco_thr = 100*CLHEP::MeV;
  if(theTruthManager->GetTruthParameter("HadronicSecondaryMinEnergy")==0){
    log() << MSG::INFO << "HadronicSecondaryMinEnergy: "
          << "setting default value of 100 MeV" << endmsg;
    theTruthManager->SetTruthParameter("HadronicSecondaryMinEnergy", seco_thr);
  }
}

bool HadronicInteractionStrategy::AnalyzeVertex(const G4Step* aStep)
{
  static double eMinPrimary =
    theTruthManager->GetTruthParameter("HadronicPrimaryMinEnergy");
  static double eMinSecondary =
    theTruthManager->GetTruthParameter("HadronicSecondaryMinEnergy");

  // Thread-safety issue
  //static StepHelper sHelper;
  //sHelper.SetStep(aStep);
  //G4ProcessType pType = sHelper.GetProcess()->GetProcessType();

  G4ProcessType pType =
    aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessType();

  if(pType == fHadronic)
  {
    double trackKEner = aStep->GetPreStepPoint()->GetMomentum().perp();

    // Get the list of secondaries in the current step
    const auto& tracks = *aStep->GetSecondaryInCurrentStep();

    // In this case, if the input track is below threshold, we will still save
    // the vertex if at least one secondary is above the secondary threshold.
    if (trackKEner < eMinPrimary){
      // Check for de-excitation
      double maxSecMom = 0.;
      for (const auto track : tracks) {
        double thisTrackKEner = track->GetMomentum().perp();
        if (thisTrackKEner > maxSecMom) maxSecMom = thisTrackKEner;
      }
      // Bail if max secondary momentum is below threshold
      if (maxSecMom < eMinSecondary){
        return true;
      }
    }

    G4TrackStatus tStatus = aStep->GetTrack()->GetTrackStatus();
    G4Track* primTrack = (tStatus==fAlive) ? aStep->GetTrack() : nullptr;
    TruthStrategyUtils::saveSecondaryVertex(primTrack, aStep->GetPostStepPoint(), tracks);

    return true;
  }

  return false;
}
