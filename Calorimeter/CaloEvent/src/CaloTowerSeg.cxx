/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloEvent/CaloTowerSeg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2013
 * @brief Out-of-line implementations for CaloTowerSeg.
 */


#include "CaloEvent/CaloTowerSeg.h"

const CaloPhiRange CaloTowerSeg::s_range;


/**
 * @brief Return a window within the current segmentation.
 *        The window is centered at eta/phi with half-widths deta/dphi.
 */
CaloTowerSeg::SubSeg CaloTowerSeg::subseg (double eta, double deta,
                                           double phi, double dphi) const
{
  using index_t = SubSeg::index_t;

  index_t etamin = this->etaIndex (eta - deta + 0.001);
  if (etamin == CaloTowerSeg::outOfRange)
    etamin = 1;

  index_t etamax = this->etaIndex (eta + deta - 0.001);
  if (etamax == CaloTowerSeg::outOfRange)
    etamax = this->neta();

  index_t phimin = this->phiIndex (CaloPhiRange::fix (phi - dphi + 0.001));
  if (phimin == CaloTowerSeg::outOfRange)
    phimin = this->phiIndex (this->phimin());

  index_t phimax = this->phiIndex (CaloPhiRange::fix (phi + dphi - 0.001));
  if (phimax == CaloTowerSeg::outOfRange)
    phimax = this->phiIndex (this->phimax() - this->dphi()/2);

  return SubSeg (*this, etamin, etamax, phimin, phimax);
}


/**
 * @brief Constructor.
 * @param parent Segmentation of which this is a part.
 * @param etamin Lower eta index.
 * @param etamax Upper eta index (inclusive).
 * @param phimin Lower phi index.
 * @param phimax Upper phi index (inclusive).
 *               If phimax < phimin, then we wrap around in phi.
 */
CaloTowerSeg::SubSeg::SubSeg (const CaloTowerSeg& parent,
                              index_t etamin, index_t etamax,
                              index_t phimin, index_t phimax)
   : m_parent (parent),
    m_etamin (etamin),
    m_etamax (etamax),
    m_phimin (phimin),
    m_phimax (phimax)
{
}


/**
 * @brief The number of towers in the phi direction in this window.
 */
size_t CaloTowerSeg::SubSeg::nphi() const
{
  int nphi = m_phimax - m_phimin + 1;
  // Handle phi wraparound.
  if (m_phimax < m_phimin)
    nphi += m_parent.nphi();
  return nphi;
}


/**
 * @brief Return a new segmentation object corresponding to this window.
 */
CaloTowerSeg CaloTowerSeg::SubSeg::segmentation() const
{
  const double etamin = m_parent.eta (m_etamin) - m_parent.deta()/2;
  const double etamax = m_parent.eta (m_etamax) + m_parent.deta()/2;

  const double phimin = m_parent.phi (m_phimin) - m_parent.dphi()/2;
  const double phimax = m_parent.phi (m_phimax) + m_parent.dphi()/2;

  return CaloTowerSeg (neta(), nphi(), etamin, etamax, phimin, phimax);
}


