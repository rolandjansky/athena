/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKWRITEFASTSIM_StoppedParticleFastSim_h
#define TRACKWRITEFASTSIM_StoppedParticleFastSim_h

//Base class header
#include "G4VFastSimulationModel.hh"

//STL headers
#include <string>

class TrackFastSimSD;

class StoppedParticleFastSim: public G4VFastSimulationModel
{
public:

  StoppedParticleFastSim(const std::string& name, const std::string& fsSDname);
  ~StoppedParticleFastSim() {}

  // Fast Sim Methods
  G4bool IsApplicable(const G4ParticleDefinition&) override final;
  virtual G4bool ModelTrigger(const G4FastTrack &) override final;
  void DoIt(const G4FastTrack&, G4FastStep&) override final;

protected:

  bool isSUSYParticle(const int) const;
  TrackFastSimSD * m_fsSD;
  bool m_init;
  std::string m_fsSDname;
};

#endif // TRACKWRITEFASTSIM_StoppedParticleFastSim_h
