/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         Range.cxx  -  description
                            -------------------
   begin                : 26/02/2002
   email                : e.moyse@qmul.ac.uk
***************************************************************************/

#include "TrigT1Interfaces/Range.h"

namespace LVL1 {

  Range::Range()
    : m_min( 0.0 ), m_max( 0.0 ) {

  }

  Range::Range( double min, double max )
    : m_min( min ), m_max( max ) {

    checkValues();
  }

  double Range::min() const {
    return m_min;
  }

  double Range::max() const {
    return m_max;
  }

  /** sets range */
  void Range::setRange( double min, double max ) {
    m_min = min;
    m_max = max;
    checkValues();
  }


  /** returns true if the range contains the number (i.e. if min<=number<=max) */
  bool Range::contains( double number ) const {
    return ( ( m_min <= number ) && ( number <= m_max ) );
  }

  /** This function makes sure m_min really is less than m_max */
  void Range::checkValues() {
    if ( m_min >= m_max ) {
      double temp = m_max;
      m_max = m_min;
      m_min = temp;
    }
    return;
  }

} // namepsace LVL1
