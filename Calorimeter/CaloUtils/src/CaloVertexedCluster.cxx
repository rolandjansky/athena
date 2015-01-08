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

#include "CaloDetDescr/CaloDetDescrManager.h"

#include "CaloUtils/CaloVertexedCluster.h"


namespace xAOD {


  /**
   * @brief Constructor.
   * @param cl The cluster to proxy.
   *
   * In this case, we'll return unchanged the kinematics of the cluster
   * for the default signal state.
   */
  CaloVertexedCluster::CaloVertexedCluster (const CaloCluster& cl)
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
  CaloVertexedCluster::CaloVertexedCluster (const CaloCluster& cl,
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
  CaloVertexedCluster::CaloVertexedCluster (const CaloCluster& cl,
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
  CaloVertexedCluster::CaloVertexedCluster (const CaloCluster& cl,
                                            CaloCluster::State s,
                                            const Amg::Vector3D&  vx)
    : CaloVertexedClusterBase(cl,s)
  {
    computeVertexCorr (vx, retrieveRadius() );
  }





  double CaloVertexedCluster::retrieveRadius() {
    
    // use entrance face of EM layer 2 
    double eta = m_eta;
    double radius =0;
    
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
    
    const CaloDetDescrElement* element = calodet->get_element(layer,eta,m_phi);
    if (element) {
      if (layer==CaloCell_ID::EMB2)
        radius = element->r()-element->dr();
      else if (std::fabs(eta)>0.)
        radius = (std::fabs(element->z())-std::fabs(element->dz()))/std::fabs(sinh(eta));
    }
    
    return radius;
  }

} // namespace xAOD
