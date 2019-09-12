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

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

namespace FTKSmearingConstants {
  const int nIBL = 2;
  const int neta = 5;
  const int nparam = 8;
  const int ntp = 5; // Ipt,eta,phi,d0,z0
  const int nRegion = nIBL*neta*nparam;
}

namespace HepPDT {
  class ParticleDataTable;
}

namespace CLHEP {
  class HepRandomEngine;
}

// double gaussian parameterisation
class dgparam {
public:
  double n1, m1, a1, b1; // core gaussian
  double n2, m2, a2, b2; // tail gaussian
};

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
  
  std::string m_smearingFilePath;
  std::string m_smearingFileName;

  dgparam m_smearing_parameters[FTKSmearingConstants::nIBL][FTKSmearingConstants::neta][FTKSmearingConstants::ntp];
};

#endif
