/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_POINT_H
#define FPTRACKER_POINT_H

#include <ostream>
#include <string>
namespace FPTracker{

  class TransversePoint;

  class Point{
  public:
    Point();
    Point(double, double, double);
    Point& operator+=(const Point& rhs);
    Point& operator-=(const Point& rhs);
    Point& operator+=(const TransversePoint& rhs);
    Point& operator-=(const TransversePoint& rhs);
    double  operator[](unsigned int) const;
    double& operator[](unsigned int);
    Point& operator*=(double);
    TransversePoint transverse()  const;
    double perp2()                const;
    std::string str()             const;
    double x()                    const;
    double y()                    const;
    double z()                    const;
    double mag2()                 const;
    double mag()                  const;
  private:
    double m_x;
    double m_y;
    double m_z;
  };

  Point operator+(const Point& lhs, const Point& rhs);
  Point operator-(const Point& lhs, const Point& rhs);
  Point operator+(const Point& lhs, const TransversePoint& rhs);
  Point operator-(const Point& lhs, const TransversePoint& rhs);
  Point operator*(double, const Point& lhs);
  std::ostream& operator<<(std::ostream&, const Point&);
}
#endif
