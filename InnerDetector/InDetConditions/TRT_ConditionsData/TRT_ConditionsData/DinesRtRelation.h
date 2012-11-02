/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRTDinesRtRelation.h 
 @brief 
 implements the default third order polynomial rt-relation

 @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>
*/
// $Id: DinesRtRelation.h,v 1.5 2008-12-16 14:35:47 dquarrie Exp $

#ifndef TRTCONDITDIONSDATA_DINESRTRELATION_H
#define TRTCONDITDIONSDATA_DINESRTRELATION_H

#include <cstring>
#include <math.h>
#include "TRT_ConditionsData/RtRelation.h"

namespace TRTCond 
{

  /** @class DinesRtRelation
   Default rt-relation class in the TRT: a 3rd degree polynomial */
  class DinesRtRelation : public RtRelation
  {
  public: 
     /** constructor */
    DinesRtRelation() : m_status(Undefined) { m_cal[0] = m_cal[1] = m_cal[2] = m_cal[3] = 0 ; }
    /** constructor */
    DinesRtRelation(const float * cal, Status stat = Calibrated ) 
      : m_status(stat) { set(cal) ; }
    /** constructor */
    DinesRtRelation(const std::vector<float>& cal, Status stat = Calibrated ) 
      : m_status(stat) { set(&(cal.front())) ; }
    /** constructor */
    DinesRtRelation(const DinesRtRelation& rhs) : RtRelation(rhs) { *this = rhs ; }
    /**  destructor */
    virtual ~DinesRtRelation() {}
    /** cloner */
    virtual RtRelation* clone() const { return new DinesRtRelation(*this) ; }
    
    /** assignment operator */
    DinesRtRelation& operator=(const DinesRtRelation& rhs)  ;
    
    /** equality operator */
    bool operator==(const DinesRtRelation& rhs) const { 
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
  
inline std::ostream& operator<<(std::ostream& os, const TRTCond::DinesRtRelation& rtr) {
  return rtr.write(os) ; }



#endif
