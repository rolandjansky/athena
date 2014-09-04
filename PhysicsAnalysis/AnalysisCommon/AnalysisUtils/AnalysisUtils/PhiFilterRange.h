///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhiFilterRange.h 
// Header file for class PhiFilterRange
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_PHIFILTERRANGE_H 
#define ANALYSISUTILS_PHIFILTERRANGE_H 

/** PhiFilterRange implements the range (ie: [min, max]) the filters will use
 *  to take their filtering decisions. It is a specialisation of FilterRange
 *  for the phi angles. It enforces the ATLAS policy for phi angles :
 *  \f$ \phi \in [-\pi,\pi[ \f$
 *
 *  Internally it uses the Boost class interval but maybe one could use a
 *  SEAL class (FML/RangeSet or FML/Bound)
 */

// STL includes

// AnalysisUtils includes
#include "AnalysisUtils/FilterRange.h"

class PhiFilterRange : public FilterRange
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Default constructor: it sets the minimum value to -infinity and the
  /// maximum value to +infinity
  PhiFilterRange();

  /// Copy constructor: 
  PhiFilterRange( const PhiFilterRange& range );

  /// Constructor with parameters:
  /// \param min : the minimum value of the range
  /// \param max : the maximum value of the range
  PhiFilterRange( const double min, const double max );

  /// Destructor: 
  virtual ~PhiFilterRange(); 

  /// Assignment operator: 
  PhiFilterRange &operator=(const PhiFilterRange &obj); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

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

  /*
   *return Return true if the point is in range : min <= point <= max
   */
  bool isInRange( const double point ) const;

  /// return Return true if the range is active (optimize parsing of 
  /// multiple ranges and returning the final decision)
  bool isActive() const;

  /// Convert a phi angle in the ATLAS conventional range -PI->PI
  /// inspired from FourMom package (could also use CaloPhiRange)
  double atlasPhi( const double phi ) const;
  
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
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

  /// add a new range [xmin,xmax] deleting previous ranges full contained
  void addRange(double xMin, double xMax);

  /// clean ranges present between xmin and xmax 
  //void cleanRanges(double xMin, double xMax);

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  
}; 

/////////////////////////////////////////////////////////////////// 
// Operators
/////////////////////////////////////////////////////////////////// 
bool operator==( const PhiFilterRange& r1, const PhiFilterRange& r2 );
bool operator!=( const PhiFilterRange& r1, const PhiFilterRange& r2 );
bool operator <( const PhiFilterRange& r1, const PhiFilterRange& r2 );
bool operator >( const PhiFilterRange& r1, const PhiFilterRange& r2 );
bool operator<=( const PhiFilterRange& r1, const PhiFilterRange& r2 );
bool operator>=( const PhiFilterRange& r1, const PhiFilterRange& r2 );

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
inline PhiFilterRange::PhiFilterRange() :
  FilterRange()
{}

inline PhiFilterRange::PhiFilterRange( const double min, const double max ) :
  FilterRange( min, max )
{
  if ( min == -M_PI && max == +M_PI ) {
    //std::cout << ">>> You are giving a NO-OP interval : ]"
    //      << min << "; "
    //      << max << "["
    //      << std::endl
    //      << ">>> De-activating this range for optimization purpose."
    //      << std::endl;
    m_range = boost::none;
    if ( isActive() ) {
      std::string err = "ERROR : PhiFilterRange is ACTIVE (Should NOT BE !!)";
      throw err;
    }
  } else {
    // Do nothing : everything is allright
  }
}

/// Destructor
///////////////
inline PhiFilterRange::~PhiFilterRange() {}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////
inline double PhiFilterRange::lower() const 
{
  if ( m_range ) {
    return m_range->lower();
  }

  return -M_PI;
}

inline double PhiFilterRange::upper() const 
{
  if ( m_range ) {
    return m_range->upper();
  }

  return +M_PI;
}

inline double PhiFilterRange::getMin() const 
{
  return lower();
}

inline double PhiFilterRange::getMax() const 
{
  return upper();
}

inline bool PhiFilterRange::isActive() const
{
  if ( m_range ) return true;
  return false;
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
inline void PhiFilterRange::setRange( const double lower, const double upper )
{
  return addRange( lower, upper );
}

inline void PhiFilterRange::setMin( const double minValue )
{
  return setRange( minValue, getMax() );
}

inline void PhiFilterRange::setMax( const double maxValue )
{
  return setRange( getMin(), maxValue );
}

#endif //> ANALYSISUTILS_PhiFILTERRANGE_H
