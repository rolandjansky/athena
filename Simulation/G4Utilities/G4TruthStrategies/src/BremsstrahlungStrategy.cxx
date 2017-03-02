/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/BremsstrahlungStrategy.h"

#include "MCTruthBase/TruthStrategyUtils.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4EmProcessSubType.hh"

#include <iostream>
#include <vector>
#include <cassert>

#include "CLHEP/Units/PhysicalConstants.h"

static BremsstrahlungStrategy Strategy("Bremsstrahlung");

BremsstrahlungStrategy::BremsstrahlungStrategy(const std::string& n)
  : TruthStrategy(n)
{
  double prim_thr = 100*CLHEP::MeV;
  double seco_thr = 100*CLHEP::MeV;

  // don't overwrite the values already in the strategy manager
  if(theTruthManager->GetTruthParameter("BremsPrimaryMinEnergy")==0){
    log() << MSG::INFO << "BremsPrimaryMinEnergy: setting default value of 100 MeV"
          << endmsg;
    theTruthManager->SetTruthParameter("BremsPrimaryMinEnergy", prim_thr);
  }
  if(theTruthManager->GetTruthParameter("BremsSecondaryMinEnergy")==0){
    log() << MSG::INFO << "BremsSecondaryMinEnergy: setting default value of 100 MeV"
          << endmsg;
    theTruthManager->SetTruthParameter("BremsSecondaryMinEnergy", seco_thr);
  }
}

bool BremsstrahlungStrategy::AnalyzeVertex(const G4Step* aStep)
{
  static double eMinPrimary =
    theTruthManager->GetTruthParameter("BremsPrimaryMinEnergy");
  static double eMinSecondary =
    theTruthManager->GetTruthParameter("BremsSecondaryMinEnergy");

  // Thread-safety issue
  //static StepHelper sHelper;
  //sHelper.SetStep(aStep);
  //G4int pSubType = sHelper.GetProcessSubType();

  G4int pSubType =
    aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType();

  if(pSubType==fBremsstrahlung)
  {
    // Check parent particle threshold
    double trackKEner = aStep->GetPreStepPoint()->GetMomentum().perp();
    if (trackKEner < eMinPrimary) return true;

    // Get the list of secondaries in the current step
    const auto& tracks = *aStep->GetSecondaryInCurrentStep();

    // In the current implementation (subject to change; see ATLASSIM-2409),
    // we save only the photon produced by Brems, dropping any additional
    // TRT transition radiation (XTR) photons. We should, however, move to
    // saving all secondaries that are above secondary threshold.
    std::vector<const G4Track*> saveTracks;

    // Assume vector ordering gives the brem first on the list
    assert( tracks.size() ); // optimized away in opt build
    saveTracks.push_back( tracks[0] );

    // Grab the brems photon by checking its creator process.
    //for (auto trk : tracks) {
    //  auto proc = trk->GetCreatorProcess();
    //  if (proc->GetProcessSubType() == fBremsstrahlung) {
    //    saveTracks.push_back(trk);
    //    break;
    //  }
    //}

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
