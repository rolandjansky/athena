/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldMesh.h
//
// Generic 3-d mesh representing a simple field map.
// The field type is templated - it may be short (for the toroid) or double (for
// the solenoid)
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDMESH_H
#define BFIELDMESH_H

#include "MagFieldElements/BFieldCache.h"
#include "MagFieldElements/BFieldVector.h"
#include <cmath>
#include <iostream>
#include <vector>

template<class T>
class BFieldMesh
{
public:
  // constructor
  BFieldMesh()
    : m_scale(1.0)
  {
    ;
  }
  BFieldMesh(double zmin,
             double zmax,
             double rmin,
             double rmax,
             double phimin,
             double phimax,
             double bscale)
    : m_scale(bscale)
    , m_nomScale(bscale)
  {
    m_min[0] = zmin;
    m_max[0] = zmax;
    m_min[1] = rmin;
    m_max[1] = rmax;
    m_min[2] = phimin;
    m_max[2] = phimax;
  }
  // set ranges
  void setRange(double zmin,
                double zmax,
                double rmin,
                double rmax,
                double phimin,
                double phimax)
  {
    m_min[0] = zmin;
    m_max[0] = zmax;
    m_min[1] = rmin;
    m_max[1] = rmax;
    m_min[2] = phimin;
    m_max[2] = phimax;
  }
  // set bscale
  void setBscale(double bscale) { m_scale = m_nomScale = bscale; }
  // scale bscale by a factor
  void scaleBscale(double factor) { m_scale = factor * m_nomScale; }
  // allocate space to vectors
  void reserve(int nz, int nr, int nphi, int nfield);
  void reserve(int nz, int nr, int nphi)
  {
    reserve(nz, nr, nphi, nz * nr * nphi);
  }
  // add elements to vectors
  void appendMesh(int i, double mesh) { m_mesh[i].push_back(mesh); }
  void appendField(const BFieldVector<T>& field) { m_field.push_back(field); }
  // build LUT
  void buildLUT();
  // test if a point is inside this zone
  bool inside(double z, double r, double phi) const;
  // find the bin
  void getCache(double z,
                double r,
                double phi,
                BFieldCache& cache,
                double scaleFactor = 1.0) const;
  // get the B field
  void getB(const double* xyz, double* B, double* deriv = nullptr) const;
  // accessors
  double min(int i) const { return m_min[i]; }
  double max(int i) const { return m_max[i]; }
  double zmin() const { return m_min[0]; }
  double zmax() const { return m_max[0]; }
  double rmin() const { return m_min[1]; }
  double rmax() const { return m_max[1]; }
  double phimin() const { return m_min[2]; }
  double phimax() const { return m_max[2]; }
  unsigned nmesh(int i) const { return m_mesh[i].size(); }
  double mesh(int i, int j) const { return m_mesh[i][j]; }
  unsigned nfield() const { return m_field.size(); }
  const BFieldVector<T>& field(int i) const { return m_field[i]; }
  double bscale() const { return m_scale; }
  int memSize() const;

protected:
  double m_min[3], m_max[3];
  std::vector<double> m_mesh[3];

private:
  std::vector<BFieldVector<T>> m_field;
  double m_scale;
  double m_nomScale; // nominal m_scale from the map
  // look-up table and related variables
  std::vector<int> m_LUT[3];
  double m_invUnit[3]; // inverse unit size in the LUT
  int m_roff, m_zoff;
};
#include "BFieldMesh.icc"
#endif
