/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Abused from Pythia6.cc in Geant4 extended examples

#include "G4ExternalDecay/Pythia8ForDecays.h"
#include "Pythia8_i/Pythia8_i.h"
#include "Pythia8/RHadrons.h"
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"
#include "TLorentzVector.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>


// TODO: Is this actually how we want to do this? 
// I suppose the idea would have been to not have to reinitialize Pythia8 every time, but that would require some rewriting of my code anyhow...
// and this seems a bit suspect
Pythia8ForDecays* Pythia8ForDecays::s_instance = 0;

Pythia8ForDecays* Pythia8ForDecays::Instance() {
  if ( ! s_instance ) s_instance = new Pythia8ForDecays();
  return s_instance;
}

Pythia8ForDecays::Pythia8ForDecays() {
  // Protect against multiple objects.   All access should be via the
  // Instance member function.
  if ( s_instance ) {
     std::cerr << "There's already an instance of Pythia8ForDecays" << std::endl;
     exit (1);
  }

  // Pythia instance where RHadrons can decay
  std::string docstring = Pythia8_i::xmlpath();
  m_pythia = new Pythia8::Pythia(docstring);
  m_pythia->readString("SLHA:file = SLHA_INPUT.DAT");
  m_pythia->readString("ProcessLevel:all = off");
  m_pythia->readString("Init:showChangedSettings = off");
  m_pythia->readString("RHadrons:allow = on");
  m_pythia->readString("RHadrons:allowDecay = on");
  m_pythia->readString("RHadrons:probGluinoball = 0.1");
  m_pythia->readString("PartonLevel:FSR = off");
  m_pythia->init();

}

G4ParticleDefinition* Pythia8ForDecays::GetParticleDefinition(const int pdgEncoding) const
{
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDefinition = 0;
  if (pdgEncoding != 0) particleDefinition = particleTable->FindParticle(pdgEncoding);
  return particleDefinition;
}

Pythia8ForDecays::~Pythia8ForDecays()
{
  if (m_pythia) delete m_pythia;
  m_pythia=nullptr;
}


// TODO: Would be nice for this to be a public function in Pythia8::RHadrons.hh
std::pair<int,int> Pythia8ForDecays::fromIdWithGluino( int idRHad, Pythia8::Rndm* rndmPtr) const 
{
  // Find light flavour content of R-hadron.
  int idLight = (abs(idRHad) - 1000000) / 10;
  int id1, id2, idTmp, idA, idB, idC;
  double diquarkSpin1RH = 0.5;

  // Gluinoballs: split g into d dbar or u ubar.
  if (idLight < 100) {
    id1 = (rndmPtr->flat() < 0.5) ? 1 : 2;
    id2 = -id1;

  // Gluino-meson: split into q + qbar.
  } else if (idLight < 1000) {
    id1 = (idLight / 10) % 10;
    id2 = -(idLight % 10);
    // Flip signs when first quark of down-type.
    if (id1%2 == 1) {
      idTmp = id1;
      id1   = -id2;
      id2   = -idTmp;
    }

  // Gluino-baryon: split to q + qq (diquark).
  // Pick diquark at random, except if c or b involved.
  } else {
    idA = (idLight / 100) % 10;
    idB = (idLight / 10) % 10;
    idC = idLight % 10;
    double rndmQ = 3. * rndmPtr->flat();
    if (idA > 3) rndmQ = 0.5;
    if (rndmQ < 1.) {
      id1 = idA;
      id2 = 1000 * idB + 100 * idC + 3;
      if (idB != idC && rndmPtr->flat() > diquarkSpin1RH) id2 -= 2;
    } else if (rndmQ < 2.) {
      id1 = idB;
      id2 = 1000 * idA + 100 * idC + 3;
      if (idA != idC && rndmPtr->flat() > diquarkSpin1RH) id2 -= 2;
    } else {
      id1 = idC;
      id2 = 1000 * idA + 100 * idB +3;
      if (idA != idB && rndmPtr->flat() > diquarkSpin1RH) id2 -= 2;
    }
  }
  // Flip signs for anti-R-hadron.
  if (idRHad < 0) {
    idTmp = id1;
    id1   = -id2;
    id2   = -idTmp;
  }

  // Done.
  return std::make_pair( id1, id2);

}

// Single-particle gun. The particle must be a colour singlet.
// Input: particle, pythia8 helpers
// Optional final argument to put particle at rest => E = m.
void Pythia8ForDecays::fillParticle(const G4Track& aTrack, Pythia8::Event& event, Pythia8::ParticleData& pdt) const
{
  // Reset event record to allow for new event.
  event.reset();

  // Select particle mass; where relevant according to Breit-Wigner.
  double mm = pdt.mSel(aTrack.GetDefinition()->GetPDGEncoding());

  // Store the particle in the event record.
  event.append( aTrack.GetDefinition()->GetPDGEncoding(), 1, 0, 0, aTrack.GetMomentum().x(), aTrack.GetMomentum().y(), 
                aTrack.GetMomentum().z(), aTrack.GetDynamicParticle()->GetTotalEnergy(), mm);
  // Note: this function returns an int, but we don't need or use its output
}

