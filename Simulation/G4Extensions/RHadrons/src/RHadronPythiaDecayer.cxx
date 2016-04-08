/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RHadronPythiaDecayer.h"
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "CLHEP/Vector/LorentzVector.h"
#include "G4ExternalDecay/PythiaForDecays.h"
#include <iostream>

G4DecayProducts* RHadronPythiaDecayer::ImportDecayProducts(const G4Track& aTrack){
#ifdef USEG4DECAYPRODUCTS2
  G4DecayProducts2 * dp = new G4DecayProducts2[1000];//allocate enough space so we can store all the additional products
#else
  G4DecayProducts * dp = new G4DecayProducts();
#endif
  dp->SetParentParticle( *(aTrack.GetDynamicParticle()) );

  // get particle momentum
  G4ThreeVector momentum = aTrack.GetMomentum();
  G4double etot = aTrack.GetDynamicParticle()->GetTotalEnergy();
  CLHEP::HepLorentzVector p;
  p[0] = momentum.x() / CLHEP::GeV;
  p[1] = momentum.y() / CLHEP::GeV;
  p[2] = momentum.z() / CLHEP::GeV;
  p[3] = etot         / CLHEP::GeV;

  G4int pdgEncoding = aTrack.GetDefinition()->GetPDGEncoding();

  // let Pythia6Decayer decay the particle
  // and import the decay products
  PythiaForDecays::Instance()->Py1ent(0, pdgEncoding, p.e(), p.theta(), p.phi());
  PythiaForDecays::Instance()->DecayRhadrons(pdgEncoding);
  std::vector<G4DynamicParticle*> particles;
  PythiaForDecays::Instance()->ImportParticles(particles);

  std::cout << "Decayed an RHadron with ID " << pdgEncoding << " and momentum " << p << " in Pythia.  Decay products are:" << std::endl;
  double totalE=0.0;
  for (unsigned int i=0; i<particles.size(); ++i){
    if (particles[i]) {
      std::cout<<i<<" "; dp->PushProducts(particles[i]);
      totalE += particles[i]->GetTotalEnergy();
    }
    else {
      std::cout << i << " null pointer!" << std::endl;
    }
  }
  std::cout << std::endl;
  //std::cout <<"\nSummary:" << std::endl;
  std::cout<<" total energy in was "<<etot<<std::endl;
  std::cout<<" total energy out is "<<totalE<<std::endl;
  //std::cout << std::endl;
  //dp->DumpInfo();

  return dp;
}
