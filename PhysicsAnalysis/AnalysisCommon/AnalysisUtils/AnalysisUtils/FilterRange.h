///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// FilterRange.h 
// Header file for class FilterRange
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_FILTERRANGE_H 
#define ANALYSISUTILS_FILTERRANGE_H 

/** FilterRange implements the range (ie: [min, max]) the filters will use to
 *  take their filtering decisions.
 *  Internally it uses the Boost class interval but maybe one could use a
 *  SEAL class (FML/RangeSet or FML/Bound)
 */

// STL includes
#include <cfloat>
#include <iostream>

// Boost includes
#include <boost/none.hpp>
#include <boost/optional.hpp>
#include <boost/numeric/interval/interval.hpp>
#include <boost/numeric/interval/checking.hpp>
#include <boost/numeric/interval/policies.hpp>
#include <boost/numeric/interval/compare/certain.hpp>

//>Wait for SEAL 1.4.0 and const-correctness
//#include "FML/RangeSet.h" 

class FilterRange 
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: it sets the minimum value to -infinity and the
   * maximum value to +infinity
   */
  FilterRange();
  
  /** Copy constructor: 
   */
  FilterRange( const FilterRange& range );

  /** Constructor with parameters:
   *  \param min : the minimum value of the range
   *  \param max : the maximum value of the range
   */
  FilterRange( const double min, const double max );

  /** Destructor: 
   */
  virtual ~FilterRange(); 

  /** Assignment operator: 
   */
  FilterRange &operator=(const FilterRange &obj); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  /** Return the double precision for the comparaison of doubles
   */
  double precision() const;

  /** Return the minimum value of the range
   */
  double lower() const;

  /** Return the maximum value of the range
   */
  double upper() const;

  /** Return the minimum value of the range
   */
  double getMin() const;

  /** Return the maximum value of the range
   */
  double getMax() const;

  /** Define +infinity according to specific implementation
   */
  double PLUS_INF() const;
  
  /** Define -infinity according to specific implementation
   */
  double MINUS_INF() const; 

  /** return Return true if the point is in range : min <= point <= max
   */
  bool isInRange( const double point ) const;

  /** return Return true if the range is active (optimize parsing of 
   *  multiple ranges and returning the final decision)
   */
  bool isActive() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  void setPrecision( const double delta );
  void setRange( const double lower, const double upper );
  void setMin( const double minValue );
  void setMax( const double maxValue );

  /**
   * Add [xMin, xMax] interval to existing set of valid ranges
   * @param xMin - lower bound of a new valid range
   * @param xMax - upper bound of a new valid range
   */  
  void include( const double xMin, const double xMax);
  
  /**
   * Set full range (from MINUS_INF to PLUS_INF ).
   */
  void includeAll();
  
  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** add a new range [xmin,xmax] deleting previous ranges full contained
   */
  void addRange(double xMin, double xMax);

  // clean ranges present between xmin and xmax 
  //void cleanRanges(double xMin, double xMax);

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  
  typedef boost::numeric::interval<double> interval_t;

  /** The boost interval wrapped by a boost optional. 
   *  This is to allow the instantiation of uninitialised ranges
   */
  boost::optional<interval_t> m_range;

  /** Setup the wanted double precision. Default = 1e-6
   */
  double m_precision;
}; 

/////////////////////////////////////////////////////////////////// 
// Operators
/////////////////////////////////////////////////////////////////// 
bool operator==( const FilterRange& r1, const FilterRange& r2 );
bool operator!=( const FilterRange& r1, const FilterRange& r2 );
bool operator <( const FilterRange& r1, const FilterRange& r2 );
bool operator >( const FilterRange& r1, const FilterRange& r2 );
bool operator<=( const FilterRange& r1, const FilterRange& r2 );
bool operator>=( const FilterRange& r1, const FilterRange& r2 );

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
inline FilterRange::FilterRange() :
  m_range(boost::none),
  m_precision(1e-6)   
{}

inline FilterRange::FilterRange( const double min, const double max ) :
  m_range( interval_t(min, max) ),
  m_precision(1e-6)   
{
  if ( min == MINUS_INF() && max == PLUS_INF() ) {
    //std::cout << ">>> You are giving a NO-OP interval : ]"
    //      << min << "; "
    //      << max << "["
    //      << std::endl
    //      << ">>> De-activating this range for optimization purpose."
    //      << std::endl;
    m_range = boost::none;
    if ( isActive() ) {
      std::string error = "ERROR : FilterRange is ACTIVE (Should NOT BE !!)";
      std::cerr << error << std::endl;
      throw error;
    }
  } else {
    // Do nothing : everything is allright
  }
}

// Destructor
///////////////
inline FilterRange::~FilterRange() {}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
inline double FilterRange::precision() const
{
  return m_precision;
}

inline double FilterRange::lower() const 
{
  if ( m_range ) {
    return m_range->lower();
  }

  return MINUS_INF();
}

inline double FilterRange::upper() const 
{
  if ( m_range ) {
    return m_range->upper();
  }

  return PLUS_INF();
}

inline double FilterRange::getMin() const 
{
  return lower();
}

inline double FilterRange::getMax() const 
{
  return upper();
}

inline double FilterRange::PLUS_INF() const
{
  return FLT_MAX;
}

inline double FilterRange::MINUS_INF() const
{
  return -FLT_MAX;
}

inline bool FilterRange::isActive() const
{
  if ( m_range ) return true;
  return false;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
inline void FilterRange::setPrecision( const double delta )
{
  m_precision = delta;
}

inline void FilterRange::setMin( const double minValue )
{
  return setRange( minValue, getMax() );
}

inline void FilterRange::setMax( const double maxValue )
{
  return setRange( getMin(), maxValue );
}

#endif //> ANALYSISUTILS_FILTERRANGE_H
