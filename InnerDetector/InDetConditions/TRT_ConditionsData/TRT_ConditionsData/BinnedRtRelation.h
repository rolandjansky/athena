/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRTBinnedRtRelation.h 

 @brief 
 parameterizes an rt-relation as a table with interpolation.

 @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>
*/

// $Id: BinnedRtRelation.h,v 1.2 2007-06-13 11:10:40 hansenph Exp $

#ifndef TRTCONDITDIONSDATA_BINNEDRTRELATION_H
#define TRTCONDITDIONSDATA_BINNEDRTRELATION_H

#include "TRT_ConditionsData/RtRelation.h"
#include <vector>

namespace TRTCond 
{

  /** @class  BinnedrtRelation
    Rt-relation parameterized by a simple array */
  class BinnedRtRelation : public RtRelation
  {
  public:  
    /** default constructor */
    BinnedRtRelation() : m_status(RtRelation::Undefined), m_tmin(0), m_tmax(25) {}

    /** normal constructor */
    BinnedRtRelation(float tmin, float tmax, const std::vector<float>& values, Status stat = Calibrated) 
      : m_status(stat), m_tmin(tmin), m_tmax(tmax), m_values(values) {}

    /** copy constructor */
    BinnedRtRelation(const BinnedRtRelation& rhs) : RtRelation(rhs) { *this = rhs ; }

    /** desctructor */
    virtual ~BinnedRtRelation() {}

    /** clone function */
    virtual RtRelation* clone() const { return new BinnedRtRelation(*this) ; }
    
    /** status flag of (calibrated) rtrelation */
    virtual int status() const { return m_status ; }
    
    /** radius for given drifttime */
    virtual float radius( float driftime ) const ;
    
    /** drifttime for given radius */
    virtual float drifttime( float radius ) const ;
    
    /** driftvelocity for given drifttime */
    virtual float drdt( float driftime ) const ;
    
    /** memory allocated by this rt-relation */
    virtual size_t footprint() const { return sizeof(*this) + m_values.size()*sizeof(float) ; }
    
    /** equality operator */
    virtual bool isequal( const RtRelation& rhs) const ;
    
    /** write to ascii ostream */
    virtual std::ostream& write(std::ostream&) const ;
    
    /** read from to ascii ostream */
    virtual std::istream& read(std::istream&) ;
    
    /** get access to minimum drifttime */
    float tmin() const { return m_tmin ; }
   /** get access to maximum drifttime */
    float tmax() const { return m_tmax ; }
   /** get access to vector of radii for each drifttime bin */
    const std::vector<float>& values() const { return m_values ; }
      
  private:
   /** get access to bin size */
    float binsize() const { return (m_tmax - m_tmin)/(m_values.size()-1) ; }

  private:
    unsigned char m_status ;        //!< status byte
    float m_tmin;                   //!< minimum drifttime
    float m_tmax ;                  //!< maximum drifttime
    std::vector<float> m_values;    //!< number of elements
  } ;
}
  
inline std::ostream& operator<<(std::ostream& os, const TRTCond::BinnedRtRelation& rtr) {
  return rtr.write(os) ; }

#endif
