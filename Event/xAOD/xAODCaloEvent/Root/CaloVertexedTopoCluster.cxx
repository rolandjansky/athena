/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file xAODCaloEvent/src/CaloVertexedTopoCluster.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Evaluate cluster kinematics with a different vertex / signal state.
 */


#include "xAODCaloEvent/CaloVertexedTopoCluster.h"


namespace xAOD {

  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * for the default signal state.
   */
  CaloVertexedTopoCluster::CaloVertexedTopoCluster (const CaloCluster& cl)
    : CaloVertexedClusterBase(cl) 
  {
  }


  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   * @param s The desired signal state.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * for signal state S.
   */
  CaloVertexedTopoCluster::CaloVertexedTopoCluster (const CaloCluster& cl,
                                            CaloCluster::State s)
    : CaloVertexedClusterBase(cl,s)
  {
  }


  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   * @param vx The vertex to use for calculating cluster kinematics.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * relative to vertex @c vx for the default signal state.
   */
  CaloVertexedTopoCluster::CaloVertexedTopoCluster (const CaloCluster& cl,
                                            const Amg::Vector3D&  vx)
    : CaloVertexedClusterBase(cl)
  {
    computeVertexCorr (vx, retrieveRadius());
  }


  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   * @param s The desired signal state.
   * @param vx The vertex to use for calculating cluster kinematics.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * relative to vertex @c vx for the default signal state.
   * for signal state S.
   */
  CaloVertexedTopoCluster::CaloVertexedTopoCluster (const CaloCluster& cl,
                                            CaloCluster::State s,
                                            const Amg::Vector3D&  vx)
    : CaloVertexedClusterBase(cl,s)
  {
    computeVertexCorr (vx, retrieveRadius() );
  }



  /** @brief returns the radius value needed for vertex correction. Uses the CENTER_MAG moment.
   */
  double CaloVertexedTopoCluster::retrieveRadius() {

    double radius = 0;
    double mag = 0;

    if (m_cl->retrieveMoment (CaloCluster::CENTER_MAG, mag)) {  
      radius = mag/cosh(m_eta);
    }
    
    return radius;
  }

} // namespace xAOD
