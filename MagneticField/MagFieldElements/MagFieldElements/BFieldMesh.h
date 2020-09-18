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
             double bscale);
  // set ranges
  void setRange(double zmin,
                double zmax,
                double rmin,
                double rmax,
                double phimin,
                double phimax);
  // set bscale
  void setBscale(double bscale);
  // scale bscale by a factor
  void scaleBscale(double factor);
  // allocate space to vectors
  void reserve(int nz, int nr, int nphi, int nfield);
  void reserve(int nz, int nr, int nphi);
  // add elements to vectors
  void appendMesh(int i, double mesh);
  void appendField(const BFieldVector<T>& field);
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
  double min(size_t i) const;
  double max(size_t i) const;
  double zmin() const;
  double zmax() const;
  double rmin() const;
  double rmax() const;
  double phimin() const;
  double phimax() const;
  unsigned nmesh(size_t i) const;
  double mesh(size_t i, size_t j) const;
  unsigned nfield() const;
  const BFieldVector<T>& field(size_t i) const;
  double bscale() const;
  int memSize() const;

protected:
  std::array<double, 3> m_min;
  std::array<double, 3> m_max;
  std::array<std::vector<double>, 3> m_mesh;

private:
  std::vector<BFieldVector<T>> m_field;
  double m_scale = 1.0;
  double m_nomScale; // nominal m_scale from the map

  // look-up table and related variables
  std::array<std::vector<int>, 3> m_LUT;
  std::array<double, 3> m_invUnit; // inverse unit size in the LUT
  int m_roff, m_zoff;
};
#include "MagFieldElements/BFieldMesh.icc"
#endif
