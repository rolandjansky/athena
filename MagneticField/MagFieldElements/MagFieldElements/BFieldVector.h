/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldVector.h
//
// Magnetic field value stored in the map.
// It may be either short (toroid) or double (solenoid).
//
// Masahiro Morii, Harvard University
//
// Athena MT RD Schaffer , C Anastopoulos
//
#ifndef BFIELDVECTOR_H
#define BFIELDVECTOR_H

#include <array>

template<class T>
class BFieldVector
{
public:
  // Default
  BFieldVector() = default;
  BFieldVector(const BFieldVector&) = default;
  BFieldVector(BFieldVector&&) = default;
  BFieldVector& operator=(const BFieldVector&) = default;
  BFieldVector& operator=(BFieldVector&&) = default;
  ~BFieldVector() = default;

  BFieldVector(T Bz, T Br, T Bphi)
    : m_B{ Bz, Br, Bphi }
  {}
  // setter
  void set(T Bz, T Br, T Bphi) { m_B = { Bz, Br, Bphi }; }

  // accessors
  T z() const { return m_B[0]; }
  T r() const { return m_B[1]; }
  T phi() const { return m_B[2]; }
  // array-like accessor
  T operator[](size_t i) const { return m_B[i]; }

private:
  std::array<T, 3> m_B;
};

#endif
