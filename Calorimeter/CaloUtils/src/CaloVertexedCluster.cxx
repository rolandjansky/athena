/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloUtils/src/CaloVertexedCluster.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Evaluate cluster kinematics with a different vertex / signal state.
 */


#include "CaloUtils/CaloVertexedCluster.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
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
CaloVertexedCluster::CaloVertexedCluster (const CaloCluster& cl)
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
CaloVertexedCluster::CaloVertexedCluster (const CaloCluster& cl,
                                          CaloCluster::State s)
  : m_cl (&cl),
    m_eta (cl.eta(s)),
    m_phi (cl.phi(s)),
    m_p4 (cl.p4(s))
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
CaloVertexedCluster::CaloVertexedCluster (const CaloCluster& cl,
                                          const Amg::Vector3D&  vx)
  : m_cl (&cl),
    m_eta (cl.eta()),
    m_phi (cl.phi()),
    m_p4 (cl.p4())
{
  computeVertexCorr (vx);
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
CaloVertexedCluster::CaloVertexedCluster (const CaloCluster& cl,
                                          CaloCluster::State s,
                                          const Amg::Vector3D&  vx)
  : m_cl (&cl),
    m_eta (cl.eta(s)),
    m_phi (cl.phi(s)),
    m_p4 (cl.p4(s))
{
  computeVertexCorr (vx);
}


/**
 * @brief Calculate cluster kinematics for a given vertex.
 * @param vx The vertex to use for calculating cluster kinematics.
 */
void
CaloVertexedCluster::computeVertexCorr (const Amg::Vector3D& vx)
{
  double radius=0.; 
  double eta = m_eta;
  double phi = m_phi;

  if (std::fabs(eta)>10. || std::fabs(phi)>10.) return;

  //Try use cluster moment 'CENTER_MAG'
  double mag = 0;
  if (m_cl->retrieveMoment (CaloCluster::CENTER_MAG, mag)) {  
    radius = mag/cosh(eta);
  }
  else {// No CENTER_MAG moment (normal for EM clusters): use entrance face of EM layer 2 

    const CaloDetDescrManager* calodet=CaloDetDescrManager::instance();
    CaloCell_ID::CaloSample layer;
    if (m_cl->inBarrel() && ! m_cl->inEndcap()) {
      layer = CaloCell_ID::EMB2;
    }
    else if (m_cl->inEndcap() && !m_cl->inBarrel()) {
      layer = CaloCell_ID::EME2;
    }
    else {
      double eBarrel = m_cl->eSample(CaloSampling::EMB2);
      double eEndCap = m_cl->eSample(CaloSampling::EME2);
      if (eEndCap>= eBarrel) layer = CaloCell_ID::EME2;
      else                   layer = CaloCell_ID::EMB2;
    }

    const CaloDetDescrElement* element = calodet->get_element(layer,eta,phi);
    if (element) {
      if (layer==CaloCell_ID::EMB2)
        radius = element->r()-element->dr();
      else if (std::fabs(eta)>0.)
        radius = (std::fabs(element->z())-std::fabs(element->dz()))/std::fabs(sinh(eta));
    }
  } //end else  No CENTER_MAG moment

  if (radius<1.) return;

  CxxUtils::sincos sc (phi);
  double iradius = 1 / radius;
  m_eta += (-vx[2]/cosh(m_eta) + sc.apply (vx[1], vx[0])*tanh(m_eta)) * iradius;
  m_phi += sc.apply (vx[0], -vx[1]) * iradius;

  double pt = m_p4.P() / cosh (m_eta);
  m_p4.SetPtEtaPhiE (pt, m_eta, m_phi, m_p4.E());
}


} // namespace xAOD
