/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_RECOTIMINGOBJ_P1_H
#define RECTPCNV_RECOTIMINGOBJ_P1_H

#include <vector>

// forward declarations
class RecoTimingObjCnv_p1;

class RecoTimingObj_p1
{
  friend class RecoTimingObjCnv_p1;
  
 public:
  
  /**  default constructor */
   RecoTimingObj_p1() : m_isTimings(true) {};
  
  /** destructor */
  ~RecoTimingObj_p1() = default;
  
  // protected:
  
  std::vector<float> timings;
  
  bool m_isTimings;
};
#endif
