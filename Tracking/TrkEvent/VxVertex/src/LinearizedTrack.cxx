/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           LinearizedTrack.cxx  -  Description
                             -------------------
    begin   : 11/2006
    comments:
    changes :

 ***************************************************************************/

#include "VxVertex/LinearizedTrack.h"
#include "GaudiKernel/MsgStream.h"

namespace Trk {

LinearizedTrack::LinearizedTrack(const AmgVector(5) & expectedParametersAtPCA,
                                 const AmgSymMatrix(5) & expectedParErrorAtPCA,
                                 const Amg::Vector3D& linPoint,
                                 const AmgMatrix(5, 3) & positionJacobian,
                                 const AmgMatrix(5, 3) & momentumJacobian,
                                 const Amg::Vector3D& expectedPositionAtPCA,
                                 const Amg::Vector3D& expectedMomentumAtPCA,
                                 const AmgVector(5) & constantTerm)
  : m_ExpectedParametersAtPOCA(expectedParametersAtPCA)
  , m_ExpectedCovarianceAtPOCA(expectedParErrorAtPCA)
  , m_linPoint(linPoint)
  , m_PositionJacobian(positionJacobian)
  , m_MomentumJacobian(momentumJacobian)
  , m_ExpectedPositionAtPOCA(expectedPositionAtPCA)
  , m_ExpectedMomentumAtPOCA(expectedMomentumAtPCA)
  , m_ConstantTerm(constantTerm)
  , m_vl(true)
{}

// clone method implemetation
LinearizedTrack*
LinearizedTrack::clone() const
{
  if (m_vl) {
    return new LinearizedTrack(m_ExpectedParametersAtPOCA,
                               m_ExpectedCovarianceAtPOCA,
                               m_linPoint,
                               m_PositionJacobian,
                               m_MomentumJacobian,
                               m_ExpectedPositionAtPOCA,
                               m_ExpectedMomentumAtPOCA,
                               m_ConstantTerm);
  }
  return new LinearizedTrack();
} // end of clone method

const AmgSymMatrix(5) & LinearizedTrack::expectedCovarianceAtPCA() const
{
  return m_ExpectedCovarianceAtPOCA;
}

AmgSymMatrix(5) LinearizedTrack::expectedWeightAtPCA() const
{
  // the m_ExpectedCovarianceAtPOCA stores a covariance matrix, need to invert
  // to get weight matrix note: no check if matrix is invertible
  return m_ExpectedCovarianceAtPOCA.inverse();
}

const AmgVector(5) & LinearizedTrack::expectedParametersAtPCA() const
{
  return m_ExpectedParametersAtPOCA;
}

MsgStream&
LinearizedTrack::dump(MsgStream& sl) const
{
  sl << "Printing Trk::LinearizedTrack" << endmsg;
  sl << "  Expected Position at POCA: " << m_ExpectedPositionAtPOCA << endmsg;
  sl << "  Expected Momentum at POCA: " << m_ExpectedMomentumAtPOCA << endmsg;
  sl << "  Expected parameters at POCA: " << m_ExpectedParametersAtPOCA
     << endmsg;
  sl << "  Expected covariance at POCA: " << m_ExpectedCovarianceAtPOCA
     << endmsg;
  sl << "  Position jacobian: " << m_PositionJacobian << endmsg;
  sl << "  Momentum jacobian: " << m_MomentumJacobian << endmsg;
  sl << "  Constant term: " << m_ConstantTerm << endmsg;
  return sl;
}

std::ostream&
LinearizedTrack::dump(std::ostream& sl) const
{
  sl << "Printing Trk::LinearizedTrack" << std::endl;
  sl << "  Expected Position at POCA: " << m_ExpectedPositionAtPOCA
     << std::endl;
  sl << "  Expected Momentum at POCA: " << m_ExpectedMomentumAtPOCA
     << std::endl;
  sl << "  Expected parameters at POCA: " << m_ExpectedParametersAtPOCA
     << std::endl;
  sl << "  Expected covariance at POCA: " << m_ExpectedCovarianceAtPOCA
     << std::endl;
  sl << "  Position jacobian: " << m_PositionJacobian << std::endl;
  sl << "  Momentum jacobian: " << m_MomentumJacobian << std::endl;
  sl << "  Constant term: " << m_ConstantTerm << std::endl;
  return sl;
}

MsgStream&
operator<<(MsgStream& sl, const LinearizedTrack& sf)
{
  return sf.dump(sl);
}

std::ostream&
operator<<(std::ostream& sl, const LinearizedTrack& sf)
{
  return sf.dump(sl);
}

} // end of namespace
