/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header for my class
#include "RHadronPythiaDecayer.h"

// The actual decayers.  Singleton classes, not toolhandles
#include "G4ExternalDecay/Pythia8ForDecays.h"

// For passing things around
#include "CLHEP/Vector/LorentzVector.h"
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4DecayProducts.hh"

RHadronPythiaDecayer::RHadronPythiaDecayer( const std::string s )
 : G4VExtDecayer(s)
{
  // In the constructor, make a decayer instance, so that it's initialized here and not in the event loop
  Pythia8ForDecays::Instance();
}

G4DecayProducts* RHadronPythiaDecayer::ImportDecayProducts(const G4Track& aTrack){
  G4DecayProducts * dp = new G4DecayProducts();
  dp->SetParentParticle( *(aTrack.GetDynamicParticle()) );

  // get properties for later print outs
  G4double etot = aTrack.GetDynamicParticle()->GetTotalEnergy();
  G4int pdgEncoding = aTrack.GetDefinition()->GetPDGEncoding();

  // Outgoing particle
  std::vector<G4DynamicParticle*> particles;

  // Pythia8 decay the particle and import the decay products
  Pythia8ForDecays::Instance()->Py1ent(aTrack, particles);

  G4cout << "Decayed an RHadron with ID " << pdgEncoding << " and momentum " << aTrack.GetMomentum() << " in Pythia.  Decay products are:" << G4endl;
  double totalE=0.0;
  for (unsigned int i=0; i<particles.size(); ++i){
    if (particles[i]) {
      dp->PushProducts(particles[i]);
      totalE += particles[i]->GetTotalEnergy();
    }
    else {
      G4cout << i << " null pointer!" << G4endl;
    }
  }

  G4cout << "Total energy in was "<<etot   << G4endl;
  G4cout << "Total energy out is "<<totalE << G4endl;

  dp->DumpInfo();

  return dp;
}
