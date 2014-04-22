/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TPINTEGERVECTOR_H
#define TPINTEGERVECTOR_H

#include <cstring>
#include <vector>
#include <stdexcept>
#include "AthenaPoolUtilities/TPObjRef.h"
#include <iostream>

using namespace std;

/** @file TPIntegerVector.h
 *  @brief This file contains template definitions for Transient/Persistent converters
 *  for various vectors
 *  @author Marcin.Nowak@cern.ch
 **/


class TPIntegerVectorStorage_p1;

class TPIntegerVector_p1
{  
public:
  typedef unsigned int	value_type;
  typedef value_type*	iterator;
  typedef const value_type* const_iterator;
  
  TPIntegerVector_p1(TPIntegerVectorStorage_p1 *storage, int idx)
	: m_storage(storage), m_position(idx)
  {}
  
  size_t size() const;
  void resize( size_t  new_size );
  void reserve( size_t  new_size );
  value_type& operator[]( size_t idx );
  const value_type& operator[]( size_t idx ) const;

  void	push_back( const value_type& val );
  void	push_int( const int&   val );
  void	push_float( const float& val );
  void	push_double( const double val ); 
  void	push_TPObjRef( const TPObjRef& val );
  void	push_vTPObjRef( const std::vector<TPObjRef>& val );


  value_type front_UI( const_iterator i ) const{value_type res=(*i); i++; return res;}
  int front_int( const_iterator& i ) const;
  float front_float( const_iterator& i ) const;
  double front_double( const_iterator& i ) const;
  TPObjRef	front_TPObjRef( const_iterator& i ) const;
  std::vector<TPObjRef>	front_vTPObjRef( const_iterator& i ) const;

  iterator begin() { return &operator[](0); }
  iterator end()   { return &operator[](size()); };

  const_iterator begin() const { return &operator[](0); }
  const_iterator end()   const { return &operator[](size()); };
  
protected:
  TPIntegerVectorStorage_p1	*m_storage;
  unsigned int			m_position;
};
     


class TPIntegerVectorStorage_p1
{
public:
friend class  TPIntegerVector_p1;

  TPIntegerVectorStorage_p1() {}
  
  typedef unsigned short	position_type;

  size_t size() const {
     return m_endPos.size();
  }

  void grow() {
     if( !size() )  m_endPos.push_back(0);
     else m_endPos.push_back( m_endPos.back() );
  }
  
  TPIntegerVector_p1  operator[](int idx) {
     return TPIntegerVector_p1( this, idx );
  }
  
  TPIntegerVector_p1  back() {
     return TPIntegerVector_p1( this, size()-1 );
  }

  void reserve( size_t s ) {
     m_endPos.reserve(s);
  }
  
protected:
  std::vector< TPIntegerVector_p1::value_type >	m_data;
  std::vector<position_type>	m_endPos;
};



inline
size_t TPIntegerVector_p1::size() const {      
     if( m_position == 0 )  return m_storage->m_endPos[0];
     return m_storage->m_endPos[m_position] - m_storage->m_endPos[m_position-1];
}

inline
void TPIntegerVector_p1::resize( size_t  new_size ) {
     if( m_position != m_storage->size() - 1 )
	throw std::runtime_error("TPIntegerVector_p1::resize() only supported on the last element");
     size_t	change = new_size - size();
     m_storage->m_endPos[m_position] += change;
     m_storage->m_data.resize( m_storage->m_endPos[m_position] );
}

inline
void TPIntegerVector_p1::reserve( size_t  new_size ) {
     if( m_position != m_storage->size() - 1 )
	throw std::runtime_error("TPIntegerVector_p1::reserve() only supported on the last element");
     size_t	change = new_size - size();
     m_storage->m_data.reserve( m_storage->m_endPos[m_position] + change );
}

inline
TPIntegerVector_p1::value_type&
TPIntegerVector_p1::operator[]( size_t idx ) {
   return const_cast<value_type&>(((const TPIntegerVector_p1*)this)->operator[](idx));
}

