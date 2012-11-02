/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file StrawT0.h 

 @brief  TRT straw t0 object

 @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>
*/
// $Id: StrawT0.h,v 1.3 2007-06-13 11:10:44 hansenph Exp $

#ifndef TRT_CONDITIONSDATA_STRAWT0_H
#define TRT_CONDITIONSDATA_STRAWT0_H

#include "TRT_ConditionsData/LinearPacker.h"

namespace TRTCond
{
  /** @class StrawT0
      class for t0 data for one straw */
  class StrawT0
  {
  private:
    unsigned char m_packedt0 ;
    unsigned char m_packedt0err ;
    
  public:
    /// constructors
    StrawT0() : m_packedt0(0), m_packedt0err(255) {}
    StrawT0(unsigned char t0, unsigned char t0err) : m_packedt0(t0), m_packedt0err(t0err) {}
    
    /// access to packed t0 value. packing parameters are defined by StrawT0Container
    unsigned char packedt0() const { return m_packedt0 ; }
    /// access to packed t0 error
    unsigned char packedt0err() const { return m_packedt0err ; }
    /// access to status
    bool isvalid() const { return m_packedt0err != 255 ; }
    void print() const { std::cout << "straw with (packed) t0 = " << int(m_packedt0) << std::endl ; }
    bool operator==(const StrawT0& rhs) const {
      return m_packedt0==rhs.m_packedt0 && m_packedt0err==rhs.m_packedt0err ; }
  } ;
}

#endif    
