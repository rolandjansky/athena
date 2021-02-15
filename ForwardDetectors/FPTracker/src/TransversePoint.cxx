/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/TransversePoint.h"
#include <sstream>
#include <cmath>
#include <iomanip>
#include <stdexcept>
namespace FPTracker{
  TransversePoint::TransversePoint():m_x(0.), m_y(0.){
  }
  TransversePoint::TransversePoint(double x, double y):m_x(x), m_y(y){
  }

  double TransversePoint::x() const {return m_x;}
  double TransversePoint::y() const {return m_y;}

  TransversePoint&   TransversePoint::operator*=(double scalar) {
    m_x *= scalar;
    m_y *= scalar;
    return *this;
  }
  
  double TransversePoint::mag2() const {return m_x*m_x+m_y*m_y;}
   
  double TransversePoint::mag() const 
  {
    return std::sqrt(mag2());
  }
  
  double TransversePoint::operator[](unsigned int i) const 
  {
    if (i>1)
      {
	std::ostringstream ost;
	ost<<"TranversePoint index out of range: "<<i<<'\n';
	throw std::range_error(ost.str()); 
      }
    if(i == 0){return m_x;}
    return m_y;
  }
  
  double& TransversePoint::operator[](unsigned int i){
    if ( i>1) 
      {
	std::ostringstream ost;
	ost<<"TranversePoint index out of range: "<<i<<'\n';
	throw std::range_error(ost.str()); 
      }

    if(i == 0){return m_x;}
    return m_y;
  }

  TransversePoint& TransversePoint::operator+=(const TransversePoint& rhs){
    m_x += rhs.m_x;
    m_y += rhs.m_y;
    return *this;
  }
  TransversePoint& TransversePoint::operator-=(const TransversePoint& rhs){
    m_x -= rhs.m_x;
    m_y -= rhs.m_y;
    return *this;
  }
  std::string TransversePoint::str() const{
    std::ostringstream ost;
    ost<<std::setprecision(3)<<std::scientific<<"x\t"<<m_x<<" y\t"<<m_y;
    return ost.str();
  }
  TransversePoint operator+(const TransversePoint& lhs, const TransversePoint& rhs){
    TransversePoint point(lhs);
    return point += rhs;
  }

  TransversePoint operator-(const TransversePoint& lhs, const TransversePoint& rhs){
    TransversePoint point(lhs);
    return point -= rhs;
  }
  std::ostream& operator<<(std::ostream& os, const TransversePoint& p){
    os<<p.str();
    return os;
  }

  TransversePoint  operator*(double scalar, const TransversePoint& point) {
    TransversePoint p = point;
    return p*=scalar;
  }
  

}
