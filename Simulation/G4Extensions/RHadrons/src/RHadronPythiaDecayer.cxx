/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header for my class
#include "RHadronPythiaDecayer.h"

// The actual decayers.  Singleton classes, not toolhandles
#include "G4ExternalDecay/PythiaForDecays.h"
#include "G4ExternalDecay/Pythia8ForDecays.h"

// For passing things around
#include "CLHEP/Vector/LorentzVector.h"
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4DecayProducts.hh"

G4DecayProducts* RHadronPythiaDecayer::ImportDecayProducts(const G4Track& aTrack){
  G4cout << "Jenn testing RHadrons here" << G4endl;
  G4DecayProducts * dp = new G4DecayProducts();
  dp->SetParentParticle( *(aTrack.GetDynamicParticle()) );

  // get properties for later print outs
  G4double etot = aTrack.GetDynamicParticle()->GetTotalEnergy();
  G4int pdgEncoding = aTrack.GetDefinition()->GetPDGEncoding();

  // Outgoing particle
  std::vector<G4DynamicParticle*> particles;

  // Different approaches depending on whether we care about Pythia8 or Pythia6
  if(m_usePythia8){
    // Pythia8 decay the particle and import the decay products
    Pythia8ForDecays::Instance()->Py1ent(aTrack, particles);
  }
  else{
    // let Pythia6Decayer decay the particle and import the decay products
    G4ThreeVector momentum = aTrack.GetMomentum();
    CLHEP::HepLorentzVector p;
    p[0] = momentum.x() / CLHEP::GeV;
    p[1] = momentum.y() / CLHEP::GeV;
    p[2] = momentum.z() / CLHEP::GeV;
    p[3] = etot         / CLHEP::GeV;

    PythiaForDecays::Instance()->Py1ent(0, pdgEncoding, p.e(), p.theta(), p.phi());
    PythiaForDecays::Instance()->DecayRHadron(pdgEncoding);
    PythiaForDecays::Instance()->ImportParticles(particles);
  }

  G4cout << "Decayed an RHadron with ID " << pdgEncoding << " and momentum " << aTrack.GetMomentum() << " in Pythia.  Decay products are:" << G4endl;
  double totalE=0.0;
  for (unsigned int i=0; i<particles.size(); ++i){
    if (particles[i]) {
      G4cout<<i<<" "; dp->PushProducts(particles[i]);
      totalE += particles[i]->GetTotalEnergy();
    }
    else {
      G4cout << i << " null pointer!" << G4endl;
    }
  }
  G4cout << G4endl;
  //G4cout <<"\nSummary:" << G4endl;
  G4cout<<" total energy in was "<<etot<<G4endl;
  G4cout<<" total energy out is "<<totalE<<G4endl;
  //G4cout << G4endl;
  dp->DumpInfo();

  return dp;
}
