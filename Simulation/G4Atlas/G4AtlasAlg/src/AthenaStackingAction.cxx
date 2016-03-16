/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "G4AtlasAlg/AthenaStackingAction.h"
#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/EventInformation.h"
#include <iostream>
#include <string>

#include "G4VProcess.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4AntiNeutrinoTau.hh"

#include "G4Gamma.hh"

// static AthenaStackingAction stacking;

AthenaStackingAction::AthenaStackingAction(): ApplicationStackingAction(), p_killAllNeutrinos(false), p_stackEnergyCut(-1){}

G4ClassificationOfNewTrack AthenaStackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
    // Neutrinos turned off at job options level
	G4ParticleDefinition &particleType = *(aTrack->GetDefinition() );
	if (  p_killAllNeutrinos                                               &&
        ( &particleType == G4NeutrinoE::NeutrinoEDefinition()              ||
          &particleType == G4AntiNeutrinoE::AntiNeutrinoEDefinition()      ||
          &particleType == G4NeutrinoMu::NeutrinoMuDefinition()            ||
          &particleType == G4AntiNeutrinoMu::AntiNeutrinoMuDefinition()    ||
          &particleType == G4NeutrinoTau::NeutrinoTauDefinition()          ||
          &particleType == G4AntiNeutrinoTau::AntiNeutrinoTauDefinition()  ) ){
	  return fKill;
	}

    if ( &particleType == G4Gamma::Gamma() &&
      aTrack->GetTotalEnergy() < 0.00005 ){ // Safe cut for ultra-low-energy photons
      return fKill;
    }


	G4Track *inT=const_cast<G4Track*>(aTrack);

	G4Event *ev=G4EventManager::GetEventManager()->GetNonconstCurrentEvent();
	EventInformation* eventInfo __attribute__ ((unused)) =static_cast<EventInformation*>
				    (ev->GetUserInformation());

	int pID=aTrack->GetParentID();
	if (!pID)	// this is a primary particle.
	{
		const G4PrimaryParticle *pp=0;
		PrimaryParticleInformation *ppi=0;
		
		const G4DynamicParticle *dp=aTrack->GetDynamicParticle();
		if (dp)  pp=(const G4PrimaryParticle *)dp->GetPrimaryParticle();
		if (pp)
		ppi=dynamic_cast
			<PrimaryParticleInformation *> (pp->GetUserInformation());
		if (ppi) {
			const HepMC::GenParticle *part=ppi->GetHepMCParticle();
			bool hasISFParticle = ppi->GetISFParticle()!=nullptr;
			if (!hasISFParticle) {
				// don't to anything
				if (part) {
				// OK, we got back to HepMC
					TrackInformation *ti=new TrackInformation(part);
					ti->SetRegenerationNr(0); 
					// regNr=0 and classify=Primary are default values anyway
					inT->SetUserInformation(ti);
					ti->SetClassification(Primary);

				}
				else if (ppi->GetParticleBarcode()>=0) {
					// PrimaryParticleInformation should at least provide a barcode
					TrackBarcodeInfo *bi=new TrackBarcodeInfo(ppi->GetParticleBarcode());
					inT->SetUserInformation(bi);
				}
			} // no ISFParticle attached
		} // has PrimaryParticleInformation
	}
	else		// secondary track: see if it must be saved
	{

          // Time cut for particles stacking after a certain time...
          if (  p_stackEnergyCut > 0 &&
                &particleType == G4Gamma::Gamma() &&
                aTrack->GetTotalEnergy() < p_stackEnergyCut ){
	    return fKill;
          }

 	}
        return fUrgent;
}
void AthenaStackingAction::NewStage() {;}
void AthenaStackingAction::PrepareNewEvent() {;}

