/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldCacheZR.h
//
// Cashe of one bin of the magnetic field map.
// Defined by ranges in z, r, and the (Bz, Br) vectors at the 4 corners of the
// "bin".
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDCACHEZR_H
#define BFIELDCACHEZR_H

#include "MagFieldElements/BFieldVectorZR.h"
#include "CxxUtils/restrict.h"
#include <cmath>

class BFieldCacheZR
{
public:
  // default constructor sets unphysical boundaries, so that inside() will fail
  BFieldCacheZR()
    : m_zmin(0.0)
    , m_zmax(-1.0)
    , m_rmin(0.0)
    , m_rmax(-1.0)
  {}
  // invalidate this cache, so that inside() will fail
  void invalidate()
  {
    m_rmin = 0.0;
    m_rmax = -1.0;
  }
  // set the z, r range that defines the bin
  void setRange(double zmin, double zmax, double rmin, double rmax)
  {
    m_zmin = zmin;
    m_zmax = zmax;
    m_rmin = rmin;
    m_rmax = rmax;
    m_invz = 1.0 / (zmax - zmin);
    m_invr = 1.0 / (rmax - rmin);
  }
  // set the field values at each corner (rescale for current scale factor)
  void setField(int i, const BFieldVectorZR& field, double scaleFactor = 1.0)
  {
    m_field[0][i] = scaleFactor * field[0];
    m_field[1][i] = scaleFactor * field[1];
  }
  // set the multiplicative factor for the field vectors
  // test if (z, r) is inside this bin
  bool inside(double z, double r) const
  {
    return (z >= m_zmin && z <= m_zmax && r >= m_rmin && r <= m_rmax);
  }
  // interpolate the field and return B[3].
  // also compute field derivatives if deriv[9] is given.
  void getB(const double*  ATH_RESTRICT xyz,
            double r,
            double* ATH_RESTRICT B,
            double* ATH_RESTRICT deriv = nullptr) const;

private:
  double m_zmin, m_zmax; // bin range in z
  double m_rmin, m_rmax; // bin range in r
  double m_invz, m_invr;  // 1/(bin size) in z, r
  double m_field[2][4];   // (Bz,Br) at 4 corners of the bin
};
#include "MagFieldElements/BFieldCacheZR.icc"
#endif
