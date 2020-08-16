/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldCache.h
//
// Cashe of one bin of the magnetic field map.
// Defined by ranges in z, r, phi, and the B vectors at the 8 corners of the
// "bin".
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDCACHE_H
#define BFIELDCACHE_H

#include "CxxUtils/restrict.h"
#include "MagFieldElements/BFieldVector.h"
#include <cmath>

class BFieldCache
{
public:
  // default constructor sets unphysical boundaries, so that inside() will fail
  BFieldCache()
    : m_zmin(0.0)
    , m_zmax(0.0)
    , m_rmin(0.0)
    , m_rmax(0.0)
    , m_phimin(0.0)
    , m_phimax(-1.0)
  {
  }
  // make this cache invalid, so that inside() will fail
  void invalidate()
  {
    m_phimin = 0.0;
    m_phimax = -1.0;
  }
  // set the z, r, phi range that defines the bin
  void setRange(double zmin,
                double zmax,
                double rmin,
                double rmax,
                double phimin,
                double phimax)
  {
    m_zmin = zmin;
    m_zmax = zmax;
    m_rmin = rmin;
    m_rmax = rmax;
    m_phimin = phimin;
    m_phimax = phimax;
    m_invz = 1.0 / (zmax - zmin);
    m_invr = 1.0 / (rmax - rmin);
    m_invphi = 1.0 / (phimax - phimin);
  }

    // **
    // ** The following two methods are commented out, and replaced by the third setField as optimization test
    // **
  // // set the field values at each corner (rescale for current scale factor)
  // void setField(const std::array<BFieldVector<double>, 8>& field,
  //               double scaleFactor = 1.0)
  // {
  //   // We pass array of 8 elements with 3 entries
  //   // Which go to 3x8 matrix
  //   for (int i = 0; i < 8; ++i) {
  //     for (int j = 0; j < 3; ++j) {
  //       m_field[j][i] = scaleFactor * field[i][j];
  //     }
  //   }
  // }

  // void setField(const std::array<BFieldVector<short>, 8>& field,
  //               double scaleFactor = 1.0)
  // {
  //   // We pass array of 8 elements with 3 entries
  //   // Which go to 3x8 matrix
  //   for (int i = 0; i < 8; ++i) {
  //     for (int j = 0; j < 3; ++j) {
  //       m_field[j][i] = scaleFactor * field[i][j];
  //     }
  //   }
  // }

  // set field array, filled externally
  void setField(double field[][8]) {
      std::copy( &field[0][0], &field[0][0] + 3*8, &m_field[0][0]);
  }
    

  // set the field values at each corner (rescale for current scale factor)
  void printField() 
  {
    // print out field values
    std::cout << "Field at corner i, for each component j (Bz, Br, Bphi)" << std::endl;

    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 3; ++j) {
          std::cout << i << "," << j << ": " << m_field[j][i] << ", ";
      }
      std::cout << std::endl;
    }
  }

  // set the multiplicative factor for the field vectors
  void setBscale(double bscale) { m_scale = bscale; }
  float bscale() { return m_scale; }

  // test if (z, r, phi) is inside this bin
  bool inside(double z, double r, double phi) const
  {
    if (phi < m_phimin) {
      phi += 2.0 * M_PI;
    }
    return (phi >= m_phimin && phi <= m_phimax && z >= m_zmin && z <= m_zmax &&
            r >= m_rmin && r <= m_rmax);
  }
  // interpolate the field and return B[3].
  // also compute field derivatives if deriv[9] is given.
  void getB(const double* ATH_RESTRICT xyz,
            double r,
            double phi,
            double* ATH_RESTRICT B,
            double* ATH_RESTRICT deriv = nullptr) const;

private:
  double m_zmin, m_zmax;           // bin range in z
  double m_rmin, m_rmax;           // bin range in r
  double m_phimin, m_phimax;       // bin range in phi
  double m_invz, m_invr, m_invphi; // 1/(bin size) in z, r, phi
  double m_field[3][8];            // (Bz,Br,Bphi) at 8 corners of the bin
  double m_scale;                  // unit of m_field in kT
};

#include "MagFieldElements/BFieldCache.icc"
#endif
