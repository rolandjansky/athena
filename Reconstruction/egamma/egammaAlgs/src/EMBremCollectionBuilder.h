/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EGAMMAALGS_EMBREMCOLLECTIONBUILDER_H
#define EGAMMAALGS_EMBREMCOLLECTIONBUILDER_H

#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "egammaInterfaces/IegammaTrkRefitterTool.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkTrack/TrackCollection.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "TrkTrack/Track.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

#include <memory>

/**
 * @class EMBremCollectionBuilder
 * @brief Algorithm which creates new brem-refitted tracks
 * @author Christos Anastopoulos
 * @author Anthony Morley
 *
 * Input containers:
 * - TrackParticleContainerName (default=InDetTrackParticles): all the track
 * particles
 * - SelectedTrackParticleContainerName (default=egammaSelectedTrackParticles):
 * selected track particle to be refitted
 *
 * Output continers:
 * - OutputTrkPartContainerName (default=GSFTrackParticles): refitted track
 * particle
 * - OutputTrackContainerName (default=GSFTracks): refitted slimmed tracks
 *
 * Only tracks with a minimum number of Si-hits (minNoSiHits) are
 * refitted. The refitted Trk::Tracks, and "copies" of the selected but not
 * refitted Trk:;Tracks (i.e TRT alone), are saved in the output containers.
 *
 * The GSF refitting can be done only when the full xAOD::Track is available
 * (e.g. not in standard AOD). The refitting is delegated to a tool implementing
 * the IegammaTrkRefitterTool interface, configured with the TrackRefitTool
 * property (by default ElectronRefitterTool).
 *
 *
 * The final track particles are created from  the Trk::Tracks (refitted or
 * copied) with a tool implementing the Trk::ITrackParticleCreatorTool interface
 * configured with the property TrackParticleCreatorTool (by default
 * TrackParticleCreatorTool).
 * During the above summary creation we prefer to avoid the so called
 * hole-search so we copy over this information for Pixel, SCT, TRT
 * according to the property usePixel, useSCT, useTRT.
 *
 * Truth information is copied from the original
 * xAOD::TrackParticle. The Trk::Tracks (refitted or copied) are saved after
 * slimming with a tool implementing the Trk::ITrackSlimmingTool interface,
 * configurable with the TrackSlimmingTool property (by default
 * TrkTrackSlimmingTool).
 */
class EMBremCollectionBuilder : public AthReentrantAlgorithm
{

public:
  EMBremCollectionBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

  /** Helper struct to store the
   * Trk::Track corresponding to a TrackParticle
   * and the index of the Track Particle in the original
   * container. **/
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
    const xAOD::TrackParticleContainer* inputTrkPartContainer) const;

  StatusCode createNew(
    const EventContext& ctx,
    TrackWithIndex& Info,
    TrackCollection* finalTracks,
    xAOD::TrackParticleContainer* finalTrkPartContainer,
    const xAOD::TrackParticleContainer* inputTrkPartContainer,
    bool isRefitted) const;

  void copyOverInfo(xAOD::TrackParticle& created,
                    const xAOD::TrackParticle& original,
                    bool isRefitted) const;

  /** @brief Option to do truth*/
  Gaudi::Property<bool> m_doTruth{ this, "DoTruth", false, "do truth" };

  /** @brief Option to copy SCT holes estimation*/
  Gaudi::Property<bool> m_doSCT{ this, "useSCT", true, "do SCT" };

  /** @brief Option to copy pixel holes estimation*/
  Gaudi::Property<bool> m_doPix{ this, "usePixel", true, "do Pix" };

  /** @brief Option to copy TRT holes estimation*/
  Gaudi::Property<bool> m_doTRT{ this, "useTRT", true, "do TRT" };

  /** @brief Option to do pixel holes estimation*/
  Gaudi::Property<bool> m_doSlimTrkTracks{ this,
                                           "slimTrkTracks",
                                           true,
                                           "Slim the GSF Trk::Tracks" };

  /** @Cut on minimum silicon hits*/
  Gaudi::Property<int> m_MinNoSiHits{ this,
                                      "minNoSiHits",
                                      4,
                                      "Minimum number of silicon hits on track "
                                      "before it is allowed to be refitted" };

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool> m_trkRefitTool{ this,
                                                     "TrackRefitTool",
                                                     "ElectronRefitterTool",
                                                     "Track refitter tool" };

  /** @brief Tool to create track particle */
  ToolHandle<Trk::ITrackParticleCreatorTool> m_particleCreatorTool{
    this,
    "TrackParticleCreatorTool",
    "TrackParticleCreatorTool",
    "TrackParticle creator tool"
  };

  /** @brief Tool to slim tracks  */
  ToolHandle<Trk::ITrackSlimmingTool> m_slimTool{ this,
                                                  "TrackSlimmingTool",
                                                  "TrkTrackSlimmingTool",
                                                  "Track slimming tool" };

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

  // counters for final statistics
  mutable std::atomic_uint m_FailedFitTracks{ 0 };
  mutable std::atomic_uint m_RefittedTracks{ 0 };
  mutable std::atomic_uint m_FailedSiliconRequirFit{ 0 };
};
#endif //
