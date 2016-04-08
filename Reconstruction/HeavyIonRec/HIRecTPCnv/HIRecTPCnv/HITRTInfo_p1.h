/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIRECTPCNV_HITRTINFO_P1_H
#define HIRECTPCNV_HITRTINFO_P1_H

#include <vector>

// forward declarations
class HITRTInfoCnv_p1;

class HITRTInfo_p1
{
  friend class HITRTInfoCnv_p1;
  
 public:
  
  /**  default constructor */
  HITRTInfo_p1() {};
  
  /** destructor */
  ~HITRTInfo_p1() {};
  
  // protected:
  
  std::vector<int> infos;
};
#endif
