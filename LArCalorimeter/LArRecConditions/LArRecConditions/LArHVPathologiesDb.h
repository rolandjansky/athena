/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTOOLS_LARHVPATHOLOGIESDB_H
#define LARTOOLS_LARHVPATHOLOGIESDB_H

#include "CxxUtils/bitmask.h"
#include <vector>

class LArHVPathologyBits {
   public:
     static const unsigned short MaskHV=0x1;
     static const unsigned short MaskCurr=0x2;
     static const unsigned short SetHVMask=0xFFF0;
};

class LArHVPathologiesDb
{
 public:

  struct LArHVElectPathologyDb {
    unsigned int cellID;
    unsigned short electInd;
    unsigned short pathologyType;
  };

  LArHVPathologiesDb() {}
  virtual ~LArHVPathologiesDb() {}

  std::vector<LArHVElectPathologyDb> m_v;
};


#endif
