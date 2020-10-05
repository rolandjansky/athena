/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LocalParameters.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <typeinfo>
// Trk
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/DefinedParameter.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"


const Trk::ProjectionMatricesSet Trk::LocalParameters::s_projectionMatrices(5);

Trk::LocalParameters::LocalParameters() :
  Amg::VectorX(1),
  m_parameterkey(0)
{}

Trk::LocalParameters::LocalParameters(int dim) :
  Amg::VectorX(dim),
  m_parameterkey(0)
{
  m_parameterkey |= 1 << (dim-1);
}

Trk::LocalParameters::LocalParameters( const Trk::DefinedParameter& par1) :
  Amg::VectorX(1),
  m_parameterkey(0)
{
     Amg::VectorX::operator[](0) = par1.first;
     m_parameterkey |= 1 << int(par1.second);
}

Trk::LocalParameters::LocalParameters( const Amg::Vector2D& lp) :
  Amg::VectorX(2),
  m_parameterkey(3)
{
     Amg::VectorX::operator[](0) = lp[Trk::locX];
     Amg::VectorX::operator[](1) = lp[Trk::locY];
}

Trk::LocalParameters::LocalParameters( const Trk::DefinedParameter& par1,
                                       const Trk::DefinedParameter& par2) :
  Amg::VectorX(2),
  m_parameterkey(0)
{
     Amg::VectorX::operator[](0) = par1.first;
     m_parameterkey |= 1 << int(par1.second);
     
     Amg::VectorX::operator[](1) = par2.first;
     m_parameterkey |= 1 << int(par2.second);
}

Trk::LocalParameters::LocalParameters(double tloc1,
                                      double tloc2,
                                      double tphi,
                                      double ttheta,
                                      double tqOverP) :
  Amg::VectorX(5),
  m_parameterkey(31)
{
   Amg::VectorX::operator[](0) = tloc1;
   Amg::VectorX::operator[](1) = tloc2;
   Amg::VectorX::operator[](2) = tphi;
   Amg::VectorX::operator[](3) = ttheta;
   Amg::VectorX::operator[](4) = tqOverP;
}     

Trk::LocalParameters::LocalParameters(const std::vector<Trk::DefinedParameter >& parms) :
  Amg::VectorX(parms.size()),
  m_parameterkey(0)
{
  // fill the Amg::VectorX
  std::vector<Trk::DefinedParameter>::const_iterator parIter = parms.begin();
  unsigned int idx = 0;
  
  for ( ; idx<parms.size(); ++idx){
     Amg::VectorX::operator[](idx) = parIter->first;
     m_parameterkey |= 1 << int(parIter->second);
     ++parIter;
   }
  
}

const Amg::MatrixX& Trk::LocalParameters::reductionMatrix() const 
 { return s_projectionMatrices.reductionMatrix(m_parameterkey); }

const Amg::MatrixX& Trk::LocalParameters::expansionMatrix() const 
 { return s_projectionMatrices.expansionMatrix(m_parameterkey); }


namespace Trk {


/**Overload of << operator for both, MsgStream and std::ostream for debug output*/
MsgStream& operator << ( MsgStream& sl, const Trk::LocalParameters& lp)
{ 
  std::streamsize ss = sl.precision();
  sl << std::setiosflags(std::ios::fixed)<< std::setprecision(3);
  sl << "Trk::LocalParameters " <<": ("; 
  for (int ipar=0; ipar<lp.dimension(); ++ipar)
    { sl << lp(ipar);
      if (ipar+1 < lp.dimension()) { sl << ", ";
      } else { sl << ")   - key: "<< lp.m_parameterkey << "(";}
    }
   
  for (int itag = 0, ipos=1; itag<5; ++itag, ipos*=2) 
    { bool bit = (lp.m_parameterkey & ipos);
      if (bit) { sl << "1";
      } else { sl << "0";}
    }
  sl << ")";
  sl.precision (ss); sl<<std::resetiosflags(std::ios::fixed);
    
  return sl; 
}

std::ostream& operator << ( std::ostream& sl, const Trk::LocalParameters& lp)
{ 
  std::streamsize ss = sl.precision();
  sl << std::setiosflags(std::ios::fixed)<< std::setprecision(3);
  sl << "Trk::LocalParameters " <<": ("; 
  for (int ipar=0; ipar<lp.dimension(); ++ipar)
    { sl << lp(ipar);
      if (ipar+1 < lp.dimension()) { sl << ", ";
      } else { sl << ")   - key: "<< lp.m_parameterkey << "(";}
    }
   
  for (int itag = 0, ipos=1; itag<5; ++itag, ipos*=2) 
    { bool bit = (lp.m_parameterkey & ipos);
      if (bit) { sl << "1";
      } else { sl << "0";
}
    }
  sl << ")";
  sl.precision (ss); sl<<std::resetiosflags(std::ios::fixed);

  
  return sl; 
}     


} // namespace Trk

 
