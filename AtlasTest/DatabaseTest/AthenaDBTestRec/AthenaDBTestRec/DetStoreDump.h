/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENADBTESTREC_DETSTOREDUMP_H
#define ATHENADBTESTREC_DETSTOREDUMP_H

#include <string>
#include <vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"


class DetStoreDump : public AthAlgorithm
{
 public:
  DetStoreDump(const std::string& name, ISvcLocator* pSvcLocator);
  ~DetStoreDump();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  // parameters
  int m_mode;

  bool m_first;
};

#endif // ATHENADBTESTREC_DETSTOREDUMP_H
