/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FixG4CreatorProcess.h"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4Gamma.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4EmParameters.hh"

namespace G4UA
{

  //---------------------------------------------------------------------------
  FixG4CreatorProcess::FixG4CreatorProcess(){}


  //---------------------------------------------------------------------------
  void FixG4CreatorProcess::UserSteppingAction(const G4Step* aStep)
  {
	//
	//  M. Novak, M. Bandieramonte: 10 November 2021
	//  
	//  Make sure that the creator process of the secondary tracks of the current step
	//  is the same as the one that limited the step. These can be different in case of
	//  using `wrapper` processes e.g. G4GammaGeneralProcess or G4HepEm, etc.
	if (G4EmParameters::Instance()->GeneralProcessActive()) {
		const G4ParticleDefinition* thePrimaryParticle = aStep->GetTrack()->GetParticleDefinition();
		const std::size_t           numThisSecondaries = aStep->GetNumberOfSecondariesInCurrentStep();
		if (thePrimaryParticle==G4Gamma::GammaDefinition() && numThisSecondaries>0) {
                // Get the pointer to the process that limited the step: i.e. the one that
                // created the secondaries of the current step
			const G4VProcess* theLimiterProcess = aStep->GetPostStepPoint()->GetProcessDefinedStep();
			// note: this is a vector of secondaries containing all secondaries created
			// along the tracking of the current `primary` track (i.e. not only
			// secondaries created in this step)
			const G4TrackVector* theSecTrackVector = aStep->GetSecondary();
			const std::size_t    numAllSecondaries = theSecTrackVector->size();
			for (std::size_t it = numAllSecondaries-numThisSecondaries; it<numAllSecondaries; ++it) {

				G4Track* secTrack = (*theSecTrackVector)[it];
				if (secTrack->GetCreatorProcess()!=theLimiterProcess) {

					secTrack->SetCreatorProcess(theLimiterProcess);
				}
      			}
    		}

  	}	
 }
} // namespace G4UA
