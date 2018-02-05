/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Abused from Geant4 version of Pythai6.hh from extended examples

#ifndef Pythia8ForDecays_H
#define Pythia8ForDecays_H

#include <vector>
#include "G4DynamicParticle.hh"
#include "Pythia8_i/Pythia8_i.h"

class Pythia8ForDecays
{
  public:

   Pythia8ForDecays();
   virtual ~Pythia8ForDecays();

   static Pythia8ForDecays *Instance();

   void Py1ent(int pdgid, double px, double py, double pz, double e, double m, std::vector<G4DynamicParticle*> &);

  private:

   G4ParticleDefinition* GetParticleDefinition(const int) const;
   void fillParticle(int id, double ee, double thetaIn, double phiIn,
                     Pythia8::Event& event, Pythia8::ParticleData& pdt, Pythia8::Rndm& rndm) const;
   std::pair<int,int> fromIdWithGluino( int idRHad, Pythia8::Rndm* rndmPtr) const;

   static  Pythia8ForDecays* s_instance;
};

#endif

