/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EGAMMAALGS_EMBREMCOLLECTIONBUILDER_H
#define EGAMMAALGS_EMBREMCOLLECTIONBUILDER_H
/**
  @class EMBremCollectionBuilder
  Algorithm which creates new brem-refitted tracks
  */

#include "egammaInterfaces/IegammaTrkRefitterTool.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkTrack/TrackCollection.h"

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

class EMBremCollectionBuilder : public AthAlgorithm 
{
  /*Helper struct, 
   * added here mainly for
   * scopping
   */

  struct localCounter{
    unsigned int failedFitTracks{0}; 
    unsigned int refittedTracks{0};
    unsigned int failedSiliconRequirFit{0};
  };

public:
  /** @brief Default constructor*/
  EMBremCollectionBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute() override final;

private:

  /** @brief Refit of track */
  StatusCode refitTrack(const xAOD::TrackParticle* tmpTrkPart , 
                        TrackCollection* finalTracks, 
                        xAOD::TrackParticleContainer* finalTrkPartContainer,
                        localCounter& counter) const;

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_trkRefitTool {this,
    "TrackRefitTool", "ElectronRefitterTool", "Track refitter tool"};

  /** @brief Tool to create track particle */
  ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool {this,
    "TrackParticleCreatorTool", 
    "TrackParticleCreatorTool", 
    "TrackParticle creator tool"};

  /** @brief Tool to slim tracks  */
  ToolHandle<Trk::ITrackSlimmingTool>  m_slimTool {this,
    "TrackSlimmingTool", "TrkTrackSlimmingTool", "Track slimming tool"};

  /** @brief Tool for Track summary  */
  ToolHandle<Trk::ITrackSummaryTool>   m_summaryTool {this,
    "TrackSummaryTool", "InDetTrackSummaryTool", "Track summary tool"};

  /** @brief Tool for extrapolation */
  ToolHandle<IEMExtrapolationTools> m_extrapolationTool {this,
    "ExtrapolationTool", "EMExtrapolationTools", "Extrapolation tool"};

  /** @brier Option to do truth*/
  Gaudi::Property<bool> m_doTruth {this, "DoTruth", false, "do truth"};

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleContainerKey {this,
    "TrackParticleContainerName", "InDetTrackParticles", 
    "Input InDet TrackParticles"};

  /** @brief Names of input output collections */
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_selectedTrackParticleContainerKey {this,
    "SelectedTrackParticleContainerName", "egammaSelectedTrackParticles", 
    "Input of Selected TrackParticles to refit"};

  SG::WriteHandleKey<xAOD::TrackParticleContainer> m_OutputTrkPartContainerKey {this,
    "OutputTrkPartContainerName", "GSFTrackParticles", 
    "Output refitted TrackParticles"};

  SG::WriteHandleKey<TrackCollection> m_OutputTrackContainerKey {this,
    "OutputTrackContainerName", "GSFTracks", "Output refitted Trk::Tracks"};

  /** @Cut on minimum silicon hits*/
  Gaudi::Property<int> m_MinNoSiHits {this, "minNoSiHits", 4, 
    "Minimum number of silicon hits on track before it is allowed to be refitted"};

  //counters
  mutable std::atomic_uint m_FailedFitTracks{0};
  mutable std::atomic_uint m_RefittedTracks{0};
  mutable std::atomic_uint m_FailedSiliconRequirFit{0};  

};
#endif //
