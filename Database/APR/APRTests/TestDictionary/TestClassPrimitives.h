/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTCLASSPRIMITIVES_H
#define TESTCLASSPRIMITIVES_H

#include <ostream>

class TestClassPrimitives {

public:
  TestClassPrimitives();
  ~TestClassPrimitives() {}

  void setNonZero();
  bool operator==( const TestClassPrimitives& rhs ) const;
  bool operator!=( const TestClassPrimitives& rhs ) const;
  std::ostream& streamOut( std::ostream& os ) const;
   
  bool m_bool;
  char m_char;
  unsigned char m_uchar;
  signed char m_schar;
  short m_short;
  unsigned short m_ushort;
  signed short m_sshort;
  short int m_shortint;
  unsigned short int m_ushortint;
  signed short int m_sshortint;
  int m_int;
  unsigned int m_uint;
  signed int m_sint;
  signed m_s;
  unsigned m_u;
  long m_long;
  unsigned long m_ulong;
  signed long m_slong;
  long int m_longint;
  unsigned long int m_ulongint;
  signed long int m_slongint;
  float m_float;
  double m_double;
  // long double m_longdouble;
};


inline
TestClassPrimitives::TestClassPrimitives():
  m_bool( false ),
  m_char( 0 ),
  m_uchar( 0 ),
  m_schar( 0 ),
  m_short( 0 ),
  m_ushort( 0 ),
  m_sshort( 0 ),
  m_shortint( 0 ),
  m_ushortint( 0 ),
  m_sshortint( 0 ),
  m_int( 0 ),
  m_uint( 0 ),
  m_sint( 0 ),
  m_s( 0 ),
  m_u( 0 ),
  m_long( 0 ),
  m_ulong( 0 ),
  m_slong( 0 ),
  m_longint( 0 ),
  m_ulongint( 0 ),
  m_slongint( 0 ),
  m_float( 0 ),
  m_double( 0 )
{}

inline void
TestClassPrimitives::setNonZero()
{
  m_bool = true;
  m_char = 1;
  m_uchar = 2;
  m_schar = 3;
  m_short = 4;
  m_ushort = 5;
  m_sshort = 6;
  m_shortint = 7;
  m_ushortint = 8;
  m_sshortint = 9;
  m_int = 10;
  m_uint = 11;
  m_sint = 12;
  m_s = 13;
  m_u = 14;
  m_long = 15;
  m_ulong = 16;
  m_slong = 17;
  m_longint = 18;
  m_ulongint = 19;
  m_slongint = 20;
  m_float = 21;
  m_double = 22;
}

inline std::ostream&
TestClassPrimitives::streamOut( std::ostream& os ) const
{
  os << "m_bool = " << static_cast<int>( m_bool ) << std::endl
     << "m_char = " << static_cast<int>( m_char ) << std::endl
     << "m_uchar = " << static_cast<int>( m_uchar ) << std::endl
     << "m_schar = " << static_cast<int>( m_schar ) << std::endl
     << "m_short = " << m_short << std::endl
     << "m_ushort = " << m_ushort << std::endl
     << "m_sshort = " << m_sshort << std::endl
     << "m_shortint = " << m_shortint << std::endl
     << "m_ushortint = " << m_ushortint << std::endl
     << "m_sshortint = " << m_sshortint << std::endl
     << "m_int = " << m_int << std::endl
     << "m_uint = " << m_uint << std::endl
     << "m_sint = " << m_sint << std::endl
     << "m_s = " << m_s << std::endl
     << "m_u = " << m_u << std::endl
     << "m_long = " << m_long << std::endl
     << "m_ulong = " << m_ulong << std::endl
     << "m_slong = " << m_slong << std::endl
     << "m_longint = " << m_longint << std::endl
     << "m_ulongint = " << m_ulongint << std::endl
     << "m_slongint = " << m_slongint << std::endl
     << "m_float = " << m_float << std::endl
     << "m_double = " << m_double;
  return os;
}

inline bool
TestClassPrimitives::operator==( const TestClassPrimitives& rhs ) const
{
  return
   ( m_bool == rhs.m_bool &&
     m_char ==  rhs.m_char &&
     m_uchar ==  rhs.m_uchar &&
     m_schar ==  rhs.m_schar &&
     m_short ==  rhs.m_short &&
     m_ushort ==  rhs.m_ushort &&
     m_sshort ==  rhs.m_sshort &&
     m_shortint ==  rhs.m_shortint &&
     m_ushortint ==  rhs.m_ushortint &&
     m_sshortint ==  rhs.m_sshortint &&
     m_int ==  rhs.m_int &&
     m_uint ==  rhs.m_uint &&
     m_sint ==  rhs.m_sint &&
     m_s ==  rhs.m_s &&
     m_u ==  rhs.m_u &&
     m_long ==  rhs.m_long &&
     m_ulong ==  rhs.m_ulong &&
     m_slong ==  rhs.m_slong &&
     m_longint ==  rhs.m_longint &&
     m_ulongint ==  rhs.m_ulongint &&
     m_slongint ==  rhs.m_slongint &&
     m_float ==  rhs.m_float &&
     m_double ==  rhs.m_double );  
}

inline bool
TestClassPrimitives::operator!=( const TestClassPrimitives& rhs ) const
{
  return ( ( (*this) == rhs ) ? false : true );
}


#endif
