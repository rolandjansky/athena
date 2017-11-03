/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSDATA_TRTALIFLOAT_H
#define TRTCONDITIONSDATA_TRTALIFLOAT_H
// TRTWeiFloat.h - class to hold a pointer to the z/r dependent
// fine corrections to the Rphi position of TRT wires:
//  Delta(Rphi)= d0 + d1*(y-y0) + d2*(y-y0)*(y-y0) [mm]
// where y is z(barrel)/r(endcap) and y0 is the middle of the wire 
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

class TRTAliFloat: public DetCondFloat {
 public:
  TRTAliFloat();
  TRTAliFloat(const std::string tag);
  
  void print() const;
  void print2() const;
  int addValue(const Identifier& ident, float d0, 
                     float d1, float d2);
  int setValue(const Identifier& ident, float d0, 
                     float d1, float d2);
  int getValue(const Identifier& ident, float& d0, float& d1, float& d2) const;
  Identifier getValue(unsigned int elem, float& d0, float& d1, float& d2) const;
  size_t getMapSize() const;

 private:
  static const int m_TRTAli_size=3;
};

CLASS_DEF(TRTAliFloat,112414883,1)

// inline functions for TRTAliFloat

inline TRTAliFloat::TRTAliFloat() : 
DetCondFloat(m_TRTAli_size, "null") {}

inline TRTAliFloat::TRTAliFloat(const std::string tag) : 
DetCondFloat(m_TRTAli_size, tag) {}

inline size_t TRTAliFloat::getMapSize() const
{return m_bufmap.size();} 

#endif // TRTCONDITIONSDATA_TRTALIFLOAT_H
