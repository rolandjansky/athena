/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file xAODCaloEvent/src/CaloVertexedClusterBase.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Evaluate cluster kinematics with a different vertex / signal state.
 */


#include "xAODCaloEvent/CaloVertexedClusterBase.h"
#include "CxxUtils/sincos.h"
#include <cmath>

using std::cosh;
using std::tanh;


namespace xAOD {


/**
 * @brief Constructor.
 * @param cl The cluster to proxy.
 *
 * In this case, we'll return unchanged the kinematics of the cluster
 * for the default signal state.
 */
CaloVertexedClusterBase::CaloVertexedClusterBase (const CaloCluster& cl)
  : m_cl (&cl),
    m_eta (cl.eta()),
    m_phi (cl.phi()),
    m_p4 (cl.p4())
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
CaloVertexedClusterBase::CaloVertexedClusterBase (const CaloCluster& cl,
                                          CaloCluster::State s)
  : m_cl (&cl),
    m_eta (cl.eta(s)),
    m_phi (cl.phi(s)),
    m_p4 (cl.p4(s))
{
}




/**
 * @brief Calculate cluster kinematics for a given vertex.
 * @param vx The vertex to use for calculating cluster kinematics.
 */
void
CaloVertexedClusterBase::computeVertexCorr (const Amg::Vector3D& vx, double radius)
{

  if (radius<1.) return;

  double eta = m_eta;
  double phi = m_phi;

  if (std::fabs(eta)>10. || std::fabs(phi)>10.) return;

  

  CxxUtils::sincos sc (phi);
  double iradius = 1 / radius;
  m_eta += (-vx[2]/cosh(m_eta) + sc.apply (vx[1], vx[0])*tanh(m_eta)) * iradius;
  m_phi += sc.apply (vx[0], -vx[1]) * iradius;

  double pt = m_p4.P() / cosh (m_eta);
  m_p4.SetPtEtaPhiE (pt, m_eta, m_phi, m_p4.E());
}


} // namespace xAOD
