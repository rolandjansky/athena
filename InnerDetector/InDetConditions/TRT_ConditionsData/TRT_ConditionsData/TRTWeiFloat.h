/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSDATA_TRTWEIFLOAT_H
#define TRTCONDITIONSDATA_TRTWEIFLOAT_H
// TRTWeiFloat.h - class to hold a pointer to the TRT resolution
// and high-threshold weight associated with a TRT_ID key..
// The constants are of type float:
// res        Space resolution in mm of drift time hit
// htw        The weight associated with a high threshold hit
// In both cases, a zero value means "not recommended for use".
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

class TRTWeiFloat: public DetCondFloat {
 public:
  TRTWeiFloat();
  TRTWeiFloat(const std::string tag);
  
  void print() const;
  void print2() const;
  int addValue(const Identifier& ident, float res, float htw);
  int setValue(const Identifier& ident, float res, float htw);
  int getValue(const Identifier& ident, float& res, float& htw) const;
  Identifier getValue(unsigned int elem, float& res, float& htw) const;
  size_t getMapSize() const;

 private:
  static const int m_TRTWei_size=2;
};

CLASS_DEF(TRTWeiFloat,174979412,1)

// inline functions for TRTWeiFloat

inline TRTWeiFloat::TRTWeiFloat() : 
DetCondFloat(m_TRTWei_size, "null") {}


inline TRTWeiFloat::TRTWeiFloat(const std::string tag) : 
DetCondFloat(m_TRTWei_size, tag) {}

inline size_t TRTWeiFloat::getMapSize() const
{return m_bufmap.size();} 

#endif // TRTCONDITIONSDATA_TRTWEIFLOAT_H
