// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloUtils/CaloVertexedCell.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2012
 * @brief Evaluate cell kinematics with a different vertex.
 */


#ifndef CALOUTILS_CALOVERTEXEDCELL_H
#define CALOUTILS_CALOVERTEXEDCELL_H


#include "CaloEvent/CaloCell.h"
#include "FourMom/P4EEtaPhiMBase.h"
#include "CxxUtils/fpcompare.h"
#include "CxxUtils/final.h"


class CaloCell;


/**
 * @brief Evaluate cell kinematics with a different vertex.
 *
 * This class acts as a proxy for a cell object.  It the same @c I4Momentum
 * interface, except that the cell kinematics are recalculated for
 * a different vertex.
 */
class CaloVertexedCell
  : public P4EEtaPhiMBase
{
public:
  /**
   * @brief Constructor.
   * @param cell The cell to proxy.
   *
   * In this case, we'll return unchanged the kinematics of the cell.
   */
  CaloVertexedCell (const CaloCell& cell);


  /**
   * @brief Constructor.
   * @param cell The cell to proxy.
   * @param vx The vertex to use for calculating cell kinematics.
   *
   * In this case, we'll return the kinematics of the cell
   * relative to vertex @c vx.
   */
  CaloVertexedCell (const CaloCell& cell,
                    const Amg::Vector3D& vx);

  /**
   * @brief Return the cell being proxied,.
   */
  const CaloCell& cell() const { return *m_cell; }

  /// The pseudorapidity of the particle.
  virtual double eta() const ATH_FINAL { return m_eta; }

  /// The aximuthal angle of the particle.
  virtual double phi() const ATH_FINAL { return m_phi; }

  /// The energy of the particle.
  virtual double e() const ATH_FINAL { return m_cell->e(); }

  /// The mass of the particle.
  virtual double m() const ATH_FINAL { return m_cell->m(); }

  /// The sine of the polar angle of the particle.
  virtual double sinTh() const ATH_FINAL { return m_sinth; }

  /// The cosine of the polar angle of the particle.
  virtual double cosTh() const ATH_FINAL { return m_costh; }

  /// The sine of the azimuthal angle of the particle.
  virtual double sinPhi() const ATH_FINAL { return m_sinph; }

  /// The cosine of the azimuthal angle of the particle.
  virtual double cosPhi() const ATH_FINAL { return m_cosph; }

  /// The energy of the particle.
  double energy() const { return m_cell->energy(); }


  /**
   * @brief Helper to compare two of these objects by energy.
   */
  struct compE
  {
    bool operator() (const CaloVertexedCell& a,
                     const CaloVertexedCell& b) const
    {
      return CxxUtils::fpcompare::greater (a.e(), b.e());
    }
  };


private:
  /**
   * @brief Calculate cell kinematics for a given vertex.
   * @param vx The vertex to use for calculating cell kinematics.
   */
  void computeVertexCorr (const Amg::Vector3D& vx);

  /// The cell being proxied.
  const CaloCell* m_cell;

  /// The recalculated cell eta.
  double m_eta;

  /// The recalculated cell phi.
  double m_phi;

  /// The recalculated cell sin(theta).
  double m_sinth;

  /// The recalculated cell cos(theta).
  double m_costh;

  /// The recalculated cell sin(phi).
  double m_sinph;

  /// The recalculated cell cos(phi).
  double m_cosph;
};



#endif // not CALOUTILS_CALOVERTEXEDCELL_H
