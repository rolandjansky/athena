/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldVectorZR.h
//
// Magnetic field value (Bz,Br) stored in 2d map.
//
// Masahiro Morii, Harvard University
//
// Athena MT RD Schaffer , C Anastopoulos
//
#ifndef BFIELDVECTORZR_H
#define BFIELDVECTORZR_H
#include <array>
class BFieldVectorZR
{
public:
  // default
  BFieldVectorZR() = default;
  BFieldVectorZR(const BFieldVectorZR&) = default;
  BFieldVectorZR(BFieldVectorZR&&) = default;
  BFieldVectorZR& operator=(const BFieldVectorZR&) = default;
  BFieldVectorZR& operator=(BFieldVectorZR&&) = default;
  ~BFieldVectorZR() = default;

  //constructor
  BFieldVectorZR(float Bz, float Br)
    : m_B{ Bz, Br }
  {}
  // setter
  void set(float Bz, float Br) { m_B = { Bz, Br }; }
  // accessors
  float z() const { return m_B[0]; }
  float r() const { return m_B[1]; }
  // array-like accessor
  float operator[](size_t i) const { return m_B[i]; }

private:
  std::array<float, 2> m_B;
};

#endif
