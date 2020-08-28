/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTCLASSSIMPLECONTAINERS_H
#define TESTCLASSSIMPLECONTAINERS_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <ostream>

class TestClassSimpleContainers {

public:
  TestClassSimpleContainers():
    m_vector_of_int(),
    m_list_of_double(),
    m_map_of_string_to_long(),
    m_set_of_ushort(),
    m_deque_of_float()
  {}
    
  ~TestClassSimpleContainers() {}
  
  void setNonZero();
  bool operator==( const TestClassSimpleContainers& rhs ) const;
  bool operator!=( const TestClassSimpleContainers& rhs ) const;
  std::ostream& streamOut( std::ostream& os ) const;
  
  std::vector< int > m_vector_of_int;
  std::list< double > m_list_of_double;
  std::map< std::string, long > m_map_of_string_to_long;
  std::set< unsigned short > m_set_of_ushort;
  std::deque< float > m_deque_of_float;
};


inline void
TestClassSimpleContainers::setNonZero()
{
  m_vector_of_int.clear();
  m_vector_of_int.push_back( 1 );
  m_list_of_double.clear();
  m_list_of_double.push_back( 2 );
  m_list_of_double.push_back( 3 );
  m_map_of_string_to_long.clear();
  m_map_of_string_to_long.insert( std::make_pair( std::string( "key1" ), 4L ) );
  m_map_of_string_to_long.insert( std::make_pair( std::string( "key2" ), 5L ) );
  m_set_of_ushort.clear();
  m_set_of_ushort.insert( 6 );
  m_set_of_ushort.insert( 7 );
  m_set_of_ushort.insert( 8 );
  m_deque_of_float.clear();
  m_deque_of_float.push_back( 9 );
  m_deque_of_float.push_front( 10 );
}

inline std::ostream&
TestClassSimpleContainers::streamOut( std::ostream& os ) const
{
  os << "m_vector_of_int( " << m_vector_of_int.size() << " ) :";
  for ( std::vector< int >::const_iterator i = m_vector_of_int.begin();
        i != m_vector_of_int.end(); ++i ) {
    os << " " << *i;
  }
  os << std::endl;
  os << "m_list_of_double( " << m_list_of_double.size() << " ) :";
  for ( std::list< double >::const_iterator i = m_list_of_double.begin();
        i != m_list_of_double.end(); ++i ) {
    os << " " << *i;
  }
  os << std::endl;
  os << "m_map_of_string_to_long( " << m_map_of_string_to_long.size() << " ) :";
  for ( std::map< std::string, long >::const_iterator i = m_map_of_string_to_long.begin();
        i != m_map_of_string_to_long.end(); ++i ) {
    os << " (" << i->first << "," << i->second << ")";
  }
  os << std::endl;
  os << "m_set_of_ushort( " << m_set_of_ushort.size() << " ) :";
  for ( std::set< unsigned short >::const_iterator i = m_set_of_ushort.begin();
        i != m_set_of_ushort.end(); ++i ) {
    os << " " << *i;
  }

  os << std::endl;
  os << "m_deque_of_float( " << m_deque_of_float.size() << " ) :";
  for ( std::deque< float >::const_iterator i = m_deque_of_float.begin();
        i != m_deque_of_float.end(); ++i ) {
    os << " " << *i;
  }

  return os;
}

inline bool
TestClassSimpleContainers::operator==( const TestClassSimpleContainers& rhs ) const
{
  // Check first the sizes of the containers
  if ( m_vector_of_int.size() != rhs.m_vector_of_int.size() ) return false;
  if ( m_list_of_double.size() != rhs.m_list_of_double.size() ) return false;
  if ( m_map_of_string_to_long.size() != rhs.m_map_of_string_to_long.size() ) return false;
  if ( m_set_of_ushort.size() != rhs.m_set_of_ushort.size() ) return false;/*
                                                                             if ( m_deque_of_float.size() != rhs.m_deque_of_float.size() ) return false;*/

  // Check element by element.
  std::vector< int >::const_iterator this_i_vector_of_int = m_vector_of_int.begin();
  std::vector< int >::const_iterator rhs_i_vector_of_int = rhs.m_vector_of_int.begin();
  while ( this_i_vector_of_int != m_vector_of_int.end() ) {
    if ( *this_i_vector_of_int != *rhs_i_vector_of_int ) return false;
    ++this_i_vector_of_int;
    ++rhs_i_vector_of_int;
  }

  std::list< double >::const_iterator this_i_list_of_double = m_list_of_double.begin();
  std::list< double >::const_iterator rhs_i_list_of_double = rhs.m_list_of_double.begin();
  while ( this_i_list_of_double != m_list_of_double.end() ) {
    if ( *this_i_list_of_double != *rhs_i_list_of_double ) return false;
    ++this_i_list_of_double;
    ++rhs_i_list_of_double;
  }

  std::map< std::string, long >::const_iterator this_i_map_of_string_to_long = m_map_of_string_to_long.begin();
  std::map< std::string, long >::const_iterator rhs_i_map_of_string_to_long = rhs.m_map_of_string_to_long.begin();
  while ( this_i_map_of_string_to_long != m_map_of_string_to_long.end() ) {
    if ( this_i_map_of_string_to_long->first != rhs_i_map_of_string_to_long->first ||
         this_i_map_of_string_to_long->second != rhs_i_map_of_string_to_long->second ) return false;
    ++this_i_map_of_string_to_long;
    ++rhs_i_map_of_string_to_long;
  }

  std::set< unsigned short >::const_iterator this_i_set_of_ushort = m_set_of_ushort.begin();
  std::set< unsigned short >::const_iterator rhs_i_set_of_ushort = rhs.m_set_of_ushort.begin();
  while ( this_i_set_of_ushort != m_set_of_ushort.end() ) {
    if ( *this_i_set_of_ushort != *rhs_i_set_of_ushort ) return false;
    ++this_i_set_of_ushort;
    ++rhs_i_set_of_ushort;
  }

  std::deque< float >::const_iterator this_i_deque_of_float = m_deque_of_float.begin();
  std::deque< float >::const_iterator rhs_i_deque_of_float = rhs.m_deque_of_float.begin();
  while ( this_i_deque_of_float != m_deque_of_float.end() ) {
    if ( *this_i_deque_of_float != *rhs_i_deque_of_float ) return false;
    ++this_i_deque_of_float;
    ++rhs_i_deque_of_float;
  }

  return true;
}

inline bool
TestClassSimpleContainers::operator!=( const TestClassSimpleContainers& rhs ) const
{
  return ( ( (*this) == rhs ) ? false : true );
}


#endif
