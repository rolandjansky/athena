/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryParticleInformation_H
#define PrimaryParticleInformation_H

#include "G4VUserPrimaryParticleInformation.hh"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"

namespace ISF {
  class ISFParticle;
}

class PrimaryParticleInformation: public G4VUserPrimaryParticleInformation {
public:
  PrimaryParticleInformation();
  PrimaryParticleInformation(HepMC::ConstGenParticlePtr, const ISF::ISFParticle* isp=0);
  HepMC::ConstGenParticlePtr GetHepMCParticle() const;
  int GetParticleBarcode() const;
  void SuggestBarcode(int bc);
  void SetParticle(HepMC::ConstGenParticlePtr);
  void Print() const {}
  int GetRegenerationNr() {return  m_regenerationNr;}
  void SetRegenerationNr(int i) {m_regenerationNr=i;}

  void SetISFParticle(const ISF::ISFParticle* isp);
  const ISF::ISFParticle* GetISFParticle() const;

private:
  HepMC::ConstGenParticlePtr m_theParticle{};
  const ISF::ISFParticle* m_theISFParticle{};

  int m_regenerationNr{0};
  int m_barcode{-1};
};

#endif
