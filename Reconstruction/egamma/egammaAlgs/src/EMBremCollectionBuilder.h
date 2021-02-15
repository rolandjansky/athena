/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EGAMMAALGS_EMBREMCOLLECTIONBUILDER_H
#define EGAMMAALGS_EMBREMCOLLECTIONBUILDER_H

#include "egammaInterfaces/IegammaTrkRefitterTool.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkTrack/TrackCollection.h"

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "TrkTrack/Track.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include <memory>


/**
 * @class EMBremCollectionBuilder
 * @brief Algorithm which creates new brem-refitted tracks
 * @author Christos Anastopoulos
 * @author Anthony Morley
 *
 * Input containers:
 * - TrackParticleContainerName (default=InDetTrackParticles): all the track particles
 * - SelectedTrackParticleContainerName (default=egammaSelectedTrackParticles): selected track particle to be refitted
 *
 * Output continers:
 * - OutputTrkPartContainerName (default=GSFTrackParticles): refitted track particle
 * - OutputTrackContainerName (default=GSFTracks): refitted slimmed tracks
 *
 * Only selected tracks with a minimum number of Si-hits (minNoSiHits) are refitted. The refitted
 * tracks, and the one that have not been refitted, are saved in the output containers.
 *
 * The GSF refitting can be done only when the full xAOD::Track is available (e.g. not in standard AOD).
 *
 * The refitting is delegated to a tool implementing the IegammaTrkRefitterTool interface,
 * configured with the TrackRefitTool property (by default ElectronRefitterTool).
 * The summary of the refitted tracks (which are new track objects) are updated with the
 * a tool implementing the ITrackSummaryTool interface, configured with the TrackSummaryTool
 * property (by default InDetTrackSummaryTool). The update is done for Pixel and SCT quanties
 * according to the property usePixel and useSCT (by default false).
 * The track particles are created from the xAOD::Track after refitting with a tool implementing
 * the Trk::ITrackParticleCreatorTool interface configured with the property TrackParticleCreatorTool
 * (by default TrackParticleCreatorTool).
 * Truth informations are copied from the original xAOD::TrackParticle.
 * The refitted tracks are saved after slimming with a tool implementing the Trk::ITrackSlimmingTool
 * interface, configurable with the TrackSlimmingTool property (by default TrkTrackSlimmingTool).
 */
class EMBremCollectionBuilder : public AthReentrantAlgorithm
{

public:
  EMBremCollectionBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

  /** Helper struct to store the track particle and the original index in the container. **/
  struct TrackWithIndex
  {

    TrackWithIndex(std::unique_ptr<Trk::Track> intrack, size_t index)
      : track(std::move(intrack))
      , origIndex(index)
    {}

    TrackWithIndex(const TrackWithIndex&) = delete;
    TrackWithIndex(TrackWithIndex&&) = default;
    TrackWithIndex& operator=(const TrackWithIndex&) = delete;
    TrackWithIndex& operator=(TrackWithIndex&&) = default;
    ~TrackWithIndex() = default;

    std::unique_ptr<Trk::Track> track;
    size_t origIndex;
  };

private:
  StatusCode refitTracks(const EventContext& ctx,
                         const std::vector<const xAOD::TrackParticle*>& input,
                         std::vector<TrackWithIndex>& refitted,
                         std::vector<TrackWithIndex>& failedfit) const;

  StatusCode createCollections(
    const EventContext& ctx,
    std::vector<TrackWithIndex>& refitted,
    std::vector<TrackWithIndex>& failedfit,
    std::vector<TrackWithIndex>& trtAlone,
    TrackCollection* finalTracks,
    xAOD::TrackParticleContainer* finalTrkPartContainer,
    const xAOD::TrackParticleContainer* AllTracks) const;

  StatusCode createNew(const EventContext& ctx,
                       TrackWithIndex& Info,
                       TrackCollection* finalTracks,
                       xAOD::TrackParticleContainer* finalTrkPartContainer,
                       const xAOD::TrackParticleContainer* AllTracks) const;

  void updateGSFTrack(const EventContext& ctx,
                      const TrackWithIndex& Info,
                      const xAOD::TrackParticleContainer* AllTracks) const;

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

  /** @brief Option to do truth*/
  Gaudi::Property<bool> m_doTruth {this, "DoTruth", false, "do truth"};

  /** @brief Option to do SCT holes estimation*/
  Gaudi::Property<bool> m_doSCT {this, "useSCT", false, "do SCT"};

  /** @brief Option to do truth*/
  Gaudi::Property<bool> m_doPix {this, "usePixel", false, "do Pix"};

  /** @Cut on minimum silicon hits*/
  Gaudi::Property<int> m_MinNoSiHits{ this,
                                      "minNoSiHits",
                                      4,
                                      "Minimum number of silicon hits on track "
                                      "before it is allowed to be refitted" };

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleContainerKey{
    this,
    "TrackParticleContainerName",
    "InDetTrackParticles",
    "Input InDet TrackParticles"
  };

  /** @brief Names of input output collections */
  SG::ReadHandleKey<xAOD::TrackParticleContainer>
    m_selectedTrackParticleContainerKey{ this,
                                         "SelectedTrackParticleContainerName",
                                         "egammaSelectedTrackParticles",
                                         "Input of Selected TrackParticles" };

  SG::WriteHandleKey<xAOD::TrackParticleContainer> m_OutputTrkPartContainerKey{
    this,
    "OutputTrkPartContainerName",
    "GSFTrackParticles",
    "Output GSF TrackParticles"
  };

  SG::WriteHandleKey<TrackCollection> m_OutputTrackContainerKey{
    this,
    "OutputTrackContainerName",
    "GSFTracks",
    "Output GSF Trk::Tracks"
  };

  SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> m_TruthParticlesLinkKey{
    this,
    "TrackParticleTruthDecorationName",
    "InDetTrackParticles.truthParticleLink",
    "Truth Decoration of input track particles"
  };

  // For P->T converters of ID tracks with Pixel
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection>
    m_pixelDetEleCollKey{ this,
                          "PixelDetEleCollKey",
                          "PixelDetectorElementCollection",
                          "Key of SiDetectorElementCollection for Pixel" };

  // For P->T converters of ID tracks with SCT
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection>
    m_SCTDetEleCollKey{ this,
                        "SCTDetEleCollKey",
                        "SCT_DetectorElementCollection",
                        "Key of SiDetectorElementCollection for SCT" };

  // counters for final statistics
  mutable std::atomic_uint m_FailedFitTracks{ 0 };
  mutable std::atomic_uint m_RefittedTracks{ 0 };
  mutable std::atomic_uint m_FailedSiliconRequirFit{ 0 };
};
#endif //
