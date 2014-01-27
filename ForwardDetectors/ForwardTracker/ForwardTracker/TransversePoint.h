/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_TRANSVERSEPOINT_H
#define FORWARDTRACKER_TRANSVERSEPOINT_H

#include <stdexcept>
#include <cmath>

namespace ForwardTracker {

  class TransversePoint {

  public:
    
    TransversePoint();
    TransversePoint(double, double);
    
    double x()    const { return m_x; }
    double y()    const { return m_y; }
    double mag2() const { return m_x*m_x + m_y*m_y; }
    double mag()  const { return std::sqrt(mag2()); }

    TransversePoint& operator*=(double scalar)             { m_x *= scalar; m_y *= scalar; return *this; }
    TransversePoint& operator+=(const TransversePoint& tp) { m_x += tp.x(); m_y += tp.y(); return *this; }
    TransversePoint& operator-=(const TransversePoint& tp) { m_x -= tp.x(); m_y -= tp.y(); return *this; }

    double  operator[](unsigned int i) const { checkIndex(i); if (i == 0) return m_x; return m_y; }
    double& operator[](unsigned int i)       { checkIndex(i); if (i == 0) return m_x; return m_y; }

    std::string str() const;

  private:
    
    void checkIndex(unsigned int i) const { if (i>1) throw std::range_error("Index of TransversePoint is out of range"); }

    double m_x;
    double m_y;
  };

  TransversePoint operator*(double,                 const TransversePoint&);
  TransversePoint operator+(const TransversePoint&, const TransversePoint&);
  TransversePoint operator-(const TransversePoint&, const TransversePoint&);
  
  std::ostream& operator<<(std::ostream&, const TransversePoint&);
}
#endif
