/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/Point.h"
#include "FPTracker/TransversePoint.h"
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <iomanip>
namespace FPTracker{
  Point::Point():m_x(0.), m_y(0.), m_z(0.){
  }
  Point::Point(double x, double y, double z):m_x(x), m_y(y), m_z(z){
  }

  double Point::x() const {return m_x;}
  double Point::y() const {return m_y;}
  double Point::z() const {return m_z;}

  double Point::perp2() const {return m_x*m_x+m_y*m_y;}

  TransversePoint Point::transverse() const {return TransversePoint(m_x, m_y);}

  Point&   Point::operator*=(double scalar) {
    m_x *= scalar;
    m_y *= scalar;
    m_z *= scalar;
    return *this;
  }
  
  double Point::mag2() const {return m_x*m_x+m_y*m_y+m_z*m_z;}
   
  double Point::mag() const {
    return std::sqrt(mag2());
  }
  
  double Point::operator[](unsigned int i) const {
    
    if ( i>2 )
      {
	std::ostringstream ost;
	ost<<"TranversePoint index out of range: "<<i<<'\n';
	throw std::range_error(ost.str()); 
      }

    if(i == 0){return m_x;}
    if(i == 1){return m_y;}
    return m_z;
  }
  
  double& Point::operator[](unsigned int i){
   
    if ( i>2 )
      {
	std::ostringstream ost;
	ost<<"TranversePoint index out of range: "<<i<<'\n';
	throw std::range_error(ost.str()); 
      }
    
    if(i == 0){return m_x;}
    if(i == 1){return m_y;}
    return m_z;
  }

  Point& Point::operator+=(const Point& rhs){
    m_x += rhs.m_x;
    m_y += rhs.m_y;
    m_z += rhs.m_z;
    return *this;
  }

  Point& Point::operator-=(const Point& rhs){
    m_x -= rhs.m_x;
    m_y -= rhs.m_y;
    m_z -= rhs.m_z;
    return *this;
  }

  Point& Point::operator+=(const TransversePoint& rhs){
    m_x += rhs.x();
    m_y += rhs.y();
    return *this;
  }

  Point& Point::operator-=(const TransversePoint& rhs){
    m_x -= rhs.x();
    m_y -= rhs.y();
    return *this;
  }

  std::string Point::str() const{
    std::ostringstream ost;
    ost<<std::setprecision(3)<<std::scientific<<"x\t"<<m_x<<" y\t"<<m_y<<" z\t"<<m_z;
    return ost.str();
  }

  Point operator+(const Point& lhs, const Point& rhs){
    Point point(lhs);
    return point += rhs;
  }

  Point operator-(const Point& lhs, const Point& rhs){
    Point point(lhs);
    return point -= rhs;
  }

  Point operator+(const Point& lhs, const TransversePoint& rhs){
    Point point(lhs);
    return point += rhs;
  }

  Point operator-(const Point& lhs, const TransversePoint& rhs){
    Point point(lhs);
    return point -= rhs;
  }

  std::ostream& operator<<(std::ostream& os, const Point& p){
    os<<p.str();
    return os;
  }

  Point  operator*(double scalar, const Point& point) {
    Point p = point;
    return p*=scalar;
  }
  

}
