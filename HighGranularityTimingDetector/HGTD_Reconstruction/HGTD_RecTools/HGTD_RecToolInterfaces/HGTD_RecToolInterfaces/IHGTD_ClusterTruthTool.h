/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RecToolInterfaces/IHGTD_ClusterTruthTool.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief To assess the performance of track hit matching algorithms, it is
 * necessary to access the truth origin of hits in HGTD that were associated to
 * tracks. It uses both RDO and SDO collections to trace back the truth info.
 */

#ifndef IHGTD_CLUSTERTRUTHTOOL_H
#define IHGTD_CLUSTERTRUTHTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "xAODTruth/TruthParticle.h"

namespace HepMC {
class GenEvent;
}

class InDetSimDataCollection;

class HGTD_Cluster;

namespace HGTD {
enum class ClusterTruthOrigin {
  UNIDENTIFIED = 0,
  TRUTH_PARTICLE = 1, // originates from the tested truth particle
  HARD_SCATTER = 2,   // originates from a HS particle, but unclear which
  PILEUP = 3,         // originates from a pileup interaction
  SECONDARY = 4       // originates from some secondary interaction
};

struct ClusterTruthInfo {
  ClusterTruthOrigin origin;
  /**
   * @brief Shadowing means that a deposit was left by the truth
   * particle, but it was not the first deposit and is thus not used for the
   * time measurement -> I will have an incorrect time
   */
  bool is_shadowed;
  /**
   * @brief A cluster is considered to be merged if more than one particle
   * deposited energy in a given pad.
   */
  bool is_merged;
};
} // namespace HGTD

class IHGTD_ClusterTruthTool : virtual public IAlgTool {

public:
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IHGTD_ClusterTruthTool, 1, 0);

  /**
   * @brief Checks if the cluster originates from the tested truth particle or
   * if it can be assigned to the hard scatter event. If not, the truth
   * information can be used to determine which category (ClusterTruthOrigin)
   * the cluster comes from.
   *
   * @param [in] cluster Hit in HGTD, for which we want the truth information.
   * @param [in] tp Truth particle that is potentially matched to the cluster.
   * @param [in] sim_data SDO collection storing the truth links of the charged
   * diodes.
   * @param [in] hard_scatter_evnt If given, a cluster can be categorised as
   * coming from the HS event, even if the direct match with the truth particle
   * fails.
   *
   * @return Struct combining the relevant truth information.
   */
  virtual HGTD::ClusterTruthInfo
  classifyCluster(const HGTD_Cluster* cluster,
                  const xAOD::TruthParticle* tp,
                  const InDetSimDataCollection* sim_data,
                  const HepMC::GenEvent* hard_scatter_evnt = nullptr) const = 0;
};

#endif // IHGTD_CLUSTERTRUTHTOOL_H
