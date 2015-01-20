/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NeutronFastSim_h
#define NeutronFastSim_h

#include "G4FastSimulation/FastSimModel.h"
class TrackFastSimSD;

class NeutronFastSim: public FastSimModel
{
public:

  NeutronFastSim(G4String name) : FastSimModel(name) , m_Energy(5) , m_fsSD(0) , m_init(false) {}

  // Fast Sim Methods
  G4bool IsApplicable(const G4ParticleDefinition&);
  virtual G4bool ModelTrigger(const G4FastTrack &);
  void DoIt(const G4FastTrack&, G4FastStep&);

protected:

  G4double m_Energy;
  TrackFastSimSD * m_fsSD;
  bool m_init;
};

#endif
