/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_TrackTimeExtensionTools/HGTD_IterativeExtensionTool.h
 *
 * @author Noemi Calace <noemi.calace@cern.ch>
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 *
 * @date August, 2021
 *
 * @brief The HGTD_IterativeExtensionTool uses an iterative Kalman filter
 * approach to find hits (HGTD_Cluster) in HGTD that are compatible with a track
 * extrapolated to the surfaces of HGTD. During this process, the track itself
 * is not altered, the found hits are returned as an array of
 * HGTD_ClusterOnTrack objects.
 *
 * TODO:
 *  - exchange chi2/ndof with TMath::Prob?
 *  - add chi2 for the time -> get an estimate at layer 0 (TOF +- beamspot),
 *    decorate the TrackParameters with time and timeres before!
 */

#ifndef HGTD_ITERATIVEEXTENTSIONTOOL_H
#define HGTD_ITERATIVEEXTENTSIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "HGTD_RecToolInterfaces/IHGTD_TrackTimeExtensionTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "HGTD_RecToolInterfaces/IHGTD_TOFcorrectionTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IUpdator.h"

#include <memory>

class HGTD_ID;
class HGTD_DetectorManager;

class HGTD_IterativeExtensionTool : public extends<AthAlgTool, IHGTD_TrackTimeExtensionTool> {

public:
  HGTD_IterativeExtensionTool(const std::string&, const std::string&,
                              const IInterface*);

  virtual StatusCode initialize() override final;

  /**
   * @brief Finds the (up to) four measurements in HGTD that can be associated
   * to the track. The Track itself is not altered during this process.
   * Since HGTD contains four active layers per endcap, four hits is the
   * maximum. An extension can be of hole-type
   *
   * @param [in] track Track reconstructed in ITk.
   *
   * @return Array of compatible HGTD hits in the form of HGTD_ClusterOnTrack.
   */
  virtual std::array<std::unique_ptr<const Trk::TrackStateOnSurface>, 4>
  extendTrackToHGTD(const EventContext& ctx,
                    const Trk::Track& track,
                    const HGTD_ClusterContainer* container) override final;

private:
  /**
   * @brief  Retrieve the last hit on track stored in the Trk::Track.
   *
   * @param [in] track Track reconstructed in ITk.
   *
   * @return The last hit on track, i.e. the closest one to HGTD.
   *
   * FIXME: returns a const*, but the Track manages this TSOS. Could I return
   * rather const&? But: I want to check if type is OK... should I return an
   * "empty" TSOS if nothing passes the requirements?
   */
  const Trk::TrackStateOnSurface* getLastHitOnTrack(const Trk::Track& track) const;

  /**
   * @brief  Select all within the vincinity of the point of extrapolation.
   *
   * @param [in] param The already extrapolated track parameter.
   * @param [in] layer Target layer of the extrapolation.
   *
   * @return Vector of compatible surfaces that represent modules.
   *
   * FIXME exchange the loop with the neighbour functionality (wherever that is)
   */
  std::vector<const Trk::Surface*>
  getCompatibleSurfaces(const Trk::TrackParameters& param,
                        const Trk::Layer* layer) const;

  /**
   * @brief After the compatible surfaces have been selected, the extrapolation
   * is repeated to each of them.
   *
   * @param [in] param The last track parameter before extrapolation.
   * @param [in] surfaces Target layer of the extrapolation.
   *
   * @return Vector of the updated track parameters.
   *
   * FIXME Is this really necessary to call the extrapolation on each target
   * surface?
   */
  std::vector<std::unique_ptr<const Trk::TrackParameters>>
  extrapolateToSurfaces(const EventContext& ctx,
                        const Trk::TrackParameters& param,
                        const std::vector<const Trk::Surface*>& surfaces) const;

  /**
   * @brief Finds the overall best fitting cluster by keeping the one that gave
   * the lowest chi2 after testing each surface and returning it.
   */
  std::unique_ptr<const Trk::TrackStateOnSurface>
  updateStateWithBestFittingCluster(
      const std::vector<std::unique_ptr<const Trk::TrackParameters>>& params) const;

  /**
   * @brief Find the cluster on a given surface that has the best chi2 passing
   * the cut. Is called within updateStateWithBestFittingCluster.
   */
  std::unique_ptr<const Trk::TrackStateOnSurface>
  findBestCompatibleCluster(const Trk::TrackParameters* param) const;

  /**
   * @brief Calls the TOF correction tool to build an HGTD_ClusterOnTrack with a
   * calibrated time and resolution and then calls the Kalman updator tools to
   * update the track state.
   */
  std::unique_ptr<const Trk::TrackStateOnSurface>
  updateState(const Trk::TrackParameters* param,
              const HGTD_Cluster* cluster) const;

  // extrapolation tool
  ToolHandle<Trk::IExtrapolator> m_extrapolator{this, "ExtrapolatorTool", "Trk::Extrapolator/AtlasExtrapolator", "Tool for extrapolating the track to the HGTD surfaces"};
  // kalman updator tool
  ToolHandle<Trk::IUpdator> m_updator{this, "UpdatorTool", "Trk::KalmanUpdator/KalmanUpdator", "Tool for updating the track parameters accounting for new measurements"};

  ToolHandle<IHGTD_TOFcorrectionTool> m_tof_corr_tool{this, "TOFCorrTool", "StraightLineTOFcorrectionTool", "Tool for correcting for time of flight"};

  // keep a pointer to the currently used track, but does not own it!
  // FIXME: this is needed for the TOF correction. Maybe there is a smarter way
  // without keeping this pointer and without pushing the Trk::Track object
  // through everything ==>> move to a TOF corr. accepting just the perigee??
  const Trk::Track* m_track{nullptr};

  /**
   * @brief Keep pointer to current cluster container.
   */
  const HGTD_ClusterContainer* m_cluster_container{nullptr};

  const HGTD_DetectorManager* m_hgtd_det_mgr{nullptr};
  const HGTD_ID* m_hgtd_id_helper{nullptr};

  /**
   * @brief Track extensions are only kept if the chi2/ndof is lower than the
   * defined cut.
   */
  FloatProperty m_chi2_cut{this, "Chi2Cut", 5.0, "Quality cut for decision to keep track extension"};

  /**
   * @brief Particle hypothesis used for the extrapolation.
   */
  IntegerProperty m_particle_hypot{this, "ParticleHypothesis", Trk::ParticleHypothesis::pion, "The hypothesis of the track's particle type"};
};

#endif // HGTD_ITERATIVEEXTENTSIONTOOL_H
