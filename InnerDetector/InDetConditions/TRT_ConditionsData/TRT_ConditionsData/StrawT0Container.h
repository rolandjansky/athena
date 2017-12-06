/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 @file StrawT0Container.h 
 
 @brief  Class for storing/accessing trt t0 data

 @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>
*/
// $Id: StrawT0Container.h,v 1.7 2007-06-13 11:10:44 hansenph Exp $

#ifndef TRT_CONDITIONSDATA_STRAWT0CONTAINER_H
#define TRT_CONDITIONSDATA_STRAWT0CONTAINER_H

#include <string>
#include "AthenaKernel/CLASS_DEF.h"
#include "TRT_ConditionsData/NestedContainer.h"
#include "TRT_ConditionsData/LinearPacker.h"
#include "TRT_ConditionsData/StrawT0.h"

namespace TRTCond
{

  /** @class StrawT0Trait
      trait class to deal with StrawT0 in NestedContainers */
  class StrawT0Trait
  {
  public:
    static void erase( StrawT0& x) { x = StrawT0() ; }
    static void copy( StrawT0& out, const StrawT0& in) { out = in ; }
    static bool isvalid( const StrawT0& x ) { return x.isvalid() ; }
    static void initialize( StrawT0& x) { x = StrawT0() ; }
    static size_t footprint( const StrawT0& x) { return sizeof(x) ; }
    static void print(const StrawT0& x) { x.print() ; }
    static StrawT0 initialvalue() { return StrawT0() ; }
    static bool isequal(const StrawT0& x, const StrawT0& y) { return x==y ; }
  } ;


  /// class for storing/accessing trt t0 data
  template <int LEVEL>
  class StrawT0ContainerTemplate : public NestedContainer<LEVEL, StrawT0,StrawT0Trait>
  {
  private:
    /// packing borders
    LinearPacker<float,unsigned char> m_t0packer ;
    LinearPacker<float,unsigned char> m_t0errpacker ;
  public:
    /// constructor
    StrawT0ContainerTemplate() : m_t0packer(0,64),m_t0errpacker(0,16) {}

    /// destructor
    virtual ~StrawT0ContainerTemplate() {}

    /// access to unpacked t0
    float getT0(const ExpandedIdentifier& id) const { 
      const StrawT0& sd = NestedContainer<LEVEL,StrawT0,StrawT0Trait>::get(id) ;
      return StrawT0Trait::isvalid(sd) ? m_t0packer.unpack(sd.packedt0()) : 0 ;
    }

    /// access to unpacked t0 error
    float getT0Err(const ExpandedIdentifier& id) const { 
      const StrawT0& sd = NestedContainer<LEVEL,StrawT0,StrawT0Trait>::get(id) ;
      return StrawT0Trait::isvalid(sd) ? m_t0errpacker.unpack(sd.packedt0err()) : 0 ;
    }
    
    /// set t0
    void setT0(const ExpandedIdentifier& id, float t0, float t0err) {
      NestedContainer<LEVEL,StrawT0,StrawT0Trait>::set(id,StrawT0(m_t0packer.pack(t0),m_t0errpacker.pack(t0err))) ;
    }

    /// set the top-level t0
    void setT0(float t0, float t0err) {
      NestedContainer<LEVEL,StrawT0,StrawT0Trait>::set(StrawT0(m_t0packer.pack(t0),m_t0errpacker.pack(t0err))) ;
    }
    
    /// public method to unpack a StrawT0 object
    void unpack(const StrawT0& sd, float& t0 , float& t0err) const {
      t0    = m_t0packer.unpack(sd.packedt0()) ;
      t0err = m_t0errpacker.unpack(sd.packedt0err()) ;
    }

    /// more public methods
    float unpackT0( const StrawT0& sd ) const { return m_t0packer.unpack(sd.packedt0()); }
    float unpackT0Err( const StrawT0& sd ) const { return m_t0errpacker.unpack(sd.packedt0err()); }

    /// change the packing limits
    void setPackingLimits( float t0min, float t0max, float t0errmin, float t0errmax) ;
      
    /// classname used to talk to iovservice
    static const char* classname() { return "TRTCond::StrawT0Container" ; }
  } ;

  template <int LEVEL>
  void StrawT0ContainerTemplate<LEVEL>::setPackingLimits( float t0min, float t0max, float t0errmin, float t0errmax)
  {
    /**  create the new packers */
    LinearPacker<float,unsigned char> newt0packer(t0min,t0max) ;
    LinearPacker<float,unsigned char> newt0errpacker(t0errmin,t0errmax) ;
    LinearPacker<float,unsigned char> oldt0packer = m_t0packer ;
    LinearPacker<float,unsigned char> oldt0errpacker = m_t0errpacker ;

    /** update all entries in the container. */
    typename NestedContainer<LEVEL,StrawT0,StrawT0Trait>::FlatContainer entries ;
    this->getall( entries ) ;
    for(typename NestedContainer<LEVEL,StrawT0,StrawT0Trait>::FlatContainer::iterator 
	  it = entries.begin() ; it != entries.end(); ++it) {
      StrawT0& sd = const_cast<StrawT0&>(*(it->second)) ;
      float t0    = oldt0packer.unpack(sd.packedt0()); 
      float t0err = oldt0errpacker. unpack(sd.packedt0err());
      unsigned char newpackedt0    = newt0packer.pack(t0) ;
      unsigned char newpackedt0err = newt0errpacker.pack(t0err) ;
      sd = StrawT0( newpackedt0, newpackedt0err ) ;
    }
    m_t0packer = newt0packer ;
    m_t0errpacker = newt0errpacker ;
  }

  // The typedef gives problems when generating the reflex dictionary
  //typedef StrawT0ContainerTemplate<ExpandedIdentifier::DETECTOR> StrawT0Container ;
  /** @class StrawT0Container
     toplevel container for storing T0
  */
  class StrawT0Container : public StrawT0ContainerTemplate<ExpandedIdentifier::DETECTOR> 
  {
  } ;
}

CLASS_DEF(TRTCond::StrawT0Container,1099535105,1)

#endif
