/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file StrawDx.h 

 @brief 
 TRT straw endpoint correction object.

 Holds a translation along x in the local straw system for
 each of the two ends. The first end (x1) is near the electronics (large z or
 R). Eventually, in GeoModel, the straw will be turned
 by (dx2-dx1)/strawLength around local y and translated by
 (dx2+dx1)/2 along local x.
 @author Peter Hansen <phansen@nbi.dk>
*/
// $Id: StrawDx.h,v 1.2 2007-06-13 11:10:43 hansenph Exp $

#ifndef TRT_CONDITIONSDATA_STRAWDX_H
#define TRT_CONDITIONSDATA_STRAWDX_H

#include "TRT_ConditionsData/LinearPacker.h"

namespace TRTCond
{
  /** @class StrawDx
    Class for endpoint data for one straw */
  class StrawDx
  {
  private:
    unsigned char m_packeddx1 ; //!< packed Rphi shift at electronics
    unsigned char m_packeddx2 ; //!< packed Rphi shift at wire fixation
    unsigned char m_packeddxerr ; //!< packed error (255 if invalid)
    
  public:
    
    /** constructors */
    StrawDx() 
      : m_packeddx1(0), m_packeddx2(0), m_packeddxerr(255) {}
    StrawDx(unsigned char dx1, unsigned char dx2, unsigned char dxerr) 
      : m_packeddx1(dx1), m_packeddx2(dx2), m_packeddxerr(dxerr){}
    
    /** access to packed dx1 values. Packing is defined by StrawDxContainer */
    unsigned char packeddx1() const { return m_packeddx1 ; }
    /** access to packed dx2 values. Packing is defined by StrawDxContainer */
    unsigned char packeddx2() const { return m_packeddx2 ; }
    /** access to packed dx error */
    unsigned char packeddxerr() const { return m_packeddxerr ; }
    /** access to validation flag */
    bool isvalid() const { return m_packeddxerr != 255 ; }
    /** dump */
    void print() const { std::cout << "straw with (packed) dx = " << int(m_packeddx1) << " at -z and " << int(m_packeddx2) << " at +z " << std::endl ; }
    /** assignment operator */
    bool operator==(const StrawDx& rhs) const {
      return m_packeddx1==rhs.m_packeddx1 && m_packeddx2==rhs.m_packeddx2 && m_packeddxerr==rhs.m_packeddxerr ; }
  } ;
}

#endif    
