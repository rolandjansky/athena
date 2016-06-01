// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PyDumper/PyDumperDict.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Generate dictionaries for adaptors needed to access event data.
 */


#include "PyDumper/PySTLAdaptor.h"
#include "AthContainers/AuxElement.h"
#include <set>
#include <cmath>


namespace PyDumper {

// Hack to return a float nan to python as a double nan without causing a FPE.
// Not doing anything will trap in pyroot.
// isnan() can also trap, so don't use that!
class SafeFloatAccess
{
public:
  union ieee754_float
  {
    float f;

    /* This is the IEEE 754 single-precision format.  */
    struct
      {
#if     __BYTE_ORDER == __BIG_ENDIAN
        unsigned int negative:1;
        unsigned int exponent:8;
        unsigned int mantissa:23;
#endif                          /* Big endian.  */
#if     __BYTE_ORDER == __LITTLE_ENDIAN
        unsigned int mantissa:23;
        unsigned int exponent:8;
        unsigned int negative:1;
#endif                          /* Little endian.  */
      } ieee;
  };
  
  SafeFloatAccess (const std::vector<float>& v) : m_v(v) {}
  double operator[] (size_t i) {
    ieee754_float ff;
    ff.f = m_v[i];
    if (ff.ieee.exponent == 0xff)
      return std::nan("");
    return m_v[i];
  }
private:
  const std::vector<float>& m_v;
};


class Utils
{
public:
  static
  std::vector<unsigned int> getAuxIDVector (const SG::AuxVectorData& e)
  {
    const SG::auxid_set_t& ids = e.getAuxIDs();
    std::vector<unsigned int> v (ids.begin(), ids.end());
    std::sort (v.begin(), v.end());
    return v;
  }
  static
  std::vector<unsigned int> getAuxIDVector (const SG::AuxElement& e)
  {
    const SG::auxid_set_t& ids = e.getAuxIDs();
    std::vector<unsigned int> v (ids.begin(), ids.end());
    std::sort (v.begin(), v.end());
    return v;
  }
};


}



struct PyDumperDict {
  PyDumperDict();
  PyDumper::PySTLAdaptor<std::set<unsigned int> > s;
  PyDumper::PySTLAdaptor<std::set<unsigned int> >::iterator s_i;
};
