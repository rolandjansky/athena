/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Abused from Pythia6.cc in Geant4 extended examples

// My own class definition
#include "G4ExternalDecay/Pythia8ForDecays.h"

// Helper functions for Pythia8 and Pythia8 classes
#include "Pythia8_i/Pythia8_i.h"

// Pythia8 RHadrons code that we call into
#include "Pythia8/RHadrons.h"

// HepMC for translation into format Pythia likes
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

// G4 classes for translation into G4 format
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"
#include "TLorentzVector.h"
#include "MCUtils/HepMCUtils.h"

// STL includes
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <fstream>

std::unique_ptr<Pythia8ForDecays> Pythia8ForDecays::s_instance(nullptr);
std::once_flag Pythia8ForDecays::m_onceFlag;

Pythia8ForDecays* Pythia8ForDecays::Instance() {
  std::call_once(m_onceFlag,
        [] {
            s_instance.reset(new Pythia8ForDecays);
    });
  return s_instance.get();
}

Pythia8ForDecays::Pythia8ForDecays()
  {
  // Pythia instance where RHadrons can decay
  std::string docstring = Pythia8_i::xmlpath();
  m_pythia = std::make_unique<Pythia8::Pythia>(docstring);
  m_pythia->readString("SLHA:file = SLHA_INPUT.DAT");
  m_pythia->readString("ProcessLevel:all = off");
  m_pythia->readString("Init:showChangedSettings = off");
  m_pythia->readString("RHadrons:allow = on");
  m_pythia->readString("RHadrons:allowDecay = on");
  m_pythia->readString("RHadrons:probGluinoball = 0.1");
  m_pythia->readString("PartonLevel:FSR = off");
  m_pythia->readString("Init:showAllParticleData = on");

  // Process the file of commands left for us by the python layer
  std::string line;
  std::ifstream command_stream ("PYTHIA8_COMMANDS.TXT");
  while(getline(command_stream,line)){
    // Leaving it to the top-level to get this file right
    m_pythia->readString(line);
  }
  command_stream.close();

  m_pythia->init();

}

G4ParticleDefinition* Pythia8ForDecays::GetParticleDefinition(const int pdgEncoding) const
{
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDefinition(nullptr);
  if (pdgEncoding != 0) particleDefinition = particleTable->FindParticle(pdgEncoding);
  return particleDefinition;
}


