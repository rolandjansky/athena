/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRTBasicRtRelation.h 
 @brief 
 implements the default third order polynomial rt-relation

 @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>
*/
// $Id: BasicRtRelation.h,v 1.5 2008-12-16 14:35:47 dquarrie Exp $

#ifndef TRTCONDITDIONSDATA_BASICRTRELATION_H
#define TRTCONDITDIONSDATA_BASICRTRELATION_H

#include <cstring>
#include <math.h>
#include "TRT_ConditionsData/RtRelation.h"

namespace TRTCond 
{

  /** @class BasicRtRelation
   Default rt-relation class in the TRT: a 3rd degree polynomial */
  class BasicRtRelation : public RtRelation
  {
  public: 
     /** constructor */
    BasicRtRelation() : m_status(Undefined) { m_cal[0] = m_cal[1] = m_cal[2] = m_cal[3] = 0 ; }
    /** constructor */
    BasicRtRelation(const float * cal, Status stat = Calibrated ) 
      : m_status(stat) { set(cal) ; }
    /** constructor */
    BasicRtRelation(const std::vector<float>& cal, Status stat = Calibrated ) 
      : m_status(stat) { set(&(cal.front())) ; }
    /** constructor */
    BasicRtRelation(const BasicRtRelation& rhs) : RtRelation(rhs) { *this = rhs ; }
    /**  destructor */
    virtual ~BasicRtRelation() {}
    /** cloner */
    virtual RtRelation* clone() const { return new BasicRtRelation(*this) ; }
    
    /** assignment operator */
    BasicRtRelation& operator=(const BasicRtRelation& rhs)  ;
    
    /** equality operator */
    bool operator==(const BasicRtRelation& rhs) const { 
      return (&rhs == this) || memcmp( m_cal, rhs.m_cal, 4*sizeof(float) )==0 ;  
    }
    
    /** status flag of (calibrated) rtrelation */
    virtual int status() const { return m_status ; }
    
    /** radius for given drifttime */
    virtual float radius( float driftime ) const ;
    
    /** drifttime for given radius */
    virtual float drifttime( float radius ) const ;
    
    /** driftvelocity for given drifttime */
    virtual float drdt( float driftime ) const ;
    
    /** memory allocated by this rt-relation */
    virtual size_t footprint() const { return sizeof(*this) ; }
    
    /** equality operator */
    virtual bool isequal( const RtRelation& rhs) const ;
    
    /** write to ascii ostream */
    virtual std::ostream& write(std::ostream&) const ;
    
    /** read from to ascii ostream */
    virtual std::istream& read(std::istream&) ;
    
    /** return to calibration constants */
    const float* cal() const { return m_cal ; }
    
  private:
    /** set the status */
    void setstatus( int status ) { m_status = status ; }
    
    /** set the calibration constants */
    void set( const float * cal ) { memcpy(m_cal,cal,4*sizeof(float)) ; }
    
  private:
    unsigned char m_status ; //!< status byte
    float m_cal[4] ;         //!< calibration constants
  } ;
  
}
  
inline std::ostream& operator<<(std::ostream& os, const TRTCond::BasicRtRelation& rtr) {
  return rtr.write(os) ; }



#endif
