/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/IonizationStrategy.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruthBase/TruthStrategyUtils.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4EmProcessSubType.hh"

#include <iostream>
#include <vector>
#include <cassert>

#include "CLHEP/Units/PhysicalConstants.h"

static IonizationStrategy Strategy("Ionization");

IonizationStrategy::IonizationStrategy(const std::string& n)
  : TruthStrategy(n)
{
  double prim_thr = 100*CLHEP::MeV;
  double seco_thr = 100*CLHEP::MeV;
  if(theTruthManager->GetTruthParameter("IonizationPrimaryMinEnergy")==0){
    log() << MSG::INFO << "IonizationPrimaryMinEnergy: "
          << "setting default value of 100 MeV" << endmsg;
    theTruthManager->SetTruthParameter("IonizationPrimaryMinEnergy",prim_thr);
  }
  if(theTruthManager->GetTruthParameter("IonizationSecondaryMinEnergy")==0){
    log() << MSG::INFO << "IonizationSecondaryMinEnergy: "
          << "setting default value of 100 MeV" << endmsg;
    theTruthManager->SetTruthParameter("IonizationSecondaryMinEnergy",seco_thr);
  }
}

bool IonizationStrategy::AnalyzeVertex(const G4Step* aStep)
{
  static double eMinPrimary =
    theTruthManager->GetTruthParameter("IonizationPrimaryMinEnergy");
  static double eMinSecondary =
    theTruthManager->GetTruthParameter("IonizationSecondaryMinEnergy");

  // This causes a data-race in multi-threading.
  // Static objects that hold state are generally not thread-safe.
  //static StepHelper sHelper;
  //sHelper.SetStep(aStep);
  //G4int pSubType = sHelper.GetProcessSubType();

  G4int pSubType =
    aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType();

  if(pSubType==fIonisation ||
     pSubType==fPhotoElectricEffect)
  {
    // Check parent particle threshold
    double trackKEner = aStep->GetPreStepPoint()->GetMomentum().perp();
    if (trackKEner < eMinPrimary) return true;

    // Get the list of secondaries in the current step
    const auto& tracks = *aStep->GetSecondaryInCurrentStep();

    // In the current implementation, we save only the first secondary on the
    // list. It's not clear if there are ever more secondaries in this strategy,
    // but to follow suit with BremsstrahlungStrategy we probably want to move
    // to saving all secondaries that are above secondary threshold.
    std::vector<const G4Track*> saveTracks;

    // Assume vector ordering
    assert( tracks.size() ); // optimized away in opt build
    saveTracks.push_back( tracks[0] );

    // Apply secondary threshold
    trackKEner = saveTracks[0]->GetMomentum().perp();
    if (trackKEner < eMinSecondary) return true;

    // Save the vertex using TruthStrategyUtils
    TruthStrategyUtils::saveSecondaryVertex(aStep->GetTrack(),
                                            aStep->GetPostStepPoint(),
                                            saveTracks);
    return true;
  }

  else return false;
}
