/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TPINTEGERVECTOR_P2_H
#define TPINTEGERVECTOR_P2_H

/** @file TPIntegerVector_p2.h
 *  @brief This file contains definition of TPIntegerVectorStorage_p2, which is
 *  a storage class used in top-level TP converters, and a vector<int> proxy: TPIntegerVector_p2,
 *  used to interface with TPIntegerVectorStorage
 *  @author Marcin.Nowak@cern.ch
 **/


#include <vector>


class TPIntegerVectorStorage_p2;

class TPIntegerVector_p2
{  
public:
  typedef unsigned int	value_type;
  typedef value_type*	iterator;
  typedef const value_type* const_iterator;

  typedef TPIntegerVector_p2	IsAnIntVect;
  
  TPIntegerVector_p2(TPIntegerVectorStorage_p2 *storage, int idx)
	: m_storage( storage ),
	  m_position( idx<<1 )
  {}

  // --- size-related
  unsigned 	size() const;
  unsigned	reserved() const;
  void 		resize( size_t  new_size );
  void 		reserve( size_t  new_size );
  void		grow( const size_t diff = 1 );
  
  // --- basic INT access
  value_type& operator[]( size_t idx );
  const value_type& operator[]( size_t idx ) const;

  // --- storing
  void		push_back( const value_type& val );
  void		push_int( const int& val );
  // void		push_back_short( const short& val );   // hmmm  
  void		push_float( const float& val );
  void		push_double( const double& val );
  // void		push_long( const long long& val );

  /// store zero-terminated C string  (does not write out the size)
  void		push_chars( const char * const );
  /// store std::string  (assuming 8-bit chars).  Uses push_chars()
  void		push_string( const std::string& );
  /// store blob of a given size (will write out the size)
  void		push_bytes( const void * s, size_t len );
  
  void		push_TPObjRef( const TPObjRef& val );
  void		push_vTPObjRef( const std::vector<TPObjRef>& val );

  /// tell how much space the object will take in the INT vector, so it can be reserved
  template<typename T>
  static size_t	size_for( const T& );

  static size_t	size_for( const char * );
  static size_t	size_for_ref();
  static size_t	size_for_blob( size_t blobsize );

  /// return the size of the next string/chars 
  size_t	next_string_size( const const_iterator& iter ) const;
  /// return the size of the next blob, in bytes
  size_t	next_blob_size( const const_iterator& iter ) const;
     
  // --- retrieving
  // all NEXT methods advance the iterator to the next element
  const value_type&  next( const_iterator& iter ) const;
  int  		next_int( const_iterator& iter ) const;
  float 	next_float( const_iterator& iter ) const;
  double	next_double( const_iterator& iter ) const;
  TPObjRef	next_TPObjRef( const_iterator& iter ) const;

  // compatible with push_chars()
  std::string	next_string( const_iterator& iter ) const;
  void		next_chars( const_iterator& iter, char *buff, size_t bufflen ) const;
  void		next_bytes( const_iterator& iter, void *buff, size_t bufflen ) const;
  void		next_vTPObjRef( const_iterator& iter, std::vector<TPObjRef> & vect ) const; 
  
  // --- iterators
  iterator begin() { return &operator[](0); }
  iterator end()   { return &operator[](size()); };

  const_iterator begin() const { return &operator[](0); }
  const_iterator end()   const { return &operator[](size()); };
  
protected:
  unsigned	startPos() const;
  bool		lastElement() const;

  void		_store_bytes( const void *, size_t len );

  TPIntegerVectorStorage_p2	*m_storage;
  unsigned int			m_position;
};
     




// This class needs to be in the same header file, otherwise the methods of
// TPIntegerVector_p2 can not be inlined

/// Storage implementation for TP converters producing TP representation as series of ints.
/// This class is not accessed directly, but through a proxy class TPIntegerVector_p2
/// that pretends to be a standard vector<int>
/// TP converters should be defined using TPIntegerVector_p2 as persistent representation type

class TPIntegerVectorStorage_p2
{
public:
friend class  TPIntegerVector_p2;
  
  TPIntegerVectorStorage_p2() {}

  typedef unsigned 	position_type;
  typedef TPIntegerVector_p2::value_type 	value_type;

  size_t size() const {
     return m_endPos.size() >> 1;
  }

  void grow() {
     position_type	pos = size()? m_endPos.back() : 0;
     m_endPos.push_back( pos );
     m_endPos.push_back( pos );
  }
  
  TPIntegerVector_p2  operator[](int idx) {
     return TPIntegerVector_p2( this, idx );
  }
  
  TPIntegerVector_p2  back() {
     return TPIntegerVector_p2( this, size()-1 );
  }

  void reserve( size_t s ) {
     m_endPos.reserve( s<<1 );
  }
  
protected:
  std::vector<value_type>	m_data;
  std::vector<position_type>	m_endPos;
};


#include "AthenaPoolUtilities/TPIntegerVector_p2.icc"
     

#endif
