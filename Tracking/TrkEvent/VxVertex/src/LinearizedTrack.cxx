/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

    LinearizedTrack::LinearizedTrack() : 
    // m_ExpectedParametersAtPOCA(5),
    // m_ExpectedCovarianceAtPOCA(5,5),
    // m_linPoint(0.,0.,0.),
    // m_PositionJacobian(5,3,0),
    // m_MomentumJacobian(5,3,0),
    // m_ExpectedPositionAtPOCA(3),
    // m_ExpectedMomentumAtPOCA(3),
    // m_ConstantTerm(5),
    m_vl(false)
    {
      m_ExpectedParametersAtPOCA.setZero();
      m_ExpectedCovarianceAtPOCA.setZero();
      m_linPoint.setZero();
      m_PositionJacobian.setZero();
      m_MomentumJacobian.setZero();
      m_ExpectedPositionAtPOCA.setZero();
      m_ExpectedMomentumAtPOCA.setZero();
      m_ConstantTerm.setZero();
    }
    

   LinearizedTrack::LinearizedTrack(const AmgVector(5)& expectedParametersAtPCA,
                                    const AmgSymMatrix(5) &  expectedParErrorAtPCA,
				    const Amg::Vector3D & linPoint,      
				    const AmgMatrix(5,3) & positionJacobian,
				    const AmgMatrix(5,3) & momentumJacobian,
				    const Amg::Vector3D & expectedPositionAtPCA,
                                    const Amg::Vector3D & expectedMomentumAtPCA,
				    const AmgVector(5) & constantTerm):
    m_ExpectedParametersAtPOCA(expectedParametersAtPCA),
    m_ExpectedCovarianceAtPOCA(expectedParErrorAtPCA),
    m_linPoint(linPoint),
    m_PositionJacobian(positionJacobian),
    m_MomentumJacobian(momentumJacobian),
    m_ExpectedPositionAtPOCA(expectedPositionAtPCA),
    m_ExpectedMomentumAtPOCA(expectedMomentumAtPCA),
    m_ConstantTerm(constantTerm),
    m_vl(true)
    {}
  
  
  
    LinearizedTrack::LinearizedTrack(const LinearizedTrack & rhs) :
    m_ExpectedParametersAtPOCA(rhs.m_ExpectedParametersAtPOCA),
    m_ExpectedCovarianceAtPOCA(rhs.m_ExpectedCovarianceAtPOCA),
    m_linPoint(rhs.m_linPoint),
    m_PositionJacobian(rhs.m_PositionJacobian),
    m_MomentumJacobian(rhs.m_MomentumJacobian),
    m_ExpectedPositionAtPOCA(rhs.m_ExpectedPositionAtPOCA),
    m_ExpectedMomentumAtPOCA(rhs.m_ExpectedMomentumAtPOCA),
    m_ConstantTerm(rhs.m_ConstantTerm),
    m_vl(rhs.m_vl){}

  
//assignement operator implementation  
  LinearizedTrack & LinearizedTrack::operator= (const LinearizedTrack& rhs)
  {
    if (this!=&rhs)
    {
      m_ExpectedParametersAtPOCA=rhs.m_ExpectedParametersAtPOCA;
      m_ExpectedCovarianceAtPOCA=rhs.m_ExpectedCovarianceAtPOCA,
      m_linPoint= rhs.m_linPoint;
      m_PositionJacobian=rhs.m_PositionJacobian;
      m_MomentumJacobian=rhs.m_MomentumJacobian;
      m_ExpectedPositionAtPOCA=rhs.m_ExpectedPositionAtPOCA;
      m_ExpectedMomentumAtPOCA=rhs.m_ExpectedMomentumAtPOCA;
      m_ConstantTerm=rhs.m_ConstantTerm;
      m_vl = rhs.m_vl;
    }
    return *this;
  }
  
//clone method implemetation
  LinearizedTrack * LinearizedTrack::clone() const
  {
    if(m_vl) { 
      return new LinearizedTrack(m_ExpectedParametersAtPOCA,
                                 m_ExpectedCovarianceAtPOCA,
			         m_linPoint,
				 m_PositionJacobian,
				 m_MomentumJacobian,
				 m_ExpectedPositionAtPOCA,
				 m_ExpectedMomentumAtPOCA,
				 m_ConstantTerm); 
    } else return new LinearizedTrack();				       	   
  }//end of clone method 
  
  const AmgSymMatrix(5) & LinearizedTrack::expectedCovarianceAtPCA() const 
  {
    return m_ExpectedCovarianceAtPOCA;
  }
  
  const AmgSymMatrix(5) LinearizedTrack::expectedWeightAtPCA() const
  {
    //the m_ExpectedCovarianceAtPOCA stores a covariance matrix, need to invert to get weight matrix
    //note: no check if matrix is invertible
    return m_ExpectedCovarianceAtPOCA.inverse();
  }
  
  const AmgVector(5) & LinearizedTrack::expectedParametersAtPCA() const {
    return m_ExpectedParametersAtPOCA;
  }

  MsgStream& LinearizedTrack::dump(MsgStream& sl) const {
    sl << "Printing Trk::LinearizedTrack" << endreq;
    sl << "  Expected Position at POCA: "   << m_ExpectedPositionAtPOCA << endreq;
    sl << "  Expected Momentum at POCA: "   << m_ExpectedMomentumAtPOCA << endreq;
    sl << "  Expected parameters at POCA: " << m_ExpectedParametersAtPOCA << endreq;
    sl << "  Expected covariance at POCA: " << m_ExpectedCovarianceAtPOCA << endreq;
    sl << "  Position jacobian: "           << m_PositionJacobian << endreq;
    sl << "  Momentum jacobian: "           << m_MomentumJacobian << endreq;
    sl << "  Constant term: " << m_ConstantTerm << endreq;
    return sl;
  }

  std::ostream& LinearizedTrack::dump(std::ostream& sl) const {
    sl << "Printing Trk::LinearizedTrack" << std::endl;
    sl << "  Expected Position at POCA: "   << m_ExpectedPositionAtPOCA << std::endl;
    sl << "  Expected Momentum at POCA: "   << m_ExpectedMomentumAtPOCA << std::endl;
    sl << "  Expected parameters at POCA: " << m_ExpectedParametersAtPOCA << std::endl;
    sl << "  Expected covariance at POCA: " << m_ExpectedCovarianceAtPOCA << std::endl;
    sl << "  Position jacobian: "           << m_PositionJacobian << std::endl;
    sl << "  Momentum jacobian: "           << m_MomentumJacobian << std::endl;
    sl << "  Constant term: " << m_ConstantTerm << std::endl;
    return sl;
  }
  
  MsgStream& operator << ( MsgStream& sl, const LinearizedTrack& sf) 
  { return sf.dump(sl); }

  std::ostream& operator << ( std::ostream& sl, const LinearizedTrack& sf) 
  { return sf.dump(sl); }
  
  LinearizedTrack::~LinearizedTrack() {}
  
} // end of namespace
