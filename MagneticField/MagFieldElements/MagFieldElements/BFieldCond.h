/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldCond.h
//
// Current conductors used in BFieldZone.
// It may be finite : p1 to p2.
// It may be infinite : passing p1 and direction parallel to p2.
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDCOND_H
#define BFIELDCOND_H

#include "EventPrimitives/EventPrimitives.h"
#include <cmath>

class BFieldCond
{
public:
  // constructor
  BFieldCond(bool finite, const double* p1, const double* p2, double curr)
    : m_finite(finite)
    , m_p1(Eigen::Map<const Eigen::Vector3d>(p1))
    , m_p2(Eigen::Map<const Eigen::Vector3d>(p2))
    , m_u(finite ? (m_p2 - m_p1).normalized() : m_p2)
    , m_curr(curr)
    , m_nomCurr(curr)
  {}

  // compute magnetic field, plus derivatives if requested, and add
  void addBiotSavart(double scaleFactor,
                     const double* xyz,
                     double* B,
                     double* deriv = nullptr) const;
  // scale the current wrt the nominal current
  void scaleCurrent(double factor) { m_curr = factor * m_nomCurr; }
  // accessors
  bool finite() const { return m_finite; }
  double p1(int i) const { return m_p1[i]; }
  double p2(int i) const { return m_p2[i]; }
  double curr() const { return m_curr; }

private:
  bool m_finite; // true if the conductor is finite in length

  /*
   * m_p1: One end of a finite conductor, or one point on an infinite conductor
   * m_p2: The other end of a finite conductor, or the direction vector of an
   * infinite conductor m_u : Direction vector (= m_p2 if infinite)
   */
  const Eigen::Vector3d m_p1, m_p2, m_u;
  double m_curr;    // current (in A) flowing through the conductor
  double m_nomCurr; // nominal current (in A) read from the map file
};
#include "BFieldCond.icc"
#endif
