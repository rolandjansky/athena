/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArWave.h"
#include <algorithm>

// private constructor from two waves
LArWave::LArWave(const LArWave& theWave1 , const LArWave& theWave2)
  : m_flag(0)
{
  if ( theWave1.m_dt == theWave2.m_dt ) {
    m_dt = theWave1.m_dt ;
    m_amplitudes.resize( std::min( theWave1.getSize(),
				   theWave2.getSize() )) ; 
  } else {
    m_amplitudes.resize(0);
    m_dt = 0. ;
  }
} 

// algebra

LArWave LArWave::operator+(const LArWave& bWave) const
{ 
  LArWave result(*this,bWave) ;
  const size_t s=result.getSize();
  for ( size_t i=0 ; i<s ; ++i ) {
    result.m_amplitudes[i] = 
      (*this).m_amplitudes[i] + bWave.m_amplitudes[i] ;
  }
  return result ;
}

LArWave& LArWave::operator+=(const LArWave& bWave)
{ 
  const size_t s=std::min(m_amplitudes.size(), bWave.getSize());
  for ( size_t i=0 ; i<s ; ++i ) {
    m_amplitudes[i] += bWave.m_amplitudes[i] ;
  }
  return *this;
}



LArWave LArWave::operator-(const LArWave& bWave) const
{ 
  LArWave result(*this,bWave) ;
  const size_t s=result.getSize();
  for ( size_t i=0 ; i<s ; ++i ) {
    result.m_amplitudes[i] = 
      (*this).m_amplitudes[i] - bWave.m_amplitudes[i] ;
  }
  return result ;
}


LArWave& LArWave::operator-=(const LArWave& bWave)
{ 
  const size_t s=std::min(m_amplitudes.size(), bWave.getSize());
  for ( size_t i=0 ; i<s ; ++i ) {
    m_amplitudes[i] -= bWave.m_amplitudes[i] ;
  }
  return *this;
}



LArWave LArWave::operator*(const LArWave& bWave) const
{ 
  LArWave result(*this,bWave) ;
  const size_t s=result.getSize();
  for ( size_t i=0 ; i<s ; ++i ) {
    result.m_amplitudes[i] = 
      (*this).m_amplitudes[i] * bWave.m_amplitudes[i] ;
  }
  return result ;
}

LArWave LArWave::operator/(const LArWave& bWave) const
{ 
  LArWave result(*this,bWave) ;
  const size_t s=result.getSize();
  for ( size_t i=0 ; i<s ; ++i ) {
    result.m_amplitudes[i] = 
      (*this).m_amplitudes[i] / bWave.m_amplitudes[i] ;
  }
  return result ;
}

LArWave LArWave::operator%(const LArWave& bWave) const
{
  //W.L., 2-Sept-09: Speed-up: 
  //This method is called several million times
  //aggressive optimization pays off.

  const double* amplPtrA=&(this->m_amplitudes.front());
  const double* amplPtrB=&(bWave.m_amplitudes.front());
    
  LArWave result(*this,bWave) ;
  const size_t s=result.getSize();
  for (size_t i=0 ; i<s ; ++i ) {
    //double& resSample=result.m_amplitudes[i];
    double sum2 = 0.5 * ( (*this).m_amplitudes[0] * bWave.m_amplitudes[i] +
			  (*this).m_amplitudes[i] * bWave.m_amplitudes[0] ) ;
    for (size_t k=1 ; k<i ; ++k ) {
      sum2 += amplPtrA[k] * amplPtrB[i-k] ;
    }
    result.m_amplitudes[i] = sum2*result.m_dt ;
  }
  return result ;
}


LArWave LArWave::operator+(const double& aBias) const {
  LArWave result(*this) ;
  std::vector<double>::iterator it=result.m_amplitudes.begin();
  std::vector<double>::iterator it_e=result.m_amplitudes.end();
  for (;it!=it_e;++it) {
    (*it)+=aBias;
  }
  return result ;
}


LArWave LArWave::operator*(const double& aScale) const{
  LArWave result(*this) ;
  std::vector<double>::iterator it=result.m_amplitudes.begin();
  std::vector<double>::iterator it_e=result.m_amplitudes.end();
  for (;it!=it_e;++it) {
    (*it)*=aScale;
  }
  return result ;
}



// Need to write this explicitly to avoid coverity warning.
LArWave& LArWave::operator= (const LArWave& other)
{
  if (this != &other) {
    m_dt = other.m_dt;
    m_amplitudes = other.m_amplitudes;
    m_flag = other.m_flag;
  }
  return *this;
}


unsigned LArWave::getIndex(const double aTime)  const 
{ return (aTime>=0 && m_dt>0) ? (unsigned)(aTime/m_dt) : getSize()+1 ; } 
