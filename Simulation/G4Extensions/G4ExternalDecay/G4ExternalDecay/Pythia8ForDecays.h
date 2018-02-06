/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Abused from Geant4 version of Pythai6.hh from extended examples

#ifndef Pythia8ForDecays_H
#define Pythia8ForDecays_H

#include <vector>
#include "G4Track.hh"

// For all the various Pythia8 classes used here
#include "Pythia8_i/Pythia8_i.h"

class G4DynamicParticle;
class G4ParticleDefinition;

class Pythia8ForDecays
{
  public:

   Pythia8ForDecays();
   virtual ~Pythia8ForDecays();

   static Pythia8ForDecays *Instance();

   void Py1ent(const G4Track&, std::vector<G4DynamicParticle*> &);

  private:

   G4ParticleDefinition* GetParticleDefinition(const int) const;
   void fillParticle(const G4Track&, Pythia8::Event& event) const;
   std::pair<int,int> fromIdWithGluino( int idRHad, Pythia8::Rndm* rndmPtr) const;

   // My own class; singleton pattern
   static  Pythia8ForDecays* s_instance;

   // The instance of Pythia8 that will do the work
   Pythia8::Pythia *m_pythia;
};

#endif

