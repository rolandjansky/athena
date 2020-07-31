// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloClusterCorrection/CaloSwCalibHitsShowerDepth.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2010
 * @brief Calculate the shower depth as used by the calib hits correction.
 */


#ifndef CALOCLUSTERCORRECTION_CALOSWCALIBHITSSHOWERDEPTH_H
#define CALOCLUSTERCORRECTION_CALOSWCALIBHITSSHOWERDEPTH_H


#include "CxxUtils/Array.h"
#include "GaudiKernel/MsgStream.h"
#include "xAODCaloEvent/CaloClusterFwd.h"


namespace CaloClusterCorr {


/**
 * @brief Helper to calculate the shower depth as used in the
 *        calib hits SW cluster correction.
 *
 *        Broken out into a separate class so that it can be used
 *        by other components.
 */
class CaloSwCalibHitsShowerDepth
{
public:
  /**
   * @brief Calculate the depth of the cluster.
   * @param aeta abs(eta) of the cluster.
   * @param start_crack Eta of the start of the crack.
   * @param end_crack Eta of the end of the crack.
   * @param sampling_depth Array of sampling depths per bin/sampling in the EC.
   * @param etamax Maximum eta value in @a sampling_depth.
   * @param cluster Cluster for which to calculate the depth.
   * @param log Stream for debug messages.
   *
   * Note that the sampling depth is used only in the endcap.
   * Parameters for the barrel are hardcoded. (FIXME!)
   * Parameters are passed by reference to allow them to be changed.
   */
  double depth (const float aeta,
                const float start_crack,
                const float end_crack,
                const CaloRec::Array<2>& sampling_depth,
                const float etamax,
                const xAOD::CaloCluster* cluster,
                MsgStream& log) const;


private:
  /**
   * @brief Calculate the sampling depth coefficients for the barrel.
   * @param aeta abs(eta) of the cluster.
   * @param R[out] The set of coefficients per layer.
   */
  void barrelCoefs (const float aeta, float R[4]) const;


  /**
   * @brief Calculate the sampling depth coefficients for the endcap.
   * @param aeta abs(eta) of the cluster.
   * @param sampling_depth Array of sampling depths per bin/sampling in the EC.
   * @param etamax Maximum eta value in @a sampling_depth.
   * @param R[out] The set of coefficients per layer.
   */
  bool endcapCoefs (const float aeta,
                    const CaloRec::Array<2>& sampling_depth,
                    const float etamax,
                    float R[4]) const;
};


} // namespace CaloClusterCorr


#endif // not CALOCLUSTERCORRECTION_CALOCALIBHITSSHOWERDEPTH_H
