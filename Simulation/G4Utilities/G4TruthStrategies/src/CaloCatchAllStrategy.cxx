/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TruthStrategies/CaloCatchAllStrategy.h"

#include "MCTruthBase/TruthStrategyUtils.h"

#include "G4EmProcessSubType.hh"
#include "G4Step.hh"
#include "G4VProcess.hh"

#include <iostream>
#include <cassert>

#include "CLHEP/Units/PhysicalConstants.h"

static CaloCatchAllStrategy caloStrategy("CALOCatchAll");

CaloCatchAllStrategy::CaloCatchAllStrategy(const std::string& n)
  : TruthStrategy(n)
{
  if(theTruthManager->GetTruthParameter("CaloMuBremsPrimaryMinEnergy")==0){
    log() << MSG::INFO << "CaloMuBremsPrimaryMinEnergy: "
          << "setting default value of 1GeV" << endmsg;
    theTruthManager->SetTruthParameter("CaloMuBremsPrimaryMinEnergy", 1000.);
  }
  if(theTruthManager->GetTruthParameter("CaloMuBremsSecondaryMinEnergy")==0){
    log() << MSG::INFO << "CaloMuBremsSecondaryMinEnergy: "
          << "setting default value of 500MeV" << endmsg;
    theTruthManager->SetTruthParameter("CaloMuBremsSecondaryMinEnergy", 500.);
  }
  if(theTruthManager->GetTruthParameter("CaloDecayPrimaryMinEnergy")==0){
    log() << MSG::INFO << "CaloDecayPrimaryMinEnergy: "
          << "setting default value of 1GeV" << endmsg;
    theTruthManager->SetTruthParameter("CaloDecayPrimaryMinEnergy", 1000.);
  }
}

bool CaloCatchAllStrategy::AnalyzeVertex(const G4Step* aStep)
{
  // Thread-safety issue
  //static StepHelper sHelper;
  //sHelper.SetStep(aStep);
  //G4int pSubType = sHelper.GetProcessSubType();
  //G4int pdgCode = sHelper.ParticlePDGCode();

  G4int pSubType =
    aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType();
  G4int pdgCode =
    aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

  if((pdgCode==13||pdgCode==-13) && pSubType==fBremsstrahlung)
  {

    static double eMuBremsMinPrimary =
      theTruthManager->GetTruthParameter("CaloMuBremsPrimaryMinEnergy");
    static double eMuBremsMinSecondary =
      theTruthManager->GetTruthParameter("CaloMuBremsSecondaryMinEnergy");

    // Parent track threshold.
    // NOTE: Why do other strategies use the magnitude of GetMomentum()?
    double trackKEner = aStep->GetPreStepPoint()->GetKineticEnergy()/CLHEP::MeV;
    if (trackKEner < eMuBremsMinPrimary){
      return true;
    }

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
    trackKEner = saveTracks[0]->GetKineticEnergy();
    if (trackKEner < eMuBremsMinSecondary) return true;

    // Save the vertex using TruthStrategyUtils
    TruthStrategyUtils::saveSecondaryVertex(aStep->GetTrack(),
                                            aStep->GetPostStepPoint(),
                                            saveTracks);
    return true;
  }//MuBrems

  return false;
}
