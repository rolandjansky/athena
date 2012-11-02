/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TRTWeiFloat.cxx
// class to hold sets of identifiers and vectors of R-t parameters for
// the TRT.
// Peter Hansen 1/5/04

#include <iostream>
#include "TRT_ConditionsData/TRTWeiFloat.h"
#include "Identifier/Identifier.h"

int TRTWeiFloat::addValue(const Identifier& ident,
                          float res,
			  float htw) {

  // get index of added objects
  int j=m_buf.size();

  // add them in to the buffer
  m_buf.resize(j+m_TRTWei_size);
  m_buf[j]=res;
  m_buf[j+1]=htw;
  // put new map entry
  m_bufmap[ident]=j;
  return j;
}
int TRTWeiFloat::setValue(const Identifier& ident,
                          float res,
			  float htw) {

  // get index in array
  const int j=this->findindex(ident);
  if(j<0) return j;
  // overwrite values
  m_buf[j]=res;
  m_buf[j+1]=htw;
  return j;
}
int TRTWeiFloat::getValue( const Identifier& ident,
                            float& res,
                            float& htw ) const {
  // get index of array
  const int j=this->findindex(ident);
  if(j<0) return j;
  // read values
  res=m_buf[j];
  htw=m_buf[j+1];
  return j;
}

Identifier TRTWeiFloat::getValue( unsigned int i,
                            float& res,
                            float& htw ) const {
  // find data of element number i
  Identifier ident;
  BufferMap::const_iterator itr=m_bufmap.begin();
  for(unsigned int count=0; count!=i;count++) {++itr;}
  ident=itr->first;
  int j=itr->second;
  res=m_buf[j];
  htw=m_buf[j+1];
  return ident;
}

void TRTWeiFloat::print() const {

  std::cout << "TRTWeiFloat " << m_tag <<  " holds " <<
    m_bufmap.size() << " elements with " << m_size <<  " floats"
    << std::endl;

}

void TRTWeiFloat::print2() const {
  print();
  int i=0;
  for (BufferMap::const_iterator itr=m_bufmap.begin(); 
      itr!=m_bufmap.end(); ++itr,++i) {
    int j=itr->second;
    std::cout << "Elem " << i << " ident " << (itr->first).getString() <<
      " indx " << j << " values:";
    std::cout << " res " << m_buf[j];
    std::cout << " htw " << m_buf[j+1];
  std::cout << std::endl;
  }
}	       
