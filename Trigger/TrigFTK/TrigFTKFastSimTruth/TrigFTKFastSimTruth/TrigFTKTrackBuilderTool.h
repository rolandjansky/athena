/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//implementation

#ifndef __TRIG_FTK_TRACK_BUILDER_TOOL_H__
#define __TRIG_FTK_TRACK_BUILDER_TOOL_H__

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/IAtRndmGenSvc.h"


#include "TrigFTKToolInterfaces/ITrigFTKTrackBuilderTool.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"


namespace HepPDT {
  class ParticleDataTable;
}

namespace CLHEP {
  class HepRandomEngine;
}

class FTKPlaneMap;

class TrigFTKTrackBuilderTool : public extends<AthAlgTool, ITrigFTKTrackBuilderTool>
{
public:
  TrigFTKTrackBuilderTool( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigFTKTrackBuilderTool() = default;
  virtual StatusCode initialize() override;

  std::unique_ptr<FTKTrack> createFTKTrack(const FTKTruthTrajectory& traj,
    const std::vector<std::unordered_map<int, int> >& sectorHitToPlaneMap,
    const std::vector<int>& regions,
    const std::vector<int>& sectors) const override;

private:
  bool smearTrack(bool hasIBLhit, FTKTrack *track, const HepMC::GenParticle* particle) const;

  std::unique_ptr<FTKTrack> createFTKTrackForSector(
    const FTKTruthTrajectory& traj,
    const std::unordered_map<int, int>& planeHits) const;

private:
  ToolHandle<Trk::IExtrapolator> m_extrapolator;
  ServiceHandle<IBeamCondSvc> m_beamSpotSvc;

  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;

  const HepPDT::ParticleDataTable* m_particleDataTable = nullptr;

  int m_nplanes;
  int m_ncoords;

  std::string m_pmap_path;
  std::unique_ptr<FTKPlaneMap> m_pmap;

  std::string m_randomStreamName;
  CLHEP::HepRandomEngine* m_randomEngine = nullptr;
};

#endif
