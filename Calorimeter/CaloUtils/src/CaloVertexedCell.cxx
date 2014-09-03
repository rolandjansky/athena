/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloUtils/src/CaloVertexedCell.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2012
 * @brief Evaluate cell kinematics with a different vertex.
 */


#include "CaloUtils/CaloVertexedCell.h"
#include "CaloEvent/CaloCell.h"


/**
 * @brief Constructor.
 * @param cell The cell to proxy.
 *
 * In this case, we'll return unchanged the kinematics of the cell.
 */
CaloVertexedCell::CaloVertexedCell (const CaloCell& cell)
  : m_cell (&cell),
    m_eta (cell.eta()),
    m_phi (cell.phi()),
    m_sinth (cell.sinTh()),
    m_costh (cell.cosTh()),
    m_sinph (cell.sinPhi()),
    m_cosph (cell.cosPhi())
{
}


/**
 * @brief Constructor.
 * @param cell The cell to proxy.
 * @param vx The vertex to use for calculating cell kinematics.
 *
 * In this case, we'll return the kinematics of the cell
 * relative to vertex @c vx.
 */
CaloVertexedCell::CaloVertexedCell (const CaloCell& cell,
                                    const Amg::Vector3D&  vx)
  : m_cell (&cell),
    m_eta (cell.eta()),
    m_phi (cell.phi()),
    m_sinth (cell.sinTh()),
    m_costh (cell.cosTh()),
    m_sinph (cell.sinPhi()),
    m_cosph (cell.cosPhi())
{
  computeVertexCorr (vx);
}


/**
 * @brief Calculate cell kinematics for a given vertex.
 * @param vx The vertex to use for calculating cell kinematics.
 */
void
CaloVertexedCell::computeVertexCorr (const Amg::Vector3D& vx)
{
  const CaloDetDescrElement& dd = *m_cell->caloDDE();
  m_eta += ((vx.x()*dd.cosPhi()+vx.y()*dd.sinPhi())*dd.cosTh()-vx.z()*dd.sinTh())/dd.r();
  m_phi += (vx.x()*dd.sinPhi()-vx.y()*dd.cosPhi())/dd.r() ;

  m_sinth += (vx.z()*dd.sinTh() - (vx.x()*dd.cosPhi() + vx.y()*dd.sinPhi())*dd.cosTh()) * dd.cosTh()*dd.sinTh()/dd.r();
  m_costh += - (vx.z()*dd.sinTh() - (vx.x()*dd.cosPhi() + vx.y()*dd.sinPhi())*dd.cosTh()) * dd.sinTh()*dd.sinTh()/dd.r();

  m_sinph += - ( vx.y()*dd.cosPhi() - vx.x()*dd.sinPhi())*dd.cosPhi()/dd.r();
  m_cosph += + ( vx.y()*dd.cosPhi() - vx.x()*dd.sinPhi())*dd.sinPhi()/dd.r();
}
