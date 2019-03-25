/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Abused from Geant4 version of Pythai6.hh from extended examples

#ifndef Pythia8ForDecays_H
#define Pythia8ForDecays_H

// For std::vector
#include <vector>
// G4 track function parameter
#include "G4Track.hh"
// For std::pair
#include <utility>
// For all the various Pythia8 classes used here
#include "Pythia8_i/Pythia8_i.h"
// For unique_ptr and once flag
#include <memory>
#include <mutex>

class G4DynamicParticle;
class G4ParticleDefinition;

class Pythia8ForDecays
{
  public:

   virtual ~Pythia8ForDecays() = default;

   static Pythia8ForDecays *Instance();

   /// Function that decays the RHadron; returns products in G4 format
   void Py1ent(const G4Track&, std::vector<G4DynamicParticle*> &);

  private:

   /// Private constructor for singleton pattern
   Pythia8ForDecays();

   /// Helper for getting G4ParticleDefinition from PDG ID
   G4ParticleDefinition* GetParticleDefinition(const int) const;

   /// Fill a Pythia8 event with the information from a G4Track
   void fillParticle(const G4Track&, Pythia8::Event& event) const;

   /// Get the quarks from a gluino R-hadron.  From Pythia8 code.
   std::pair<int,int> fromIdWithGluino( int idRHad, Pythia8::Rndm* rndmPtr) const;
   std::pair<int,int> fromIdWithSquark( int idRHad) const;
   bool isGluinoRHadron(int pdgId) const;

   /// My own class; singleton pattern; thread safe for future-proofing
   static std::unique_ptr<Pythia8ForDecays> s_instance;
   static std::once_flag m_onceFlag;

   /// The instance of Pythia8 that will do the work
   std::unique_ptr<Pythia8::Pythia> m_pythia;
};

#endif

