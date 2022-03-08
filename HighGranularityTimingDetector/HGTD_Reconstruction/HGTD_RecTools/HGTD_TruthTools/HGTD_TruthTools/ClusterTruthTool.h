/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_TruthTools/ClusterTruthTool.h
 *
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @author Noemi Calace <noemi.calace@cern.ch>
 *
 * @date August, 2021
 *
 * @brief Uses the same categorisation as for the HGTD TDR results, taken from:
 * https://gitlab.cern.ch/aleopold/memleakhunt/-/blob/master/TrkExtrapolation/TrkExTools/src/TrackTimingExtensionAlg.cxx#L901
 */

#ifndef HGTD_TRUTHTOOLS_CLUSTERTRUTHTOOL_H
#define HGTD_TRUTHTOOLS_CLUSTERTRUTHTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "HGTD_RecToolInterfaces/IHGTD_ClusterTruthTool.h"

class HGTD_ID;

namespace HGTD {

class ClusterTruthTool : public extends<AthAlgTool, IHGTD_ClusterTruthTool> {

public:
  ClusterTruthTool(const std::string&, const std::string&, const IInterface*);

  /**
   * @brief The InDetSimDataCollection is a map, connecting Identifiers from the
   * RDOs to InDetSimData objects, that keep a vector of HepMcParticleLink,
   * float pairs, where the float keeps the energy of the given energy deposit.
   * For HGTD, we store the time of the deposit instead, and can then select the
   * first deposit, since only this one would contribute to the time read out by
   * the ASIC.
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
  virtual HGTD::ClusterTruthInfo classifyCluster(
      const HGTD_Cluster* cluster, const xAOD::TruthParticle* tp,
      const InDetSimDataCollection* sim_data,
      const HepMC::GenEvent* hard_scatter_evnt = nullptr) const override final;
};

} // namespace HGTD

#endif // HGTD_TRUTHTOOLS_CLUSTERTRUTHTOOL_H
