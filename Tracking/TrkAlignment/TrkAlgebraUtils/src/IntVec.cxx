/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkAlgebraUtils/IntVec.h"
#include <iostream>
#include <stdint.h>
#include <exception>

namespace Trk {

IntVec::IntVec()
  : m_Nele(0), m_ptr_data(0)
{}

IntVec::IntVec(int N) {
  m_Nele = N;
  m_ptr_data = new int[m_Nele];
  for(int i=0;i<m_Nele;i++)
    *(m_ptr_data+i)=0;
}

IntVec::IntVec(int N, int init) {
  m_Nele = N;
  m_ptr_data = new int[m_Nele];
  for(int i=0;i<m_Nele;i++)
    *(m_ptr_data+i)=init;
}

IntVec::IntVec(const IntVec& v) {
  m_Nele = v.m_Nele;
  m_ptr_data = new int[m_Nele];
  for(int i=0;i<m_Nele;i++)
    *(m_ptr_data+i)=v[i];
}

IntVec::~IntVec(){
  delete [] m_ptr_data;
}

IntVec& IntVec::operator=(const IntVec& v) {
  if(m_Nele!=0 && m_Nele!=v.m_Nele) {
    throw std::range_error( "IntVec Assignment: size does not match!" );
  }

  if ( m_ptr_data != v.m_ptr_data ) {
    reSize(v.m_Nele);
    for(int i=0;i<m_Nele;i++)
      *(m_ptr_data+i)=v[i];
  }

  return *this;
}

int& IntVec::operator[](int i) {
  if(i<0) {
    throw std::out_of_range( "IntVec: Index < zero! " );
  }
  else if(i>=m_Nele) {
    throw std::out_of_range( "IntVec: Index too large! " );
  }

  return *(m_ptr_data+i);
}

const int& IntVec::operator[](int i) const {
  if(i<0) {
    throw std::out_of_range( "IntVec: Index < zero! " );
  }
  else if(i>=m_Nele) {
    throw std::out_of_range( "IntVec: Index too large! " );
  }

  return  *(m_ptr_data+i);
}

IntVec IntVec::operator+(const IntVec& v) {
  if( m_Nele != v.m_Nele ) {
    throw std::range_error( "operator+: vectors size does not match!" );
  }

  IntVec b(m_Nele);
  for (int i=0;i<m_Nele;i++)
    b[i] = *(m_ptr_data+i) + v[i];

  return b;
}

IntVec& IntVec::operator+=(const IntVec& v) {
  if( m_Nele != v.m_Nele ) {
    throw std::range_error(  "operator+=: vectors size does not match!" );
  }

  for (int i=0;i<m_Nele;i++)
    *(m_ptr_data+i)+=v[i];

  return *this;
}

IntVec  IntVec::operator-(const IntVec& v) {
  if( m_Nele != v.m_Nele ) {
    throw std::range_error(  "operator+: vectors size does not match!" );
  }

  IntVec b(m_Nele);
  for (int i=0;i<m_Nele;i++)
    b[i] = *(m_ptr_data+i) - v[i];

  return b;
}

IntVec& IntVec::operator-=(const IntVec& v) {
  if( m_Nele != v.m_Nele ) {
    throw std::range_error(  "operator+=: vectors size does not match!" );
  }

  for (int i=0;i<m_Nele;i++)
    *(m_ptr_data+i)-=v[i];

  return *this;
}


void IntVec::reSize(int Nnew) {
  if ( Nnew>=0 && Nnew != m_Nele ) {
    int*  p = m_ptr_data;
    int Nele_old = m_Nele;
    m_ptr_data = new int[Nnew];
    m_Nele = Nnew;
    int k = m_Nele <= Nele_old ? m_Nele : Nele_old;

    p += k;
    int*  q = m_ptr_data + k;
    while (q > m_ptr_data)
      *(--q) = *(--p);

    delete [] p;
  }
}

} // end namespace Trk
