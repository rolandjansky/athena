/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Abused from Geant4 version of Pythai6.hh from extended examples

#ifndef PythiaForDecays_H
#define PythiaForDecays_H

#include <vector>
#include "G4DynamicParticle.hh"

class PythiaForDecays
{
  public:

   PythiaForDecays();
   virtual ~PythiaForDecays();

   static PythiaForDecays *Instance();

   int  Pycomp(int);
   void Py1ent(int, int, double, double, double);

   void ImportParticles( std::vector<G4DynamicParticle*> &);

   void DecayRHadron(int pdgid);

  private:

   G4ParticleDefinition* GetParticleDefinition(const int) const;

   static  PythiaForDecays* s_instance;
};

#endif

