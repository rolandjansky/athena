/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECEXALGS_RECOTIMINGOBJ_H
#define RECEXALGS_RECOTIMINGOBJ_H

#include "SGTools/CLASS_DEF.h"
#include "SGTools/BuiltinsClids.h"
#include <vector>

class RecoTimingObj : public std::vector<float>
{
 public:
  
  friend class RecoTimingObjCnv_p1;
  
  RecoTimingObj(bool isTimings=true) : m_readFromFile(false), m_isTimings(isTimings), m_usetcmalloc(true) { };
    
 private:
  
  bool m_readFromFile;

  bool m_isTimings;

  bool m_usetcmalloc;
};

// clid for float is now defined by SGTools
CLASS_DEF(RecoTimingObj,76321643,1)

#endif
