/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// NoiseOnSurface.cxx , (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include "GaudiKernel/MsgStream.h"
#include "TrkPatternParameters/NoiseOnSurface.h"

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& Trk::operator << 
  (std::ostream& sl,const Trk::NoiseOnSurface& se)
{ 
  return se.dump(sl); 
}   

MsgStream& Trk::operator    << 
(MsgStream& sl, const Trk::NoiseOnSurface& se)
{
  return se.dump(sl);
}
///////////////////////////////////////////////////////////////////
// Print track parameters information
///////////////////////////////////////////////////////////////////

std::ostream& Trk::NoiseOnSurface::dump( std::ostream& out ) const
{
  out<<"Noise(Azi,Pol,Mom,cMom) :"
     <<std::setw(15)<<std::setprecision(9)<< m_covarianceAzim 
     <<std::setw(15)<<std::setprecision(9)<< m_covariancePola 
     <<std::setw(15)<<std::setprecision(9)<< m_covarianceIMom 
     <<std::setw(15)<<std::setprecision(9)<< m_correctionIMom;

  return out;
}
///////////////////////////////////////////////////////////////////
// Print track parameters information
///////////////////////////////////////////////////////////////////

MsgStream& Trk::NoiseOnSurface::dump(MsgStream& out) const
{
  out<<"Noise(Azi,Pol,Mom,cMom) :"
     <<std::setw(15)<<std::setprecision(9)<< m_covarianceAzim 
     <<std::setw(15)<<std::setprecision(9)<< m_covariancePola 
     <<std::setw(15)<<std::setprecision(9)<< m_covarianceIMom 
     <<std::setw(15)<<std::setprecision(9)<< m_correctionIMom;

  return out;
}
