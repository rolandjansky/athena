/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/Point.h"

namespace ForwardTracker {

  Point::Point():                             m_x(0), m_y(0), m_z(0) {}
  Point::Point(double x, double y, double z): m_x(x), m_y(y), m_z(z) {}
  
  void Point::rotat(double angle, bool bendplane) {
    
    double ds = (*this)[bendplane];
    double dz = (*this)[2];
    
    (*this)[bendplane] = cos(angle)*ds - sin(angle)*dz;
    (*this)[2]         = sin(angle)*ds + cos(angle)*dz;    
  }

  std::string Point::str() const {
    
    std::ostringstream ost; 

    ost << std::fixed
	<< std::setprecision(9) << "(" 
 	<< std::setw(12) << m_x << ", " 
 	<< std::setw(12) << m_y << ", " 
 	<< std::setw(14) << m_z << ")";
    
    return ost.str();
  }
  
  Point operator*(double    scalar, const           Point& p)   { Point point(p);   return point *= scalar; }
  Point operator+(const Point& lhs, const           Point& rhs) { Point point(lhs); return point += rhs; }
  Point operator-(const Point& lhs, const           Point& rhs) { Point point(lhs); return point -= rhs; }
  Point operator+(const Point& lhs, const TransversePoint& rhs) { Point point(lhs); return point += rhs; }
  Point operator-(const Point& lhs, const TransversePoint& rhs) { Point point(lhs); return point -= rhs; }
  
  std::ostream& operator<<(std::ostream& os, const Point& p) { os << p.str(); return os; }
}
