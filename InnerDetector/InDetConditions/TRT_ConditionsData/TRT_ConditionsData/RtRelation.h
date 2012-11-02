/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file RtRelation.h 
 
 @brief 
 abstract base class for rt-relations

 @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>
*/
// $Id: RtRelation.h,v 1.3 2007-06-13 11:10:42 hansenph Exp $

#ifndef TRTCONDITIONSDATA_RTRELATION_H
#define TRTCONDITIONSDATA_RTRELATION_H

#include <ostream>
#include <iostream>

namespace TRTCond
{

  /** @class RtRelation
       Base class for rt-relations in the TRT */
  class RtRelation
  {
  public:  
    /** enumerates status */
    enum Status { Undefined, Calibrated } ;
    
    /** constructors, desctructors, cloners */
    RtRelation() {}
    virtual ~RtRelation() {}
    virtual RtRelation* clone() const = 0 ;
    
    /** status flag of (calibrated) rtrelation */
    virtual int status() const = 0 ;
    
    /** radius for given drifttime */
    virtual float radius( float driftime ) const = 0 ;
    
    /** drifttime for given radius */
    virtual float drifttime( float radius ) const = 0 ;
    
    /** driftvelocity for given drifttime */
    virtual float drdt( float driftime ) const = 0 ;
    
    /** memory allocated by this rt-relation */
    virtual size_t footprint() const { return sizeof(*this) ; }
    
    /** equality operator */
    virtual bool isequal( const RtRelation& rhs) const = 0 ;
    bool operator==( const RtRelation& rhs) const { return isequal(rhs) ; }
    
    /** write to ascii ostream */
    virtual std::ostream& write(std::ostream&) const = 0 ;

    /** read from ascii istream */
    virtual std::istream& read(std::istream&) = 0 ;
  } ;

}

#include "GaudiKernel/MsgStream.h"
#include <sstream>

inline std::ostream& operator<<(std::ostream& os,const TRTCond::RtRelation& rtr) {
  return rtr.write( os ) ;
}

inline std::istream& operator>>(std::istream& is, TRTCond::RtRelation& rtr) {
  return rtr.read( is ) ;
}

inline MsgStream& operator<<(MsgStream& os, const TRTCond::RtRelation& rtr) { 
  std::ostringstream message ; 
  message << rtr ;
  return os << message.str() ;
}
#endif
