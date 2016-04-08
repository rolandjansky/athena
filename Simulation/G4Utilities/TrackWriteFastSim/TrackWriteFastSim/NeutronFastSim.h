/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKWRITEFASTSIM_NeutronFastSim_h
#define TRACKWRITEFASTSIM_NeutronFastSim_h

//Base class header
#include "G4VFastSimulationModel.hh"

//STL headers
#include <string>

class TrackFastSimSD;

class NeutronFastSim: public G4VFastSimulationModel
{
public:

  NeutronFastSim(const std::string& name, const std::string& fsSDname, const double etaCut, const double timeCut);
  ~NeutronFastSim() {}

  // Fast Sim Methods
  G4bool IsApplicable(const G4ParticleDefinition&) override final;
  virtual G4bool ModelTrigger(const G4FastTrack &) override final;
  void DoIt(const G4FastTrack&, G4FastStep&) override final;

protected:

  G4double m_Energy;
  TrackFastSimSD * m_fsSD;
  bool m_init;
  std::string m_fsSDname;
  double m_etaCut;
  double m_timeCut;
};

#endif //TRACKWRITEFASTSIM_NeutronFastSim_h
