/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENADBTESTREC_DETSTOREDUMP_H
#define ATHENADBTESTREC_DETSTOREDUMP_H

#include <string>
#include <vector>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"


class DetStoreDump : public Algorithm
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

  MsgStream m_log;
  StoreGateSvc* p_detstore;
  bool m_first;
};

#endif // ATHENADBTESTREC_DETSTOREDUMP_H
