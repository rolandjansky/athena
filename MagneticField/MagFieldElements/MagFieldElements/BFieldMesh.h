/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * BFieldMesh.h
 * Generic 3-d mesh representing a simple field map.
 * The field type is templated - it may be short (for the toroid) or double (for
 * the solenoid)
 *
 * Masahiro Morii, Harvard University
 *
 * AthenaMT : RD Schaffer , Christos Anastopoulos
 */
//
#ifndef BFIELDMESH_H
#define BFIELDMESH_H

#include "MagFieldElements/BFieldCache.h"
#include "MagFieldElements/BFieldVector.h"
#include <array>
#include <cmath>
#include <vector>

template<class T>
class BFieldMesh
{
public:
  BFieldMesh() = default;
  BFieldMesh(const BFieldMesh&) = default;
  BFieldMesh(BFieldMesh&&) = default;
  BFieldMesh& operator=(const BFieldMesh&) = default;
  BFieldMesh& operator=(BFieldMesh&&) = default;
  ~BFieldMesh() = default;
  // constructor
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
    m_min = { zmin, rmin, phimin };
    m_max = { zmax, rmax, phimax };
  }
  // set ranges
  void setRange(double zmin,
                double zmax,
                double rmin,
                double rmax,
                double phimin,
                double phimax)
  {
    m_min = { zmin, rmin, phimin };
    m_max = { zmax, rmax, phimax };
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
  // build Look Up Table
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
  void getB(const double* ATH_RESTRICT xyz,
            double* ATH_RESTRICT B,
            double* ATH_RESTRICT deriv = nullptr) const;
  // accessors
  double min(size_t i) const { return m_min[i]; }
  double max(size_t i) const { return m_max[i]; }
  double zmin() const { return m_min[0]; }
  double zmax() const { return m_max[0]; }
  double rmin() const { return m_min[1]; }
  double rmax() const { return m_max[1]; }
  double phimin() const { return m_min[2]; }
  double phimax() const { return m_max[2]; }
  unsigned nmesh(size_t i) const { return m_mesh[i].size(); }
  double mesh(size_t i, size_t j) const { return m_mesh[i][j]; }
  unsigned nfield() const { return m_field.size(); }
  const BFieldVector<T>& field(size_t i) const { return m_field[i]; }
  double bscale() const { return m_scale; }
  int memSize() const;

protected:
  std::array<double, 3> m_min;
  std::array<double, 3> m_max;
  std::array<std::vector<double>,3> m_mesh;

private:
  std::vector<BFieldVector<T>> m_field;
  double m_scale = 1.0;
  double m_nomScale; // nominal m_scale from the map

  // look-up table and related variables
  std::array<std::vector<int>,3> m_LUT;
  std::array<double,3> m_invUnit; // inverse unit size in the LUT
  int m_roff, m_zoff;

};
#include "MagFieldElements/BFieldMesh.icc"
#endif
