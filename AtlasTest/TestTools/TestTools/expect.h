/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestTools_expect_h
#define TestTools_expect_h

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <cmath>
namespace Athena_test {

  /*
   * Helpers for float/double comarisons with the precision and every other type exactly
   */
  template <typename T>
  bool cmp_eq( T a, T b ) {  return a == b; }
  template<>
  bool cmp_eq<float>( float a, float b ) { return std::abs(a - b) < 1.e-4; }
  template<>
  bool cmp_eq<double>( double a, double b ) { return std::abs(a - b) < 1.e-6; }

  /*
   * Helper class, offering method to compare for equality to the value captured during construction.
   * In case of a difference the message of what is the value captured and what was expected 
   * is printed. In addition the assertion macro is used to make the test failing in this case.
   *  
   * There is also a symmetric method for checking for inequality.
   */
  template <typename T>
  class TestedValue {
  public:
    TestedValue( T v, std::string&& f, int l)
      : m_value(v),
	m_file(std::move(f)),
	m_line(l) {}
    void EXPECTED( const T& e ) {
      if ( not cmp_eq(e, m_value) ) {
	std::cerr << m_file << ":" << m_line << ": error: Test failed, " 
		  << "expected: " << e << " obtained: " << m_value << "\n";
	assert( cmp_eq(e, m_value) );
      }
    }
    void NOT_EXPECTED( const T& e ) {
      if ( cmp_eq(e, m_value) ) {
	std::cerr << m_file << ":" << m_line << ": error: Test failed, " 
		  << "NOT expected: " << e << " obtained: " << m_value << "\n";
	assert( not cmp_eq(e, m_value) );
      }
    }
  private:
    T m_value;
    std::string m_file;
    int m_line;
  };
}

#define VALUE( TESTED ) Athena_test::TestedValue<decltype(TESTED)>(TESTED, __FILE__, __LINE__).

/*
 * @brief macros (& simple class) for human readable stating assertions in unit tests
 * The syntax will be:
 * VALUE ( x ) EXPECTED ( true );  // exact comparisons
 * VALUE ( y ) EXPECTED ( "something");
 * VALUE ( z ) EXPECTED ( 3.1415 ); // this would compare with precision 1e-4 for floats and 1e-6 for doubles
 * VALUE ( t ) NOT_EXPECTED ( 0 ); // the inverted check is also possible
 */

#endif // TestTools_expect_h
