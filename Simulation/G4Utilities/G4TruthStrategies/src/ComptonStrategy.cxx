/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/ComptonStrategy.h"

#include "MCTruthBase/TruthStrategyUtils.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4EmProcessSubType.hh"

#include <iostream>
#include <vector>
#include <cassert>

#include "CLHEP/Units/PhysicalConstants.h"

static ComptonStrategy Strategy("Compton");

ComptonStrategy::ComptonStrategy(const std::string& n)
  : TruthStrategy(n)
{
  double prim_thr = 100*CLHEP::MeV;
  double seco_thr = 100*CLHEP::MeV;

  // don't overwrite the values already in the strategy manager
  if(theTruthManager->GetTruthParameter("ComptonPrimaryMinEnergy")==0){
    log() << MSG::INFO << "ComptonPrimaryMinEnergy: setting default value of 100 MeV"
          << endmsg;
    theTruthManager->SetTruthParameter("ComptonPrimaryMinEnergy", prim_thr);
  }
  if(theTruthManager->GetTruthParameter("ComptonSecondaryMinEnergy")==0){
    log() << MSG::INFO << "ComptonSecondaryMinEnergy: setting default value of 100 MeV"
          << endmsg;
    theTruthManager->SetTruthParameter("ComptonSecondaryMinEnergy", seco_thr);
  }
}

bool ComptonStrategy::AnalyzeVertex(const G4Step* aStep)
{

  static double eMinPrimary =
    theTruthManager->GetTruthParameter("ComptonPrimaryMinEnergy");
  static double eMinSecondary =
    theTruthManager->GetTruthParameter("ComptonSecondaryMinEnergy");

  // Thread-safety issue
  //static StepHelper sHelper;
  //sHelper.SetStep(aStep);
  //G4int pSubType = sHelper.GetProcessSubType();

  G4int pSubType =
    aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType();

  if(pSubType==fComptonScattering)
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
