/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMTRACKMATCHBUILDER_H
#define EGAMMATOOLS_EMTRACKMATCHBUILDER_H

/**
  @class EMTrackMatchBuilder
  EMTrackMatch data object builder.
  @author H. Ma
  @author RD schaffer
  @author Thomas Koffas
  @author Christos Anastopoulos
  The matching of a track to a cluster is driven by the EMTrackMatchBuilder tool
  located in the Reconstruction/egamma/egammaTools package.
  */

// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaRecEvent/egammaRecContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

namespace Reco {
class ITrackToVertex;
}

class EMTrackMatchBuilder
  : public AthAlgTool
  , virtual public IEMTrackMatchBuilder
{

public:
  /** @brief Default constructor*/
  EMTrackMatchBuilder(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  /** @brief Destructor*/
  virtual ~EMTrackMatchBuilder() = default;

  /** @brief Gaudi algorithm hooks*/
  StatusCode initialize() override final;
  /** @brief execute method*/
  virtual StatusCode executeRec(
    const EventContext& ctx,
    EgammaRecContainer* egammas) const override final;

  /** @brief execute method*/
  virtual StatusCode trackExecute(
    const EventContext& ctx,
    egammaRec* eg,
    const xAOD::TrackParticleContainer* trackPC) const override final;

private:
  /** @brief A structure for keeping track match information */
  struct TrackMatch
  {
  public:
    bool hasPix;
    int trackNumber;
    int hitsScore;
    double dR;
    double seconddR;
    double deltaPhiLast;
    std::array<double, 4> deltaEta;
    std::array<double, 4> deltaPhi;
    std::array<double, 4> deltaPhiRescaled;
  };

  /** @brief function object to sort track matches based on quality */
  class TrackMatchSorter
  {
  public:
    TrackMatchSorter(double distance = 0.0)
      : m_distance(distance){};
    bool operator()(const TrackMatch& match1, const TrackMatch& match2);

  private:
    double m_distance;
  };

  /** @brief Compute for tracks passing the loose matching
    the distance between track extrapolated to 2nd sampling and cluster */
  bool inBroadWindow(const EventContext& ctx,
                     std::vector<TrackMatch>& trackMatches,
                     const xAOD::CaloCluster& cluster,
                     int trackNumber,
                     const xAOD::TrackParticle& trkPB,
                     const Trk::PropDirection dir) const;

  /** @brief Loose track-cluster matching */
  bool isCandidateMatch(const xAOD::CaloCluster* cluster,
                        const xAOD::TrackParticle* track,
                        bool flip) const;

  // configuration:
  /** @brief name of TrackParticle container in TDS*/
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackParticlesKey{
    this,
    "TrackParticlesName",
    "",
    "Name of the input track particle container"
  };

  /** @brief broad cut on deltaEta*/
  Gaudi::Property<double> m_broadDeltaEta{ this,
                                           "broadDeltaEta",
                                           0.1,
                                           "Value of broad cut for delta eta" };

  /** @brief broad cut on deltaPhi*/
  Gaudi::Property<double> m_broadDeltaPhi{ this,
                                           "broadDeltaPhi",
                                           0.1,
                                           "Value of broad cut for delta phi" };

  /** @brief narrow cut on deltaEta*/
  Gaudi::Property<double> m_narrowDeltaEta{
    this,
    "narrowDeltaEta",
    0.05,
    "Value of narrow cut for delta eta"
  };

  /** @brief narrow cut on deltaPhiRescale*/
  Gaudi::Property<double> m_narrowDeltaPhi{
    this,
    "narrowDeltaPhi",
    0.05,
    "Value of the narrowd cut for delta phi"
  };

  /** @brief narrow cut on deltaPhi for electrons*/
  Gaudi::Property<double> m_narrowDeltaPhiBrem{
    this,
    "narrowDeltaPhiBrem",
    0.1,
    "Value of the narrow cut for delta phi  Brem"
  };

  /** @brief narrow cut on deltaPhiRescale*/
  Gaudi::Property<double> m_narrowDeltaPhiRescale{
    this,
    "narrowDeltaPhiRescale",
    0.05,
    "Value of the narrow cut for delta phi Rescale"
  };

  /** @brief narrow cut on deltaPhiRescale for electrons*/
  Gaudi::Property<double> m_narrowDeltaPhiRescaleBrem{
    this,
    "narrowDeltaPhiRescaleBrem",
    0.1,
    "Value of the narrow cut for delta phi Rescale Brem"
  };

  /** @Maximum deltaPhi (Res) allowed for a match */
  Gaudi::Property<double> m_MaxDeltaPhiRescale{
    this,
    "MaxDeltaPhiRescale",
    0.25,
    "Maximum Value of the deltaPhi rescale"
  };

  /** @brief flag to turn on/off use of isCandidateMatch*/
  Gaudi::Property<bool> m_useCandidateMatch{
    this,
    "useCandidateMatch",
    true,
    "Boolean to use candidate matching"
  };

  /** @brief Boolean to apply heuristic when tracks have close deltaR */
  Gaudi::Property<bool> m_useScoring{
    this,
    "useScoring",
    true,
    "Boolean to apply heuristic when tracks have close deltaR"
  };

  /** @brief Boolean to use Rescale in the metric*/
  Gaudi::Property<bool> m_useRescaleMetric{ this,
                                            "UseRescaleMetric",
                                            true,
                                            "Use Rescale Metric" };

  /** @brief Boolean to do second pass with Rescale*/
  Gaudi::Property<bool> m_SecondPassRescale{ this,
                                             "SecondPassRescale",
                                             true,
                                             "Do second pass with rescale" };

  /// The resolutions:  might be good to split in barrel/end-cap in the future
  Gaudi::Property<float> m_deltaEtaResolution{ this,
                                               "DeltaEtaResolution",
                                               1.0,
                                               "The deltaEta resolution" };

  Gaudi::Property<float> m_deltaPhiResolution{ this,
                                               "DeltaPhiResolution",
                                               1.0,
                                               "The deltaPhi resolution" };

  Gaudi::Property<float> m_deltaPhiRescaleResolution{
    this,
    "DeltaPhiRescaleResolution",
    1.0,
    "The deltaPhiRescale resolution"
  };

  /** @brief The distance from which one goes from using better deltaR to using
     score. Note that this distance varies depending on the resolutions entered
     above. If you don't use resolutions (resolution = 1.0) this becomes deltaR
     distance.
  */
  Gaudi::Property<float> m_distanceForScore{
    this,
    "DistanceForScore",
    0.01,
    "The distance from which one goes from using better deltaR to using score."
  };

  ToolHandle<IEMExtrapolationTools> m_extrapolationTool{
    this,
    "ExtrapolationTool",
    "EMExtrapolationTools",
    "Name of the extrapolation tool"
  };

  /** @brief */
  Gaudi::Property<bool> m_isCosmics{ this,
                                     "isCosmics",
                                     false,
                                     "Boolean for use of cosmics" };

  // Calculated values based on resolutions
  double m_deltaEtaWeight;
  double m_deltaPhiWeight;
  double m_deltaPhiRescaleWeight;

  TrackMatchSorter m_sorter;
};

#endif