inline
const TPIntegerVector_p1::value_type&
TPIntegerVector_p1::operator[]( size_t idx ) const {
   size_t pos = m_position? m_storage->m_endPos[m_position-1] : 0;
   return m_storage->m_data[ pos + idx ];
}

inline
void	TPIntegerVector_p1::push_back( const value_type& val )
{
   m_storage->m_data.push_back( val );
   m_storage->m_endPos[m_position]++;
}

inline
void	TPIntegerVector_p1::push_int( const int& val ){
	union {unsigned int i; int f;} m_union;
	m_union.f = val;
	m_storage->m_data.push_back( m_union.i );
	m_storage->m_endPos[m_position]++;
}
 
inline
void	TPIntegerVector_p1::push_float( const float& val ){
	union {unsigned int i; float f;} m_union;
	m_union.f = val;
	m_storage->m_data.push_back( m_union.i );
	m_storage->m_endPos[m_position]++;
} 


inline
void	TPIntegerVector_p1::push_double( const double val ){
	union {struct{unsigned int i1;unsigned int i2;} c; double d;} m_union;
	m_union.d = val;
	m_storage->m_data.push_back( m_union.c.i1 );
	m_storage->m_data.push_back( m_union.c.i2 );
	m_storage->m_endPos[m_position]+=2;
}

inline
void	TPIntegerVector_p1::push_TPObjRef( const TPObjRef& val ){
	union {struct{unsigned short i1;unsigned short i2;} c; unsigned int I;} m_union;
	m_union.c.i1 = val.typeID();
	m_union.c.i2 = val.topLevelCnvID();
	m_storage->m_data.push_back( m_union.I );
	m_storage->m_data.push_back( val.index());
	m_storage->m_endPos[m_position]+=2;
}

inline
void	TPIntegerVector_p1::push_vTPObjRef( const std::vector<TPObjRef>& val ){
	m_storage->m_data.push_back( val.size());
	m_storage->m_endPos[m_position]++;
	union {struct{unsigned short i1;unsigned short i2;} c; unsigned int I;} m_union;
	std::vector<TPObjRef>::const_iterator it=val.begin();
	for (;it!=val.end();it++){
		TPObjRef v = (*it);
		m_union.c.i1 = v.typeID();
		m_union.c.i2 = v.topLevelCnvID();
		m_storage->m_data.push_back( m_union.I );
		m_storage->m_data.push_back( v.index());
	}	
	m_storage->m_endPos[m_position]+= 2*val.size();	
}

inline
int	TPIntegerVector_p1::front_int( const_iterator& i ) const{
	union { int f; unsigned int i;} m_union;
	m_union.i = (*i); i++;
	return m_union.f;
}

inline
float	TPIntegerVector_p1::front_float( const_iterator& i ) const{
	union { float f; unsigned int i;} m_union;
	m_union.i = (*i); i++;
	return m_union.f;
}

inline
double	TPIntegerVector_p1::front_double( const_iterator& i ) const{
	union { double d; struct{unsigned int i1; unsigned int i2;}c; } m_union;
	m_union.c.i1 = (*i); i++;
	m_union.c.i2 = (*i); i++;
	return m_union.d;
}

inline
TPObjRef	TPIntegerVector_p1::front_TPObjRef( const_iterator& i ) const{
	union {struct{unsigned short i1;unsigned short i2;} c; unsigned int I;} m_union;
	m_union.I = (*i); i++;
	TPObjRef res(TPObjRef::typeID_t(m_union.c.i2, m_union.c.i1),(int) (*i)); i++;
	return res;
}

inline
std::vector<TPObjRef>	TPIntegerVector_p1::front_vTPObjRef( const_iterator& i ) const{
	int size = (*i); i++;
	union {struct{unsigned short i1;unsigned short i2;} c; unsigned int I;} m_union;
	std::vector<TPObjRef> res; 
	res.reserve(size);
	for(int it=0;it<size;it++){
		m_union.I = (*i); i++;
		res.push_back( TPObjRef(TPObjRef::typeID_t(m_union.c.i2, m_union.c.i1),(int) (*i)) ); i++;
		}
	return res;
}

#endif

