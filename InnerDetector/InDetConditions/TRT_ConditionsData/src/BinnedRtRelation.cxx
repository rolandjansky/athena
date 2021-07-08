/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ConditionsData/BinnedRtRelation.h"
#include <algorithm>

namespace TRTCond
{



  bool BinnedRtRelation::isequal( const RtRelation& rhs) const 
  {
    const BinnedRtRelation* rhscast=dynamic_cast<const BinnedRtRelation*>(&rhs) ;
    return rhscast!=nullptr && *this==*rhscast ;
  }


  float BinnedRtRelation::radius( float time ) const 
  { 
    float r(0) ;
    if( time <= m_tmin )     r = m_values.front() ;
    else if(time >= m_tmax ) r = m_values.back() ;
    else {
      // interpolate
      float  rtimebin = (time - m_tmin) / binsize() ;
      size_t timebin = int(rtimebin) ;
      float  fracbin = rtimebin - timebin ;
      r = (1-fracbin)*m_values[timebin] + fracbin*m_values[timebin+1] ;
    }
    return r ;
  }

  float BinnedRtRelation::drdt( float time ) const
  {
    float dt = binsize() ;
    int timebin = int((time - m_tmin) / dt) ;
    if( time <= m_tmin )     return 0 ;
    else if(time >= m_tmax ) return 0 ;
    else return (m_values[timebin+1]-m_values[timebin])/dt ;
  }
  
  float BinnedRtRelation::drifttime(float r) const 
  {
    double t ;
    if(r <= m_values.front())     t = m_tmin ;
    else if(r >= m_values.back()) t = m_tmax ;
    else {
      // first find the bin with a binary search
      std::vector<float>::const_iterator it = std::lower_bound(m_values.begin(),m_values.end(),r) ;
      size_t timebin = it - m_values.begin() - 1 ;
      // fraction in this bin
      float fracbin = (r - m_values[timebin])/(m_values[timebin+1]-m_values[timebin]) ;
      // now get the time
      t = m_tmin + binsize() * (timebin + fracbin) ;
    }
    return t ;
  }

  std::ostream& BinnedRtRelation::write( std::ostream& os ) const 
  {
    os << m_tmin << " " << m_tmax << " " << m_values.size() << " " ;
    for(size_t i=0; i<m_values.size(); ++i) os << m_values[i] << " " ;
    return os ;
  }

  std::istream& BinnedRtRelation::read( std::istream& is ) 
  {
    size_t n(0) ;
    is >> m_tmin >> m_tmax >> n ;
    if(n<UINT_MAX){
    		   m_values.resize(n) ;
    		   for(size_t i=0; i<n; ++i) is >> m_values[i] ;
		  }
    return is ;
  }

}
