/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FitQuality.cxx (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/FitQuality.h"
#include "GaudiKernel/MsgStream.h"
#include <string>

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

