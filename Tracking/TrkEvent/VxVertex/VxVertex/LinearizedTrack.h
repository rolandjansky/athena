/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
//  LinearizedTrack (c) ATLAS Inner Detector Software  2006
////////////////////////////////////////////////////////////////////////////
/**
 * @class Trk::LinearizedTrack.h
 *
 * Caching class to store the parameters of expansion of the
 * measurement equation in the vicinity of the vertex.
 *
 * See [1] Billoir and Qian et  al. NIM A311 (1992) 139-150 and
 * [2] R, Fruehwirth et al. Comp. Phys. Comm 96(1996) 189-208 for reference.
 *
 * The actiual way of expansion depends on the factory used. We assume
 * that by default a FullPerigeeLinearized track factory, implementing
 * an exact solution is used.
 *
 * This is a replacement of a former PerigeeLinearizedTrack class
 *
 * @authors
 *                 Kirill Prokofiev <Kirill.Prokofiev@cern.ch>,
 *                 Andreas Wildauer <Andreas.Wildauer@cern.ch>,
 *                 Giacinto Piacquadio <nicola.giacinto.piacquadio@cern.ch>
 *
 * @date           20-11-2006
 *
 */

#ifndef VXVERTEX_LINEARIZEDTRACK_H
#define VXVERTEX_LINEARIZEDTRACK_H

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

class LinearizedTrack
{

public:

  /**
   * Constructor taking the perigee parameters  and covariance matrix
   * of the track, propagated to the point of closest approach of
   * linearization point, position and momentum Jacobians, constant
   * term of expansion, chi2, and weight of the current track.
   */
  LinearizedTrack(const AmgVector(5) & expectedParametersAtPCA,
                  const AmgSymMatrix(5) & expectedParErrorAtPCA,
                  const Amg::Vector3D& linPoint,
                  const AmgMatrix(5, 3) & positionJacobian,
                  const AmgMatrix(5, 3) & momentumJacobian,
                  const Amg::Vector3D& expectedPosition,
                  const Amg::Vector3D& expectedMomentum,
                  const AmgVector(5) & constantTerm);

  
  ///default operations
  LinearizedTrack() = default;
  LinearizedTrack(const LinearizedTrack&) = default;
  LinearizedTrack(LinearizedTrack&&) = default;
  LinearizedTrack& operator=(const LinearizedTrack&) = default;
  LinearizedTrack& operator=(LinearizedTrack&&) = default;
  ~LinearizedTrack() = default;
  
  LinearizedTrack* clone() const;

  /**
   *Output Method for MsgStream, to be overloaded by child classes
   */
  MsgStream& dump(MsgStream& sl) const;

  /**
   * Output Method for std::ostream, to be overloaded by child classes
   */
  std::ostream& dump(std::ostream& sl) const;


  /**
   * Position Jacobian access
   */
  const AmgMatrix(5, 3) & positionJacobian() const
  {
    return m_PositionJacobian;
  }

  /**
   * Momentum Jacobian access
   */
  const AmgMatrix(5, 3) & momentumJacobian() const
  {
    return m_MomentumJacobian;
  }

  /**
   * Access to the expected position at point of
   * closet approach
   */
  const Amg::Vector3D& expectedPositionAtPCA() const
  {
    return m_ExpectedPositionAtPOCA;
  }

  /**
   * Access to the expected momentum at point of
   * closet approach
   */
  const Amg::Vector3D& expectedMomentumAtPCA() const
  {
    return m_ExpectedMomentumAtPOCA;
  }

  /**
   * A constant term of Taylor expansion
   */
  const AmgVector(5) & constantTerm() const { return m_ConstantTerm; }

  /**
   * Access to the covariance and weight matrix of the
   * trajectory state. The parametrization obtained
   * through these methods depends on the actual TrackParameters
   * current LinearizedTrack was created with. So far it can
   * be perigee for charged track or "quasy perigee" for
   * a neutral one. In the latter case, the 1/p is used instead of
   * the corresponding q/p data member.
   */
  const AmgSymMatrix(5) & expectedCovarianceAtPCA() const;

  /**
   * Access to the expected weight matrix at point of closest approach.
   * Note that m_ExpectedCovarianceAtPOCA stores a covariance matrix, this
   * function will just invert it. No caching mechanism is implemented for now.
   */
  AmgSymMatrix(5) expectedWeightAtPCA() const;

  /**
   * Access to the parameters at point of closest approach
   */
  const AmgVector(5) & expectedParametersAtPCA() const;

  /**
   * Class validity check
   */
  bool isValid() const { return m_vl; }

  /**
   * An access to an actual linearization point.
   */
  const Amg::Vector3D& linearizationPoint() const { return m_linPoint; }

private:
  AmgVector(5) m_ExpectedParametersAtPOCA;
  AmgSymMatrix(5) m_ExpectedCovarianceAtPOCA;
  Amg::Vector3D m_linPoint;
  AmgMatrix(5, 3) m_PositionJacobian;
  AmgMatrix(5, 3) m_MomentumJacobian;
  Amg::Vector3D m_ExpectedPositionAtPOCA;
  Amg::Vector3D m_ExpectedMomentumAtPOCA;
  AmgVector(5) m_ConstantTerm;

  bool m_vl = false; // validity flag: false if the class was created with
                     // default constructor, true otherwise

}; // end of class definitions

/**
 * Overload of << operator for both, MsgStream and std::ostream
 * for debug output
 */
MsgStream&
operator<<(MsgStream& sl, const LinearizedTrack& sf);
std::ostream&
operator<<(std::ostream& sl, const LinearizedTrack& sf);

} // end of namespace definitions
#endif
