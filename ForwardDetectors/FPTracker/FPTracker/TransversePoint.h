/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_TRANSVERSEPOINT_H
#define FPTRACKER_TRANSVERSEPOINT_H

#include <ostream>
#include <string>
namespace FPTracker{

  class TransversePoint{
  public:
    TransversePoint();
    TransversePoint(double, double);
    TransversePoint& operator+=(const TransversePoint& rhs);
    TransversePoint& operator-=(const TransversePoint& rhs);
    double  operator[](unsigned int)     const;
    double& operator[](unsigned int);
    TransversePoint& operator*=(double);
    std::string str()           const;
    double x()                  const;
    double y()                  const;
    double mag2()               const;
    double mag()                const;
  private:
    double m_x;
    double m_y;
  };

  TransversePoint operator+(const TransversePoint& lhs, const TransversePoint& rhs);
  TransversePoint operator-(const TransversePoint& lhs, const TransversePoint& rhs);
  TransversePoint operator*(double, const TransversePoint& lhs);
  std::ostream& operator<<(std::ostream&, const TransversePoint&);
}
#endif
