/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DetCondCFloat.cxx
// class to hold sets of identifiers and vectors of floats for detector
// element specific information
// Richard Hawkings, started 30/1/04

#include <iostream>
#include "DetDescrConditions/DetCondCFloat.h"

int DetCondCFloat::add(const Identifier& ident, const float* f) {
  // get pointer to added objects
  size_t j=m_buf.size();
  // add them in to the buffer
  m_buf.resize(j+m_size);
  for (int i=0;i<m_size;++i) m_buf[j+i]=f[i];
  // put new map entry
  m_bufmap[ident]=j;
  return j;
}

void DetCondCFloat::share(const Identifier& ident, const int index) {
  // add object which is pointing to already existing buffer space
  m_bufmap[ident]=index;
}

const float* DetCondCFloat::find(const Identifier& ident) const {
  BufferMap::const_iterator itr=m_bufmap.find(ident);
  if (itr!=m_bufmap.end()) {
    // return pointer to datastructure
    return &m_buf[itr->second];
  } else {
    // identifier not in map, return null pointer
    return 0;
  }
}

int DetCondCFloat::findindex(const Identifier& ident) const {
  BufferMap::const_iterator itr=m_bufmap.find(ident);
  if (itr!=m_bufmap.end()) {
    // return index in datastructure, for subsequent share call
    return itr->second;
  } else {
    // identifier not in map, return -ve offset to signal error
    return -1;
  }
}

void DetCondCFloat::print() const {
  std::cout << "DetCondCFloat " << m_tag << " has granularity " << m_size <<
    " with " << m_bufmap.size() << " entries and " << m_buf.size() <<
    " elements" << std::endl;
}

void DetCondCFloat::print2() const {
  print();
  int i=0;
  for (BufferMap::const_iterator itr=m_bufmap.begin(); 
      itr!=m_bufmap.end(); ++itr,++i) {
    int j=itr->second;
    std::cout << "Element " << i << " ident " << (itr->first).getString() <<
      "offset " << j << " values:";
    for (int k=j; k<j+m_size; ++k) std::cout << " " << m_buf[k];
  std::cout << std::endl;
  }
}	       
