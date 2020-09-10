/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldZone.h
//
// A "zone" inside the toroid field map
//
// Masahiro Morii, Harvard University
//
//
#ifndef BFIELDZONE_H
#define BFIELDZONE_H

#include "CxxUtils/restrict.h"
#include "MagFieldElements/BFieldCond.h"
#include "MagFieldElements/BFieldMesh.h"
#include <vector>

class BFieldZone : public BFieldMesh<short>
{
public:
  // constructor
  BFieldZone(int id,
             double zmin,
             double zmax,
             double rmin,
             double rmax,
             double phimin,
             double phimax,
             double scale);
  // add elements to vectors
  void appendCond(const BFieldCond& cond);
  // compute Biot-Savart magnetic field and add to B[3]
  void addBiotSavart(const double* ATH_RESTRICT xyz,
                     double* ATH_RESTRICT B,
                     double* ATH_RESTRICT deriv = nullptr) const;
  // scale B field by a multiplicative factor: RDS 2019/09 - no longer used.
  // Scaling is done in cachec
  void scaleField(double factor);
  // accessors
  int id() const;
  unsigned ncond() const;
  const BFieldCond& cond(int i) const;
  const std::vector<BFieldCond>* condVector() const;
  int memSize() const;
  // adjust the min/max edges to a new value
  void adjustMin(int i, double x);
  void adjustMax(int i, double x);

private:
  int m_id;                       // zone ID number
  std::vector<BFieldCond> m_cond; // list of current conductors
};

#include "BFieldZone.icc"

#endif
