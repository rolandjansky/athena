/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTOOLS_LARHVPATHOLOGIESDB_H
#define LARTOOLS_LARHVPATHOLOGIESDB_H

#include <vector>

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
