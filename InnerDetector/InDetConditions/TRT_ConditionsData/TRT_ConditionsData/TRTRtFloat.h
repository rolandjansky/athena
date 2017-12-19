/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSDATA_TRTRTFLOAT_H
#define TRTCONDITIONSDATA_TRTRTFLOAT_H
// TRTRtFloat.h - class to hold a pointer to the TRT space-time calibration
// constants associated with a TRT_ID key..
// The constants are of type float:
// T0        Tzero in units of ns
// vd        The drift velocity (0.2mm to 1.6mm from the wire) in mm/ns
// vp        The propagation velocity in units of the speed of light
// a0-a3     Coefficients of a third deg pol R(T) in mm with T in ns.
// 
// This class inherits from DetDescrCond/DetCondFloat.
// Peter Hansen, started 1/5/04
//
#include <vector>
#include <map>
#include <algorithm>
#include "DetDescrConditions/DetCondFloat.h"
#include "AthenaKernel/CLASS_DEF.h"

class Identifier;

class TRTRtFloat: public DetCondFloat {
 public:
  TRTRtFloat();
  TRTRtFloat(const std::string tag);
  
  void print() const;
  void print2() const;
  int addValue(const Identifier& ident, float T0, float vd, float vp,
                                        float a0, float a1, float a2,
                                        float a3);
  int setValue(const Identifier& ident, float T0, float vd, float vp,
                                        float a0, float a1, float a2,
                                        float a3);
  int getValue(const Identifier& ident, float& T0, float& vd, float& vp,
                                        float& a0, float& a1, float& a2,
                                        float& a3) const;
  Identifier getValue(unsigned int elem, float& T0, float& vd, float& vp,
                                        float& a0, float& a1, float& a2,
                                        float& a3) const;
  size_t getMapSize() const;

 private:
  static const int m_TRTRt_size=7;
};

CLASS_DEF(TRTRtFloat,56983368,1)

// inline functions for TRTRtFloat

inline TRTRtFloat::TRTRtFloat() : 
DetCondFloat(m_TRTRt_size, "null") {}


inline TRTRtFloat::TRTRtFloat(const std::string tag) : 
DetCondFloat(m_TRTRt_size, tag) {}

inline size_t TRTRtFloat::getMapSize() const
{return m_bufmap.size();} 

#endif // TRTCONDITIONSDATA_TRTRTFLOAT_H
