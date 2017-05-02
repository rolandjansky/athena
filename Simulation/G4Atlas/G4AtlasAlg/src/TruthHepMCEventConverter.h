/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthHepMCEventConverter_H
#define TruthHepMCEventConverter_H

#include "FadsKinematics/HepMCEventConverter.h"
#include <vector>

class G4PrimaryParticle;
class G4PrimaryVertex;

// Helper class to convert HepMC into G4. this is used by the Generator
// Center, instead of the standard conversion helper

class TruthHepMCEventConverter: public FADS::HepMCEventConverter {
public:
  TruthHepMCEventConverter();
  ~TruthHepMCEventConverter();
  void HepMC2G4(const HepMC::GenEvent* ,G4Event *, const bool) const;
private:
  TruthHepMCEventConverter (const TruthHepMCEventConverter&);  //declared, but not defined
  TruthHepMCEventConverter& operator= (const TruthHepMCEventConverter&); //declared, but not defined
  // helper functions here
  bool SeparateVertices(HepMC::GenVertex* , HepMC::GenVertex*) const ;
  G4PrimaryVertex* ConstructG4PrimaryVertex(HepMC::GenVertex*) const ;
  G4PrimaryParticle* ConstructG4PrimaryParticle(HepMC::GenParticle*) const;
  HepMC::GenVertex* ConstructHepMCVertex(G4PrimaryVertex*) const ;
  void TransformHepMCParticle(HepMC::GenParticle*,G4PrimaryParticle*) const;
  void TransformHepMCVertex(HepMC::GenVertex*,G4PrimaryVertex*) const;
  void ModifyVertex(G4PrimaryParticle *pp, HepMC::GenVertex *v) const;
  bool ValidHepMCVertex(HepMC::GenVertex*) const;
  int m_printLev;
  std::vector<int> *m_modded;
};

#endif
