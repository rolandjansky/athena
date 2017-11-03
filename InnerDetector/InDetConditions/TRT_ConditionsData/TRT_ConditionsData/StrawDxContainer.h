/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file StrawDxContainer.h 

 @brief 
 Class for storing/accessing trt endpoint corrections data

 @author Peter Hansen <phansen@nbi.dk>
*/
// $Id: StrawDxContainer.h,v 1.4 2007-06-13 11:10:43 hansenph Exp $

#ifndef TRT_CONDITIONSDATA_STRAWDXCONTAINER_H
#define TRT_CONDITIONSDATA_STRAWDXCONTAINER_H

#include <string>
#include "AthenaKernel/CLASS_DEF.h"
#include "TRT_ConditionsData/NestedContainer.h"
#include "TRT_ConditionsData/LinearPacker.h"
#include "TRT_ConditionsData/StrawDx.h"

namespace TRTCond
{
  /** @class StrawDxTrait
    trait class to deal with StrawDx in NestedContainers
  */
  class StrawDxTrait
  {
  public:
    static void erase( StrawDx& x) { initialize(x) ; }
    static void copy( StrawDx& out, const StrawDx& in) { out = in ; }
    static bool isvalid( const StrawDx& x ) { return x.isvalid() ; }
    static void initialize( StrawDx& x) { x = initialvalue() ; }
    static size_t footprint( const StrawDx& x) { return sizeof(x) ; }
    static void print(const StrawDx& x) { x.print() ; }
    static StrawDx initialvalue() { return StrawDx() ; }
    static bool isequal(const StrawDx& x, const StrawDx& y) { return x==y ; }
  } ;


  /**  @class StrawDxContainerTemplate 
       Template class for storing/accessing trt endpoint correction data */
  template <int LEVEL>
  class StrawDxContainerTemplate : public NestedContainer<LEVEL, StrawDx,StrawDxTrait>
  {
  private:
    /** packing borders */
    LinearPacker<float,unsigned char> m_dxpacker ;
    LinearPacker<float,unsigned char> m_dxerrpacker ;
  public:
    /** constructor */
    StrawDxContainerTemplate() : m_dxpacker(-1.0,1.0),m_dxerrpacker(0,0.2) {}

    /** destructor */
    virtual ~StrawDxContainerTemplate() {}

    /** access to unpacked dx */
    float getDx1(const ExpandedIdentifier& id) const { 
      const StrawDx& sd = NestedContainer<LEVEL,StrawDx,StrawDxTrait>::get(id) ;
      return StrawDxTrait::isvalid(sd) ? m_dxpacker.unpack(sd.packeddx1()) : 0 ;
    }
    float getDx2(const ExpandedIdentifier& id) const { 
      const StrawDx& sd = NestedContainer<LEVEL,StrawDx,StrawDxTrait>::get(id) ;
      return StrawDxTrait::isvalid(sd) ? m_dxpacker.unpack(sd.packeddx2()) : 0 ;
    }
    /** access to unpacked dx error */
    float getDxErr(const ExpandedIdentifier& id) const { 
      const StrawDx& sd = NestedContainer<LEVEL,StrawDx,StrawDxTrait>::get(id) ;
      return m_dxerrpacker.unpack(sd.packeddxerr());
    }
    
    /** set dx */
    void setDx(const ExpandedIdentifier& id, float dx1, float dx2, float dxerr) {
      NestedContainer<LEVEL,StrawDx,StrawDxTrait>::set(id,StrawDx(m_dxpacker.pack(dx1),m_dxpacker.pack(dx2),m_dxerrpacker.pack(dxerr))) ;
    }

    /** set the top-level dx */
    void setDx(float dx1, float dx2, float dxerr) {
      NestedContainer<LEVEL,StrawDx,StrawDxTrait>::set(StrawDx(m_dxpacker.pack(dx1),m_dxpacker.pack(dx2),m_dxerrpacker.pack(dxerr))) ;
    }
    
    /** public method to unpack a StrawDx object */
    void unpack(const StrawDx& sd, float& dx1 , float& dx2, float& dxerr) const {
      dx1    = m_dxpacker.unpack(sd.packeddx1()) ;
      dx2    = m_dxpacker.unpack(sd.packeddx2()) ;
      dxerr = m_dxerrpacker.unpack(sd.packeddxerr()) ;
    }

    /** unpack dx near electronics */
    float unpackDx1( const StrawDx& sd ) const { return m_dxpacker.unpack(sd.packeddx1()); }
    /** unpack dx near unterminated fixation */
    float unpackDx2( const StrawDx& sd ) const { return m_dxpacker.unpack(sd.packeddx2()); }
    /** unpack dx near unterminated fixation */
    float unpackDxErr( const StrawDx& sd ) const { return m_dxerrpacker.unpack(sd.packeddxerr()); }

    /** classname used to talk to iovservice */
    static const char* classname() { return "TRTCond::StrawDxContainer" ; }
  } ;
  
  // The typedef gives problems when generating the reflex dictionary
  //typedef StrawDxContainerTemplate<ExpandedIdentifier::DETECTOR> StrawDxContainer ;
  /**  @class StrawDxContainer
       specialise to detector level
  */
  class StrawDxContainer : public StrawDxContainerTemplate<ExpandedIdentifier::DETECTOR> 
  {
  } ;
}

CLASS_DEF(TRTCond::StrawDxContainer,1162934314,1)

#endif
