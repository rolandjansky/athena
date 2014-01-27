/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_POINT_H
#define FORWARDTRACKER_POINT_H

#include "TransversePoint.h"

#include <sstream>
#include <iomanip>

namespace ForwardTracker {

  class Point {

  public:
    
    Point();
    Point(double, double, double);

    double          x()          const { return m_x; }
    double          y()          const { return m_y; }
    double          z()          const { return m_z; }
    double          perp2()      const { return m_x*m_x + m_y*m_y; }
    double          mag2()       const { return m_x*m_x + m_y*m_y + m_z*m_z; }
    double          mag()        const { return std::sqrt(mag2()); }
    TransversePoint transverse() const { return TransversePoint(m_x, m_y); }
    
    Point& operator*=(double scalar)            { m_x *= scalar; m_y *= scalar; m_z *= scalar; return *this; }
    Point& operator+=(const Point&           p) { m_x += p.x();  m_y += p.y();  m_z += p.z();  return *this; }
    Point& operator-=(const Point&           p) { m_x -= p.x();  m_y -= p.y();  m_z -= p.z();  return *this; }
    Point& operator+=(const TransversePoint& p) { m_x += p.x();  m_y += p.y();                 return *this; }
    Point& operator-=(const TransversePoint& p) { m_x -= p.x();  m_y -= p.y();                 return *this; }
    
    double  operator[](unsigned int i) const { checkIndex(i); if (i == 0) return m_x; if (i == 1) return m_y; return m_z; }
    double& operator[](unsigned int i)       { checkIndex(i); if (i == 0) return m_x; if (i == 1) return m_y; return m_z; }

    void rotat(double angle, bool bendplane);

    std::string str() const;

  private:
  
    void checkIndex(unsigned int i) const { if (i>2) throw std::range_error("Index of Point is out of range"); }
    
    double m_x;
    double m_y;
    double m_z;
  };
  
  Point operator*(double    scalar, const           Point& p);
  Point operator+(const Point& lhs, const           Point& rhs);
  Point operator-(const Point& lhs, const           Point& rhs);
  Point operator+(const Point& lhs, const TransversePoint& rhs);
  Point operator-(const Point& lhs, const TransversePoint& rhs);
  
  std::ostream& operator<<(std::ostream&, const Point&);
}

#endif
