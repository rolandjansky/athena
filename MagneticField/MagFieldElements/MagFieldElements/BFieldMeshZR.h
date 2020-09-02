/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldMeshZR.h
//
// A 2-dim z-r mesh inside the solenoid field map
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDMESHZR_H
#define BFIELDMESHZR_H

#include "MagFieldElements/BFieldCacheZR.h"
#include "MagFieldElements/BFieldVectorZR.h"
#include <cmath>
#include <vector>
#include <array>

class BFieldMeshZR
{
public:
  // constructor
  BFieldMeshZR(double zmin, double zmax, double rmin, double rmax)
  {
    m_min = { zmin, rmin };
    m_max = { zmax, rmax };
  }
  // allocate space to vectors
  void reserve(int nz, int nr)
  {
    m_mesh[0].reserve(nz);
    m_mesh[1].reserve(nr);
    m_field.reserve(nz * nr);
  }
  // add elements to vectors
  void appendMesh(int i, double mesh) { m_mesh[i].push_back(mesh); }
  void appendField(const BFieldVectorZR& field) { m_field.push_back(field); }
  // build LUT
  void buildLUT();
  // test if a point is inside this zone
  inline bool inside(double z, double r) const
  {
    return (z >= zmin() && z <= zmax() && r >= rmin() && r <= rmax());
  }
  // find the bin
  inline void getCache(double z,
                       double r,
                       BFieldCacheZR& cache,
                       double scaleFactor = 1.0) const;
  // accessors
  double min(size_t i) const { return m_min[i]; }
  double max(size_t i) const { return m_max[i]; }
  double zmin() const { return m_min[0]; }
  double zmax() const { return m_max[0]; }
  double rmin() const { return m_min[1]; }
  double rmax() const { return m_max[1]; }
  unsigned nmesh(size_t i) const { return m_mesh[i].size(); }
  double mesh(size_t i, size_t j) const { return m_mesh[i][j]; }
  unsigned nfield() const { return m_field.size(); }
  const BFieldVectorZR& field(size_t i) const { return m_field[i]; }
  int memSize() const;

private:
  std::array<double,2> m_min;
  std::array<double,2> m_max;
  std::array<std::vector<double>,2> m_mesh;
  std::vector<BFieldVectorZR> m_field;
  // look-up table and related variables
  std::array<std::vector<int>,2> m_LUT;
  std::array<double, 2> m_invUnit; // inverse unit size in the LUT
  int m_zoff;
};

#include "MagFieldElements/BFieldMeshZR.icc"
#endif
