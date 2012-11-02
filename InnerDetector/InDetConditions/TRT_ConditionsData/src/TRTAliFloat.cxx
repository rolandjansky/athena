/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TRTAliFloat.cxx
// class to hold sets of identifiers and corrections to the Rphi for
// TRT detector elements.
// Peter Hansen 1/5/04

#include <iostream>
#include "TRT_ConditionsData/TRTAliFloat.h"
#include "Identifier/Identifier.h"

int TRTAliFloat::addValue(const Identifier& ident,
                          float d0,
                          float d1,
			  float d2) {

  // get index of added objects
  int j=m_buf.size();

  // add them in to the buffer
  m_buf.resize(j+m_TRTAli_size);
  m_buf[j]=d0;
  m_buf[j+1]=d1;
  m_buf[j+1]=d2;
  // put new map entry
  m_bufmap[ident]=j;
  return j;
}
int TRTAliFloat::setValue(const Identifier& ident,
                          float d0,
                          float d1,
			  float d2) {
  // get index in array
  const int j=this->findindex(ident);
  if(j<0) return j;
  // overwrite values
  m_buf[j]=d0;
  m_buf[j+1]=d1;
  m_buf[j+1]=d2;
  return j;
}
int TRTAliFloat::getValue( const Identifier& ident,
                            float& d0,
                            float& d1,
                            float& d2 ) const {
  // get index of array
  const int j=this->findindex(ident);
  if(j<0) return j;
  // read values
  d0=m_buf[j];
  d1=m_buf[j+1];
  d2=m_buf[j+2];
  return j;
}

Identifier TRTAliFloat::getValue( unsigned int i,
                            float& d0,
                            float& d1,
                            float& d2 ) const {
  // find data of element number i

  Identifier ident;
  BufferMap::const_iterator itr=m_bufmap.begin();
  for(unsigned int count=0; count!=i;count++) {++itr;}
  ident=itr->first;
  int j=itr->second;
  d0=m_buf[j];
  d1=m_buf[j+1];
  d2=m_buf[j+2];
  return ident;
}

void TRTAliFloat::print() const {
  std::cout << "TRTAliFloat " << m_tag <<  " holds " <<
    m_bufmap.size() << " elements with " << m_size <<  " floats"
    << std::endl;
}

void TRTAliFloat::print2() const {
  print();
  int i=0;
  for (BufferMap::const_iterator itr=m_bufmap.begin(); 
      itr!=m_bufmap.end(); ++itr,++i) {
    int j=itr->second;
    std::cout << "Element " << i << " ident " << (itr->first).getString() <<
      " indx " << j << " values:" << std::endl;
    std::cout << " d0 " << m_buf[j];
    std::cout << " d1 " << m_buf[j+1];
    std::cout << " d2 " << m_buf[j+2];
  std::cout << std::endl;
  }
}	       
