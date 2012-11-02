/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TRTRtFloat.cxx
// class to hold sets of identifiers and vectors of R-t parameters for
// the TRT.
// Peter Hansen, started 1/5/04

#include <iostream>
#include "TRT_ConditionsData/TRTRtFloat.h"
#include "Identifier/Identifier.h"

int TRTRtFloat::addValue( const Identifier& ident,
                            float T0,
                            float vd,
                            float vp,
                            float a0,
                            float a1,
                            float a2,
                            float a3 ) {

  // get index of added objects
  int j=m_buf.size();

  // add them in to the buffer
  m_buf.resize(j+m_TRTRt_size);
  m_buf[j]=T0;
  m_buf[j+1]=vd;
  m_buf[j+2]=vp;
  m_buf[j+3]=a0;
  m_buf[j+4]=a1;
  m_buf[j+5]=a2;
  m_buf[j+6]=a3;
  // put new map entry
  m_bufmap[ident]=j;
  return j;
}
int TRTRtFloat::setValue( const Identifier& ident,
                            float T0,
                            float vd,
                            float vp,
                            float a0,
                            float a1,
                            float a2,
                            float a3 ) {

  // get index of array
  const int j=this->findindex(ident);
  if(j<0) return j;
  // overwrite values
  m_buf[j]=T0;
  m_buf[j+1]=vd;
  m_buf[j+2]=vp;
  m_buf[j+3]=a0;
  m_buf[j+4]=a1;
  m_buf[j+5]=a2;
  m_buf[j+6]=a3;
  return j;
}
int TRTRtFloat::getValue( const Identifier& ident,
                            float& T0,
                            float& vd,
                            float& vp,
                            float& a0,
                            float& a1,
                            float& a2,
                            float& a3 ) const {

  // get index of array
  const int j=this->findindex(ident);
  if(j<0) return j;
  // read values
  T0=m_buf[j];
  vd=m_buf[j+1];
  vp=m_buf[j+2];
  a0=m_buf[j+3];
  a1=m_buf[j+4];
  a2=m_buf[j+5];
  a3=m_buf[j+6];
  return j;
}

Identifier TRTRtFloat::getValue( unsigned int i,
                            float& T0,
                            float& vd,
                            float& vp,
                            float& a0,
                            float& a1,
                            float& a2,
                            float& a3 ) const {
  // find data of element number i
  Identifier ident;
  BufferMap::const_iterator itr=m_bufmap.begin();
  for(unsigned int count=0; count!=i;count++) {++itr;}
  ident=itr->first;
  int j=itr->second;
  T0=m_buf[j];
  vd=m_buf[j+1];
  vp=m_buf[j+2];
  a0=m_buf[j+3];
  a1=m_buf[j+4];
  a2=m_buf[j+5];
  a3=m_buf[j+6];
  return ident;
}

void TRTRtFloat::print() const {
  std::cout << "TRTRtFloat " << m_tag << " holds " <<
    m_bufmap.size() << " elements with " << m_size <<  " floats"
    << std::endl;
}

void TRTRtFloat::print2() const {
  print();
  int i=0;
  for (BufferMap::const_iterator itr=m_bufmap.begin(); 
      itr!=m_bufmap.end(); ++itr,++i) {
    int j=itr->second;
    std::cout << "Element " << i << " ident " << (itr->first).getString() <<
      " indx " << j << " values:" << std::endl;
    std::cout << " T0 " << m_buf[j];
    std::cout << " vd " << m_buf[j+1];
    std::cout << " vp " << m_buf[j+2];
    std::cout << " a0 " << m_buf[j+3];
    std::cout << " a1 " << m_buf[j+4];
    std::cout << " a2 " << m_buf[j+5];
    std::cout << " a3 " << m_buf[j+6];
  std::cout << std::endl;
  }
}	       
