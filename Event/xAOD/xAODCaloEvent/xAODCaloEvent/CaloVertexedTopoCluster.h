// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file xAODCaloEvent/CaloVertexedTopoCluster.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Evaluate cluster kinematics with a different vertex / signal state.
 */


#ifndef XAODCALOEVENT_CALOVERTEXEDTOPOCLUSTER_H
#define XAODCALOEVENT_CALOVERTEXEDTOPOCLUSTER_H

#include "xAODCaloEvent/CaloVertexedClusterBase.h"


namespace xAOD {


/**
 * @brief Evaluate cluster kinematics with a different vertex / signal state.
 *
 * This class acts as a proxy for a cluster object.  It provides
 * the same @c IParticle interface, except that the cluster kinematics
 * are recalculated for a different vertex, and optionally a different
 * signal state.
 * When kinematics are recalculated w.r.t a vertex, the CENTER_MAG moment is used.
 * This class is available in Athena and RootCore.
 * See CaloVertexedCluster for a calculation based on the EM layer.
 */
class CaloVertexedTopoCluster
  : public CaloVertexedClusterBase
{
public:

  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * for the default signal state.
   */
  CaloVertexedTopoCluster (const CaloCluster& cl);

  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   * @param s The desired signal state.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * for signal state S.
   */
  CaloVertexedTopoCluster (const CaloCluster& cl,
                       CaloCluster::State s);

  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   * @param vx The vertex to use for calculating cluster kinematics.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * relative to vertex @c vx for the default signal state.
   */  
  CaloVertexedTopoCluster (const CaloCluster& cl,
                       const Amg::Vector3D& vx);
  
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
  CaloVertexedTopoCluster (const CaloCluster& cl,
                       CaloCluster::State s,
                       const Amg::Vector3D& vx);


protected:  

  /** @brief returns the radius value needed for vertex correction. Uses the CENTER_MAG moment.
   */
  double retrieveRadius() ;

};


} // namespace xAOD


#endif // not XAODCALOEVENT_CALOVERTEXEDTOPOCLUSTER_H