void Pythia8ForDecays::Py1ent(const G4Track& aTrack, std::vector<G4DynamicParticle*> & particles)
{

  // Get members from Pythia8 instance where RHadrons can decay
  Pythia8::Event& event      = m_pythia->event;
  Pythia8::ParticleData& pdt = m_pythia->particleData;

  // Pythia instance where RHadrons cannot decay - do we need this?
  std::string docstring = Pythia8_i::xmlpath();
  Pythia8::Pythia pythiaDecay(docstring);
  pythiaDecay.readString("SLHA:file = SLHA_INPUT.DAT");
  pythiaDecay.readString("ProcessLevel:all = off");
  pythiaDecay.readString("Init:showChangedSettings = off");
  pythiaDecay.readString("RHadrons:allow = on");
  pythiaDecay.readString("RHadrons:allowDecay = off");
  pythiaDecay.readString("RHadrons:probGluinoball = 0.1");
  pythiaDecay.readString("PartonLevel:FSR = off");
  Pythia8::ParticleData& pdtDecay = pythiaDecay.particleData;
  pythiaDecay.init();

  // Use pythiaDecay information to fill event with the input particle
  fillParticle( aTrack, event, pdtDecay);

  // Copy and past of RHadron decay code
  int    iRNow  = 1;
  int    idRHad = event[iRNow].id();
  double mRHad  = event[iRNow].m();
  int    iR0    = 0;
  int    iR2    = 0;

  // Find flavour content of squark or gluino R-hadron.
  // Note that for now, this only works with gluinos
  // A similar function for squarks may be found in the RHadrons.cc code in Pythia8
  std::pair<int,int> idPair = fromIdWithGluino( idRHad, &(m_pythia->rndm));
  int id1 = idPair.first;
  int id2 = idPair.second;

  // Sharing of momentum: the squark/gluino should be restored
  // to original mass, but error if negative-mass spectators.
  int idRGo = 1000021; // Todo: Hard-coded, but could (and should) be generalized
  
  double mRBef = pdt.mSel(idRGo);
  double fracR = mRBef / mRHad;
  if (fracR >= 1.) return;

  // Hard wired for gluino -- could be generalized
  double mOffsetCloudRH = 0.2; // could be read from internal data?
  double m1Eff  = pdt.constituentMass(id1) + mOffsetCloudRH;
  double m2Eff  = pdt.constituentMass(id2) + mOffsetCloudRH;
  double frac1 = (1. - fracR) * m1Eff / ( m1Eff + m2Eff);
  double frac2 = (1. - fracR) * m2Eff / ( m1Eff + m2Eff);

  // Two new colours needed in the breakups.
  int col1 = event.nextColTag();
  int col2 = event.nextColTag();

  // Store the constituents of a gluino R-hadron.
  iR0 = event.append( idRGo, 106, iRNow, 0, 0, 0, col2, col1, fracR * event[iRNow].p(), fracR * mRHad, 0.);
  event.append( id1, 106, iRNow, 0, 0, 0, col1, 0, frac1 * event[iRNow].p(), frac1 * mRHad, 0.);
  iR2 = event.append( id2, 106, iRNow, 0, 0, 0, 0, col2, frac2 * event[iRNow].p(), frac2 * mRHad, 0.);

  // Mark R-hadron as decayed and update history.
  event[iRNow].statusNeg();
  event[iRNow].daughters( iR0, iR2);

  // Generate events. Quit if failure.
  if (!m_pythia->next()) {
    m_pythia->forceRHadronDecays();
    event.list();
  }
  event.list();

  ///////////////////////////////////////////////////////////////////////////
  // Add the particles from the Pythia event into the GEANT particle vector
  ///////////////////////////////////////////////////////////////////////////
  particles.clear();

  for(int i=0; i<m_pythia->event.size(); i++){
    if ( m_pythia->event[i].status()<0 ) continue; // stable only
    G4ThreeVector momentum( m_pythia->event[i].px() , m_pythia->event[i].py() , m_pythia->event[i].pz() );
    momentum*=1000.0;//GeV to MeV
    const G4ParticleDefinition * particleDefinition = GetParticleDefinition( m_pythia->event[i].id() );

    if (!particleDefinition){
      std::cout<<"WARNING: I don't know a definition for pdgid "<<m_pythia->event[i].id()<<"! Skipping it..."<<std::endl;
      continue;
    }

    G4DynamicParticle* dynamicParticle = new G4DynamicParticle(particleDefinition, momentum);
    particles.push_back( dynamicParticle );
  }

}

