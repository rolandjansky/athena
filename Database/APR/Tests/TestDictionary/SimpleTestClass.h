/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMPLETESTCLASS_H
#define SIMPLETESTCLASS_H

#include <ostream>

class SimpleTestClass {

public:
  SimpleTestClass();
  ~SimpleTestClass() {}
  
  void setNonZero();
  bool operator==( const SimpleTestClass& rhs ) const;
  bool operator!=( const SimpleTestClass& rhs ) const;
  std::ostream& streamOut( std::ostream& os ) const;

  int data;
};

inline void
SimpleTestClass::setNonZero()
{
  data = 1;
}

inline std::ostream&
SimpleTestClass::streamOut( std::ostream& os ) const
{
  os << "data = " << data;
  return os;
}

inline
SimpleTestClass::SimpleTestClass():
  data( 0 )
{}

inline bool
SimpleTestClass::operator==( const SimpleTestClass& rhs ) const
{
  return ( data == rhs.data );
}

inline bool
SimpleTestClass::operator!=( const SimpleTestClass& rhs ) const
{
  return ( ( (*this) == rhs ) ? false : true );
}


#endif
