/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASTSIMULATION_DEADMATERIALSHOWER_H
#define G4FASTSIMULATION_DEADMATERIALSHOWER_H

#include "G4VFastSimulationModel.hh"

#include <string>
class G4FastStep;
class G4FastTrack;
class G4ParticleDefinition;

class DeadMaterialShower: public G4VFastSimulationModel
{
 public:

  DeadMaterialShower(const std::string& name, const double& highEnergy, const double& lowEnergy, const double& zcutoff);
  ~DeadMaterialShower() {}

  G4bool IsApplicable(const G4ParticleDefinition&) override final;
  void DoIt(const G4FastTrack&, G4FastStep&) override final;

  /** Determines the applicability of the fast sim model to this particular track.
  Checks that geometric location, energy, and particle type are within bounds.  Also checks for
  containment of the particle's shower within a specific detector region.*/
  G4bool ModelTrigger(const G4FastTrack &) override final;

 private:
  double m_highEnergy; ///!< Kill everything but muons up to this energy
  double m_lowEnergy; ///!< Kill only electrons and positrons up to this energy
  double m_zcutoff; ///!< Z-position to start killing everything but muons

};

#endif //G4FASTSIMULATION_DEADMATERIALSHOWER_H

