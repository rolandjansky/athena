// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloUtils/CaloVertexedCluster.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Evaluate cluster kinematics with a different vertex / signal state.
 */


#ifndef CALOUTILS_CALOVERTEXEDCLUSTER_H
#define CALOUTILS_CALOVERTEXEDCLUSTER_H


#include "xAODCaloEvent/CaloCluster.h"
#include "xAODBase/IParticle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CxxUtils/final.h"


namespace xAOD {


/**
 * @brief Evaluate cluster kinematics with a different vertex / signal state.
 *
 * This class acts as a proxy for a cluster object.  It provides
 * the same @c IParticle interface, except that the cluster kinematics
 * are recalculated for a different vertex, and optionally a different
 * signal state.
 */
class CaloVertexedCluster
  : public IParticle
{
public:
  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * for the default signal state.
   */
  CaloVertexedCluster (const CaloCluster& cl);


  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   * @param s The desired signal state.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * for signal state S.
   */
  CaloVertexedCluster (const CaloCluster& cl,
                       CaloCluster::State s);


  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   * @param vx The vertex to use for calculating cluster kinematics.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * relative to vertex @c vx for the default signal state.
   */
  CaloVertexedCluster (const CaloCluster& cl,
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
  CaloVertexedCluster (const CaloCluster& cl,
                       CaloCluster::State s,
                       const Amg::Vector3D& vx);


  /**
   * @brief Return the cluster being proxied,.
   */
  const CaloCluster& clust() const { return *m_cl; }

  /// @name Functions describing the 4-momentum of the object
  /// @{

  /// The transverse momentum (\f$p_T\f$) of the particle
  virtual double           pt() const ATH_FINAL { return m_p4.Pt(); }
  /// The pseudorapidity (\f$\eta\f$) of the particle
  virtual double           eta() const ATH_FINAL { return m_eta; }
  /// The azimuthal angle (\f$\phi\f$) of the particle
  virtual double           phi() const ATH_FINAL { return m_phi; }
  /// The invariant mass of the particle
  virtual double           m() const ATH_FINAL { return m_p4.M(); }
  /// The total energy of the particle
  virtual double           e() const ATH_FINAL { return m_p4.E(); }
  /// The true rapidity (y) of the particle
  virtual double           rapidity() const ATH_FINAL { return m_p4.Rapidity(); }

  /// The full 4-momentum of the particle
  virtual const FourMom_t& p4() const ATH_FINAL { return m_p4; }

  /// @}

  /// The type of the object as a simple enumeration
  virtual Type::ObjectType type() const ATH_FINAL { return m_cl->type(); }



private:
  /**
   * @brief Calculate cluster kinematics for a given vertex.
   * @param vx The vertex to use for calculating cluster kinematics.
   */
  void computeVertexCorr (const Amg::Vector3D& vx);


  /// The cluster being proxied.
  const CaloCluster* m_cl;


  /// The recalculated cluster eta.
  double m_eta;


  /// The recalculated cluster phi.
  double m_phi;


  /// The recalculated cluster 4-momentum.
  FourMom_t m_p4;
};


} // namespace xAOD


#endif // not CALOUTILS_CALOVERTEXEDCLUSTER_H
