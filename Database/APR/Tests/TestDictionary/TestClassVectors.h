/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTCLASSSVECTORS_H
#define TESTCLASSSVECTORS_H

#include <string>
#include <vector>
#include <ostream>

class TestClassVectors {
public:
  TestClassVectors();
  ~TestClassVectors() {}
  
  void setNonZero();
  bool operator==( const TestClassVectors& rhs ) const;
  bool operator!=( const TestClassVectors& rhs ) const;
  std::ostream& streamOut( std::ostream& os ) const;
    
  std::vector< std::string > m_vector_of_string;
  std::vector< double > m_vector_of_double;
  std::vector< std::vector< double > > m_vector_of_vector_of_double;
};


inline
TestClassVectors::TestClassVectors():
  m_vector_of_string(),
  m_vector_of_double(),
  m_vector_of_vector_of_double()
{}

inline std::ostream&
TestClassVectors::streamOut( std::ostream& os ) const
{
  os << "m_vector_of_string( " << m_vector_of_string.size() << " ) :";
  for ( std::vector< std::string >::const_iterator i = m_vector_of_string.begin();
        i != m_vector_of_string.end(); ++i ) {
    os << " " << *i;
  }
  os << std::endl;
  os << "m_vector_of_double( " << m_vector_of_double.size() << " ) :";
  for ( std::vector< double >::const_iterator i = m_vector_of_double.begin();
        i != m_vector_of_double.end(); ++i ) {
    os << " " << *i;
  }
  os << std::endl;
  os << "m_vector_of_vector_of_double( " << m_vector_of_vector_of_double.size() << " ) :";
  for ( std::vector< std::vector< double > >::const_iterator i = m_vector_of_vector_of_double.begin();
        i != m_vector_of_vector_of_double.end(); ++i ) {
    os << std::endl << " (" << i->size() << ") :";
    for ( std::vector< double >::const_iterator j = i->begin();
          j != i->end(); ++j ) {
      os << " " << *j;
    }
  }

  return os;
}

inline void
TestClassVectors::setNonZero()
{
  m_vector_of_string.clear();
  m_vector_of_string.push_back( "lcg" );
  m_vector_of_string.push_back( "app" );
  m_vector_of_string.push_back( "pool" );
  m_vector_of_double.clear();
  m_vector_of_double.push_back( 1 );
  m_vector_of_double.push_back( 2 );
  m_vector_of_vector_of_double.clear();
  m_vector_of_vector_of_double.push_back( m_vector_of_double );
  m_vector_of_double.push_back( 3 );
}

inline bool
TestClassVectors::operator==( const TestClassVectors& rhs ) const
{
  if ( m_vector_of_string.size() != rhs.m_vector_of_string.size() ) return false;
  if ( m_vector_of_double.size() != rhs.m_vector_of_double.size() ) return false;
  if ( m_vector_of_vector_of_double.size() != rhs.m_vector_of_vector_of_double.size() ) return false;

  std::vector< std::string >::const_iterator i_vector_of_string_rhs = rhs.m_vector_of_string.begin();
  for ( std::vector< std::string >::const_iterator i = m_vector_of_string.begin();
        i != m_vector_of_string.end(); ++i, ++i_vector_of_string_rhs ) {
    if ( *i != *i_vector_of_string_rhs ) return false;
  }

  std::vector< double >::const_iterator i_vector_of_double_rhs = rhs.m_vector_of_double.begin();
  for ( std::vector< double >::const_iterator i = m_vector_of_double.begin();
        i != m_vector_of_double.end(); ++i, ++i_vector_of_double_rhs ) {
    if ( *i != *i_vector_of_double_rhs ) return false;
  }

  std::vector< std::vector< double > >::const_iterator i_vector_of_vector_of_double_rhs = rhs.m_vector_of_vector_of_double.begin();
  for ( std::vector< std::vector< double > >::const_iterator i = m_vector_of_vector_of_double.begin();
        i != m_vector_of_vector_of_double.end(); ++i, ++i_vector_of_vector_of_double_rhs ) {
    if ( i->size() != i_vector_of_vector_of_double_rhs->size() ) return false;

    std::vector< double >::const_iterator j_vector_of_vector_of_double_rhs = i_vector_of_vector_of_double_rhs->begin();
    for ( std::vector< double >::const_iterator j = i->begin();
          j != i->end(); ++j, ++j_vector_of_vector_of_double_rhs ) {
      if ( *j != *j_vector_of_vector_of_double_rhs ) return false;
    }
  }

  return true;
}


inline bool
TestClassVectors::operator!=( const TestClassVectors& rhs ) const
{
  return ( ( (*this) == rhs ) ? false : true );
}


#endif
