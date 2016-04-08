/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FitQuality.cxx (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/FitQuality.h"
#include "GaudiKernel/MsgStream.h"
#include <string>

Trk::FitQuality::FitQuality()
	:
	m_chiSquared(0.0),
	m_numberDoF(0)
{
}

Trk::FitQuality::FitQuality(double chiSquared, int numberDoF)
	:
	m_chiSquared(double(chiSquared)),
	m_numberDoF(numberDoF)
{
 
}

Trk::FitQuality::FitQuality(double chiSquared, double numberDoF):
	                                m_chiSquared(chiSquared),
	                                  m_numberDoF(numberDoF)
{
}


Trk::FitQuality::FitQuality(const FitQuality& rhs)
{
	m_chiSquared=rhs.m_chiSquared;
	m_numberDoF= rhs.m_numberDoF;
}

Trk::FitQuality::~FitQuality()
{
}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::FitQuality& fq)
{ 
  std::streamsize ss = sl.precision();
  sl << std::setiosflags(std::ios::fixed)<< std::setprecision(3);
  sl << "FitQuality: \t"
     << "("<<fq.chiSquared()<<", \t"<<fq.numberDoF()<<")\t"
     << "(chi^2, ndf)";
  sl.precision (ss); sl<<std::resetiosflags(std::ios::fixed);
  return sl; 
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::FitQuality& fq)
{ 
  std::streamsize ss = sl.precision();
  sl << std::setiosflags(std::ios::fixed)<< std::setprecision(3);
  sl <<"FitQuality: \t"
     << "("<<fq.chiSquared()<<", \t"<<fq.numberDoF()<<")\t"
     << "(chi^2, ndf)";
  sl.precision (ss); sl<<std::resetiosflags(std::ios::fixed);
  return sl; 
}

