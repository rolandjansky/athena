/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ConditionsData/BasicRtRelation.h"

namespace TRTCond
{

  BasicRtRelation& BasicRtRelation::operator=(const BasicRtRelation& rhs) 
 {

    if ( this != &rhs){
    	m_status = rhs.m_status ;
    	set(rhs.m_cal) ;
    }
    return *this ;
  }


  float BasicRtRelation::radius( float time ) const 
  { 
    return m_cal[0]+time*(m_cal[1]+time*(m_cal[2]+time*m_cal[3])); 
    //return r<0 ? 0 : (r>2 ? 2 : r ) ;
  }
  

  float BasicRtRelation::drdt( float time ) const 
  { 
    return m_cal[1]+time*(2*m_cal[2]+time*3*m_cal[3]); 
  }
  

  float BasicRtRelation::drifttime(float r) const 
  {
    const double precision = 0.0001 ;
    double t = 20 ;
    double residual = fabs(r) - radius(t) ;
    const unsigned char maxtries = 10 ;
    unsigned char ntries = 0 ;
    while ( fabs(residual) > precision) {
      if ( drdt(t) == 0. ) t += 0.;
      else t += residual / drdt(t) ;
      residual = r - radius(t) ;
      if(++ntries>maxtries) {
	t=-1 ;
	break ;
      }
    }
    return t ;
  }
  

  bool BasicRtRelation::isequal(const RtRelation& rhs) const 
  {
    const BasicRtRelation* rhsp = dynamic_cast<const BasicRtRelation*>(&rhs) ;
    return rhsp && (*rhsp)==*this ;
  }
  

  std::ostream& BasicRtRelation::write( std::ostream& os ) const 
  {
    return os << m_cal[0] << " " << m_cal[1] << " " 
	      << m_cal[2] << " " << m_cal[3] << " " ;
  }

  std::istream& BasicRtRelation::read( std::istream& is ) 
  {
    return is >> m_cal[0] >> m_cal[1] >> m_cal[2] >> m_cal[3] ;
  }
}
