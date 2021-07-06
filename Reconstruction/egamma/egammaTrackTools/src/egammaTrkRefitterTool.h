/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATRACKTOOLS_EGAMMATRKREFITTERTOOL_H
#define EGAMMATRACKTOOLS_EGAMMATRKREFITTERTOOL_H
/** @brief
  @class egammaTrackRefitterTool
          It receives a egamma object or Trk::Track
          Refits the track associated with an electron
          NOTE  a  new track has been created to ensure
          that no memory leaks occur you must delete the Trk::Track pointed to
  by this function.
  @author A. Morley
  @author C. Anastopouls
*/

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "egammaInterfaces/ICaloCluster_OnTrackBuilder.h"
#include "egammaInterfaces/IegammaTrkRefitterTool.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

#include <memory>

class AtlasDetectorID;
namespace Trk {
class VertexOnTrack;
}

class egammaTrkRefitterTool final
  : virtual public IegammaTrkRefitterTool
  , public AthAlgTool
{

public:
  /** @brief Constructor with AlgTool parameters */
  egammaTrkRefitterTool(const std::string&,
                        const std::string&,
                        const IInterface*);

  /** @brief Destructor */
  ~egammaTrkRefitterTool();

  /** @brief AlgTool initialise method */
  virtual StatusCode initialize() override;

  /** @brief AlgTool finalise method */
  virtual StatusCode finalize() override;

  typedef IegammaTrkRefitterTool::Cache Cache;
  /** @brief Refit the track associated with an egamma object*/
  virtual StatusCode refitElectronTrack(const EventContext& ctx,
                                        const xAOD::Electron*,
                                        Cache& cache) const override final;

  /** @brief Refit a track assoicated to a TrackParticle*/
  virtual StatusCode refitTrackParticle(const EventContext& ctx,
                                        const xAOD::TrackParticle*,
                                        Cache& cache) const override final;

  /** @brief Refit a track*/
  virtual StatusCode refitTrack(const EventContext& ctx,
                                const Trk::Track*,
                                Cache& cache) const override final;

private:
  /** @brief Get the hits from the Inner Detector*/
  std::vector<const Trk::MeasurementBase*> getIDHits(
    const Trk::Track* track) const;

  /** Provide Vertex on track from the beam spot*/
  const Trk::VertexOnTrack* provideVotFromBeamspot(
    const EventContext& ctx,
    const Trk::Track* track) const;

  struct MeasurementsAndTrash
  {
    /*
     * we need to take care of returning all the relevant measurements
     * while at the same time keeping proper ownership only for the ones
     * not handled by the EDM
     */
    std::vector<const Trk::MeasurementBase*> m_measurements;
    std::vector<std::unique_ptr<const Trk::MeasurementBase>> m_trash;
  };
  /** @brief Adds a beam spot to the Measurements passed to the track refitter*/
  MeasurementsAndTrash addPointsToTrack(
    const EventContext& ctx,
    const Trk::Track* track,
    const xAOD::Electron* eg = nullptr) const;

  /** @brief Handle for BeamSpotData*/
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{
    this,
    "BeamSpotKey",
    "BeamSpotData",
    "SG key for beam spot"
  };

  /** @brief The track refitter */
  ToolHandle<Trk::ITrackFitter> m_ITrackFitter{
    this,
    "FitterTool",
    "Trk__GaussianSumFitter/GSFTrackFitter",
    "ToolHandle for track fitter implementation"
  };

  /** @brief track extrapolator */
  ToolHandle<Trk::IExtrapolator> m_extrapolator{
    this,
    "Extrapolator",
    "Trk::Extrapolator/AtlasExtrapolator",
    "Track extrapolator"
  };

  ToolHandle<ICaloCluster_OnTrackBuilder>
    m_CCOTBuilder{ this, "CCOTBuilder", "CaloCluster_OnTrackBuilder", "" };

  /** @brief Run outlier removal when doing the track refit*/
  Gaudi::Property<Trk::RunOutlierRemoval> m_runOutlier{
    this,
    "runOutlier",
    false,
    "Switch to control outlier finding in track fit"
  };

  /** @brief type of material interaction in extrapolation*/
  Gaudi::Property<int> m_matEffects{
    this,
    "matEffects",
    1,
    "Type of material interaction in extrapolation (Default Electron)"
  };

  /** @brief Minimum number of silicon hits on track before it is allowed to be
   * refitted*/
  Gaudi::Property<int> m_MinNoSiHits{ this,
                                      "minNoSiHits",
                                      3,
                                      "Minimum number of silicon hits on track "
                                      "before it is allowed to be refitted" };

  /** @brief Particle Hypothesis*/
  Trk::ParticleHypothesis m_ParticleHypothesis;

  const AtlasDetectorID* m_idHelper;

  /** @brief Add outlier to track hits into vector of hits*/
  Gaudi::Property<bool> m_reintegrateOutliers{
    this,
    "ReintegrateOutliers",
    false,
    "Switch to control addition of  outliers back for track fit"
  };

  /** @brief Option to use very simplistic beam spot constraint*/
  Gaudi::Property<bool> m_useBeamSpot{
    this,
    "useBeamSpot",
    false,
    "Switch to control use of Beam Spot Measurement"
  };

  Gaudi::Property<bool> m_useClusterPosition{
    this,
    "useClusterPosition",
    false,
    "Switch to control use of Cluster position measurement"
  };

  /** @brief Option to remove TRT hits from track*/
  Gaudi::Property<bool> m_RemoveTRT{ this,
                                     "RemoveTRTHits",
                                     false,
                                     "RemoveTRT Hits" };
};

#endif