std::pair<int,int> Pythia8ForDecays::fromIdWithSquark( int idRHad) const
{

  // Find squark flavour content.
  int idRSb            = m_pythia->settings.mode("RHadrons:idSbottom");
  int idRSt            = m_pythia->settings.mode("RHadrons:idStop");
  int idLight = (abs(idRHad) - 1000000) / 10;
  int idSq    = (idLight < 100) ? idLight/10 : idLight/100;
  int id1     = (idSq == 6) ? idRSt : idRSb;
  if (idRHad < 0) id1 = -id1;

  // Find light (di)quark flavour content.
  int id2     =  (idLight < 100) ? idLight%10 : idLight%100;
  if (id2 > 10) id2 = 100 * id2 + abs(idRHad)%10;
  if ((id2 < 10 && idRHad > 0) || (id2 > 10 && idRHad < 0)) id2 = -id2;

  // Done.
  return std::make_pair( id1, id2);

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

/// Add a G4Track to a Pythia8 event to make it a single-particle gun. The particle must be a colour singlet.
/// Input: particle, Pythia8 event
void Pythia8ForDecays::fillParticle(const G4Track& aTrack, Pythia8::Event& event) const
{
  // Reset event record to allow for new event.
  event.reset();

  // Select particle mass; where relevant according to Breit-Wigner.
  double mm = aTrack.GetDynamicParticle()->GetMass();

  // Store the particle in the event record.
  event.append( aTrack.GetDefinition()->GetPDGEncoding(), 1, 0, 0, aTrack.GetMomentum().x()/CLHEP::GeV, aTrack.GetMomentum().y()/CLHEP::GeV,
                aTrack.GetMomentum().z()/CLHEP::GeV, aTrack.GetDynamicParticle()->GetTotalEnergy()/CLHEP::GeV, mm/CLHEP::GeV);
  // Note: this function returns an int, but we don't need or use its output
}

bool Pythia8ForDecays::isGluinoRHadron(int pdgId) const{
  // Checking what kind of RHadron this is based on the digits in its PDGID
  const unsigned short digitValue_q1 = MCUtils::PID::_digit(MCUtils::PID::Location::nq1,pdgId);
  const unsigned short digitValue_l = MCUtils::PID::_digit(MCUtils::PID::Location::nl,pdgId);

  // Gluino R-Hadrons have the form 109xxxx or 1009xxx
  if (digitValue_l == 9 || (digitValue_l==0 && digitValue_q1 == 9) ){
    // This is a gluino R-Hadron
    return true;
  }

  // Special case : R-gluinoball
  if (pdgId==1000993) return true;

  // This is not a gluino R-Hadron (probably a squark R-Hadron)
  return false;

}

void Pythia8ForDecays::Py1ent(const G4Track& aTrack, std::vector<G4DynamicParticle*> & particles)
{

  // Get members from Pythia8 instance where RHadrons can decay
  Pythia8::Event& event      = m_pythia->event;
  Pythia8::ParticleData& pdt = m_pythia->particleData;

  // Use pythiaDecay information to fill event with the input particle
  fillParticle(aTrack, event);

  // Copy and paste of RHadron decay code
  int    iRNow  = 1;
  int    idRHad = event[iRNow].id();
  double mRHad  = event[iRNow].m();
  int    iR0    = 0;
  int    iR2    = 0;

  bool isTriplet = !isGluinoRHadron(idRHad);

  // Find flavour content of squark or gluino R-hadron.
  std::pair<int,int> idPair = (isTriplet) ? fromIdWithSquark( idRHad) : fromIdWithGluino( idRHad, &(m_pythia->rndm));
  int id1 = idPair.first;
  int id2 = idPair.second;

  // Sharing of momentum: the squark/gluino should be restored
  // to original mass, but error if negative-mass spectators.
  int idRSb            = m_pythia->settings.mode("RHadrons:idSbottom");
  int idRSt            = m_pythia->settings.mode("RHadrons:idStop");
  int idRGo            = m_pythia->settings.mode("RHadrons:idGluino");
  int idLight = (abs(idRHad) - 1000000) / 10;
  int idSq    = (idLight < 100) ? idLight/10 : idLight/100;
  int idRSq     = (idSq == 6) ? idRSt : idRSb;

  // Handling R-Hadrons with anti-squarks
  idRSq = idRSq * std::copysign(1, idRHad);

  int idRBef = isTriplet ? idRSq : idRGo;

  // Mass of the underlying sparticle
  double mRBef = pdt.mSel(idRBef);

  // Fraction of the RHadron mass given by the sparticle
  double fracR = mRBef / mRHad;
  int counter=0;
  while (fracR>=1.){
    if (counter==10){
      G4cout << "Needed more than 10 attempts with constituent " << idRBef << " mass (" << mRBef << " above R-Hadron " << idRHad << " mass " << mRHad << G4endl;
    } else if (counter>100){
      G4cout << "Pythia8ForDecays::Py1ent ERROR   Failed >100 times. Constituent " << idRBef << " mass (" << mRBef << " above R-Hadron " << idRHad << " mass " << mRHad << G4endl;
      return;
    }
    mRBef = pdt.mSel(idRBef);
    fracR = mRBef / mRHad;
    counter++;
  }

  // Squark case
  if(isTriplet){
    int col    = (event[idRBef].col() != 0) ? event.nextColTag() : 0;
    int tmpSparticleColor = id1>0 ? col : 0;
    int tmpSparticleAnticolor = id1>0 ? 0 : col;

    // Store the constituents of a squark R-hadron.

    // Sparticle
    // (id, status, mother1, mother2, daughter1, daughter2, col, acol, px, py, pz, e, m=0., scaleIn=0., polIn=9.)
    iR0 = event.append( id1, 106, iRNow, 0, 0, 0, tmpSparticleColor, tmpSparticleAnticolor, fracR * event[iRNow].p(), fracR * mRHad, 0.);
    // Spectator quark
    iR2 = event.append( id2, 106, iRNow, 0, 0, 0, tmpSparticleAnticolor, tmpSparticleColor, (1. - fracR) * event[iRNow].p(), (1. - fracR) * mRHad, 0.);
  }
  // Gluino case
  else{
    double mOffsetCloudRH = 0.2; // could be read from internal data?
    double m1Eff  = pdt.constituentMass(id1) + mOffsetCloudRH;
    double m2Eff  = pdt.constituentMass(id2) + mOffsetCloudRH;
    double frac1 = (1. - fracR) * m1Eff / ( m1Eff + m2Eff);
    double frac2 = (1. - fracR) * m2Eff / ( m1Eff + m2Eff);

    // Two new colours needed in the breakups.
    int col1 = event.nextColTag();
    int col2 = event.nextColTag();

    // Store the constituents of a gluino R-hadron.
    iR0 = event.append( idRBef, 106, iRNow, 0, 0, 0, col2, col1, fracR * event[iRNow].p(), fracR * mRHad, 0.);
    event.append( id1, 106, iRNow, 0, 0, 0, col1, 0, frac1 * event[iRNow].p(), frac1 * mRHad, 0.);
    iR2 = event.append( id2, 106, iRNow, 0, 0, 0, 0, col2, frac2 * event[iRNow].p(), frac2 * mRHad, 0.);
  }

  // Mark R-hadron as decayed and update history.
  event[iRNow].statusNeg();
  event[iRNow].daughters( iR0, iR2);

  // Generate events. Quit if failure.
  if (!m_pythia->next()) {
    m_pythia->forceRHadronDecays();
  }

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
      G4cout << "I don't know a definition for pdgid "<<m_pythia->event[i].id()<<"! Skipping it..." << G4endl;
      continue;
    }

    G4DynamicParticle* dynamicParticle = new G4DynamicParticle(particleDefinition, momentum);
    particles.push_back( dynamicParticle );
  }

}

