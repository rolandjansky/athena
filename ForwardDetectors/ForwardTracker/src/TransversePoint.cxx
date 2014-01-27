/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/TransversePoint.h"

#include <iomanip>
#include <sstream>

namespace ForwardTracker {

  TransversePoint::TransversePoint():                   m_x(0), m_y(0) {}
  TransversePoint::TransversePoint(double x, double y): m_x(x), m_y(y) {}
  
  std::string TransversePoint::str() const {
    
    std::ostringstream ost; 

    ost << std::fixed 
	<< std::setprecision(9) << "(" 
 	<< std::setw(12) << m_x << ", " 
 	<< std::setw(12) << m_y << ")";
    
    return ost.str();
  }

  TransversePoint operator*(double scalar,              const TransversePoint& p)   { TransversePoint point(p);   return point *= scalar; }
  TransversePoint operator+(const TransversePoint& lhs, const TransversePoint& rhs) { TransversePoint point(lhs); return point += rhs; }
  TransversePoint operator-(const TransversePoint& lhs, const TransversePoint& rhs) { TransversePoint point(lhs); return point -= rhs; }

  std::ostream& operator<<(std::ostream& os, const TransversePoint& p) { os << p.str(); return os; }
}
