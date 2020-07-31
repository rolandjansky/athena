/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ComparisonFunction, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTUTILS_COMPARISONFUNCTION_H
#define TRKEVENTUTILS_COMPARISONFUNCTION_H

#include "GeoPrimitives/GeoPrimitives.h"
#include <cmath>
#include <cstdint>

namespace Trk {

/**
  @class ComparisonFunction

  this functor is used to find the closest objects:

  - a point
  - a line
  - a cylinder of radius R

  @author Andreas.Salzburger@cern.ch
  @author Christos Anastopoulos (MT cleanup)

  */

template<class T>
class ComparisonFunction
{
public:
  /** Default */
  ComparisonFunction() = default;
  ComparisonFunction(const ComparisonFunction& cpf) = default;
  ComparisonFunction(ComparisonFunction&& cpf) = default;
  ComparisonFunction& operator=(const ComparisonFunction& cpf) = default;
  ComparisonFunction& operator=(ComparisonFunction&& cpf) = default;
  ~ComparisonFunction() = default;

  /** Constructor for pointlike Search */
  ComparisonFunction(const Amg::Vector3D& sp)
    : m_point(sp)
    , m_line{}
    , m_radius{ 0 }
    , m_hasPointAndLine{ 1 }
  {
    m_line.setZero();
  }

  /** Constructor with point and line, search with direction */
  ComparisonFunction(const Amg::Vector3D& sp, const Amg::Vector3D& dir)
    : m_point(sp)
    , m_line(dir)
    , m_radius{ 0. }
    , m_hasPointAndLine{ 2 }
  {}

  /** Search wrt to distance wrt to radius*/
  ComparisonFunction(double cradius)
    : m_point{}
    , m_line{}
    , m_radius{ std::abs(cradius) }
    , m_hasPointAndLine{ 0 }
  {
    m_point.setZero();
    m_line.setZero();
  }

  bool operator()(const T* one, const T* two) const
  {
    if (m_hasPointAndLine == 0) {
      return (fabs(one->position().perp() - m_radius) <
              fabs(two->position().perp() - m_radius));
    }
    if (m_hasPointAndLine == 1) {
      return ((one->position() - m_point).mag() <
              (two->position() - m_point).mag());
    }
    Amg::Vector3D distPosOne((one->position()) - m_point);
    double lmag2 = m_line.mag();
    lmag2 *= lmag2;
    double dp1_mag2 = distPosOne.mag();
    dp1_mag2 *= dp1_mag2;
    double pl1_2 = m_line.dot(distPosOne);
    pl1_2 *= pl1_2;
    double distOne = (lmag2 * dp1_mag2 - pl1_2) / dp1_mag2;

    Amg::Vector3D distPosTwo((two->position()) - m_point);
    double dp2_mag2 = distPosTwo.mag();
    dp1_mag2 *= dp1_mag2;
    double pl2_2 = m_line.dot(distPosTwo);
    pl1_2 *= pl1_2;
    double distTwo = (lmag2 * dp2_mag2 - pl2_2) / dp2_mag2;

    return (fabs(distOne) < fabs(distTwo));
  }

private:
  Amg::Vector3D m_point;
  Amg::Vector3D m_line;
  double m_radius = 0.;
  // 0 no line no point, 1 only point , 2 has both
  int8_t m_hasPointAndLine = 0;
};

} // end of namespace

#endif

