/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         PhiRange.cxx  -  description
                            -------------------
   begin                : 26/02/2002
   email                : e.moyse@qmul.ac.uk
***************************************************************************/


#include "TrigT1Interfaces/PhiRange.h"

#include <iostream>
#include "math.h"

namespace LVL1 {

  PhiRange::PhiRange()
    : m_min( 0.0 ), m_max( 0.0 ) {

  }

  PhiRange::PhiRange( double min, double max )
    : m_min( min ), m_max( max ) {

    checkValues();
  }

  double PhiRange::min() const {
    return m_min;
  }

  double PhiRange::max() const {
    return m_max;
  }

  /** sets range */
  void PhiRange::setRange( double min, double max ) {
    m_min = min;
    m_max = max;
    checkValues();
  }


  /** returns true if the range contains the number (i.e. if min<=number<=max).
  Because with phi ranges it loops at phi=2PI, the assumption is made that
  phiMax-phiMin<PI. If this is not true, the output from this method will be
  wrong!*/
  bool PhiRange::contains( double number ) const {

    double diff = fabs( m_min - m_max );
    if ( diff > M_PI ) {
      // range must overlap 2PI
      return ( ( m_min <= number && number <= ( 2 * M_PI ) )
               || ( ( 0 <= number && number <= m_max ) ) );
    }
    // diff must be <=M_PI
    return ( ( m_min <= number ) && ( number <= m_max ) );
  }

  /** This function makes sure m_min really is less than m_max, and checks that phi values are sensible */
  void PhiRange::checkValues() {
    // should really check for more extreme values.

//    double & tmpMin = m_min;
//    double& tmpMax = m_max;

    if ( m_min < 0.0 ) m_min = ( 2.0 * M_PI ) + m_min;
    if ( m_max > ( 2.0 * M_PI ) ) m_max -= ( 2.0 * M_PI );

    if ( fabs(m_min-m_max) > M_PI) {
      // RoI range must span 2PI wraparound. In which case m_min should have higher value, m_max lower
      if (m_min < m_max) {
         // swap them
         double temp = m_max;
         m_max = m_min;
         m_min = temp;        
      }
    }
    else if ( m_min >= m_max ) {
      // swap them
      double temp = m_max;
      m_max = m_min;
      m_min = temp;
    }
    //  }
    return;
  }

  /** returns the centre of the two ranges. The ranges are assumed to be <PI. If this is not true,
      then this function will return a centre opposite the true centre. */
  double LVL1::PhiRange::centre() const {
    double diff = fabs( m_min - m_max );
    if ( diff > M_PI ) {
      // range must overlap 2PI
      double cent = ( m_min + ( m_max - m_min + 2 * M_PI ) / 2 );
      if ( cent < 0.0 )            return ( 2.0 * M_PI ) + cent;
      if ( cent > ( 2.0 * M_PI ) ) return cent -( 2.0 * M_PI );
      return cent;
    }
    // diff must be <=M_PI
    return ( m_min + ( ( m_max - m_min ) / 2 ) );

  }

} // namespace LVL1
