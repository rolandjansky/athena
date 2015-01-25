/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StoppedParticleFastSim_h
#define StoppedParticleFastSim_h

#include "G4FastSimulation/FastSimModel.h"
class TrackFastSimSD;

class StoppedParticleFastSim: public FastSimModel
{
public:

  StoppedParticleFastSim(G4String name) : FastSimModel(name) , m_fsSD(0) , m_init(false) {}

  // Fast Sim Methods
  G4bool IsApplicable(const G4ParticleDefinition&);
  virtual G4bool ModelTrigger(const G4FastTrack &);
  void DoIt(const G4FastTrack&, G4FastStep&);

protected:

  bool isSUSYParticle(const int) const;
  TrackFastSimSD * m_fsSD;
  bool m_init;
};

#endif
