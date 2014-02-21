/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DeadMaterialShower_h
#define DeadMaterialShower_h

#include <string>

#include "G4FastSimulation/FastSimModel.h"

class ILArG4FastSimSvc;

class DeadMaterialShower: public FastSimModel
{
 public:

  DeadMaterialShower(G4String);                    //!< Default constructor

  G4bool IsApplicable(const G4ParticleDefinition&);
  void DoIt(const G4FastTrack&, G4FastStep&);

  /** Determines the applicability of the fast sim model to this particular track.
  Checks that geometric location, energy, and particle type are within bounds.  Also checks for
  containment of the particle's shower within a specific detector region.*/
  G4bool ModelTrigger(const G4FastTrack &);

//  void initializeMyEnergy();			//!< Function for initialization

 private: 

  ILArG4FastSimSvc* m_fastSimSvc;                     //!< The particle bounds for this particular particle
  ILArG4FastSimSvc* fastSimSvc();                    //!< Accessor for particle energy bounds

};

#endif

