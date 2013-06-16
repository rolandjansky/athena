/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTOverlapCalculatorBase.cxx 448474 2011-07-13 09:01:48Z krasznaa $

// Local include(s):
#include "LUTOverlapCalculatorBase.h"

namespace LVL1MUCTPI {

   LUTOverlapCalculatorBase::LUTOverlapCalculatorBase( const std::string& name )
      : m_logger( name ), m_lut(), m_thr( 3 ) {

   }

   LUTOverlapCalculatorBase::LUTOverlapCalculatorBase( const LUTOverlapCalculatorBase& calc )
      : m_logger( calc.m_logger ), m_lut( calc.m_lut ), m_thr( calc.m_thr ) {

   }

   void LUTOverlapCalculatorBase::setMaximumThreshold( int thr ) {

      m_thr = thr;
      return;
   }

   bool operator== ( const LUTOverlapCalculatorBase& calc1,
                     const LUTOverlapCalculatorBase& calc2 ) {

      if( calc1.m_lut == calc2.m_lut ) return true;
      else return false;
   }

} // namespace LVL1MUCTPI
