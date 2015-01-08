/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ConditionsData/DinesRtRelation.h"
//comment
namespace TRTCond
{

  DinesRtRelation& DinesRtRelation::operator=(const DinesRtRelation& rhs) 
 {
    if ( this != &rhs){
    	m_status = rhs.m_status ;
    	set(rhs.m_cal) ;
    }
    return *this ;
  }


  float DinesRtRelation::radius( float time ) const 
  { 
    double rmin0 = m_cal[0];
    double rho = m_cal[1];
    double v = m_cal[2];
    double t_const = m_cal[3]; // tw-t0
    double r_squared = (4*rho*rho*sin(v*(time-t_const)/(2*rho))-rmin0*rmin0)/(1-0.25*rmin0*rmin0); 
    double r = 0;
    if (r_squared>0) r = sqrt(r_squared);

    return r;
  }
  
  float DinesRtRelation::drdt( float time ) const 
  { 
    double rmin0 = m_cal[0];
    double rho = m_cal[1];
    double v = m_cal[2];
    double t_const = m_cal[3]; // tw-t0

    
    double drdt = 
      0.5/sqrt((4*rho*rho*sin(v*(time-t_const)/(2*rho))*sin(v*(time-t_const)/(2*rho))-rmin0*rmin0)/(1-0.25*rmin0*rmin0))*
      4*rho*rho/(1-0.25*rmin0*rmin0)*
      2*sin(v*(time-t_const)/(2*rho))*
      cos(v*(time-t_const)/(2*rho))*
      v/(2*rho);

    return drdt;

  }
  
  float DinesRtRelation::drifttime(float r) const 
  {
    double rmin0 = m_cal[0];
    double rho = m_cal[1];
    double v = m_cal[2];
    double t_const = m_cal[3];// tw-t0
    double t = t_const + 2*rho/v*asin(sqrt(rmin0*rmin0*(1-0.25*r*r)+r*r)/(2*rho)); 

    return t;
  }
  

  bool DinesRtRelation::isequal(const RtRelation& rhs) const 
  {
    const DinesRtRelation* rhsp = dynamic_cast<const DinesRtRelation*>(&rhs) ;
    return rhsp && (*rhsp)==*this ;
  }
  

  std::ostream& DinesRtRelation::write( std::ostream& os ) const 
  {
    return os << m_cal[0] << " " << m_cal[1] << " " 
	      << m_cal[2] << " " << m_cal[3] << " " ;
  }

  std::istream& DinesRtRelation::read( std::istream& is ) 
  {
    return is >> m_cal[0] >> m_cal[1] >> m_cal[2] >> m_cal[3] ;
  }
}
