/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDCOOL_RPCREADWRITECOOLSTR_H
#define MUONCONDCOOL_RPCREADWRITECOOLSTR_H
// ReadWriteCoolStrFile - simple algorithm to demonstrate reading/writing
// of string objects stored in COOL via CoolStrFileSvc


#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"

class RpcIdHelper;
class StoreGateSvc;

namespace MuonCalib {
class RpcICoolStrSvc;

class RpcReadWriteCoolStr: public AthAlgorithm
{
 public:
  RpcReadWriteCoolStr(const std::string& name, ISvcLocator* pSvcLocator);
  ~RpcReadWriteCoolStr(void);
  StatusCode initialize(void);
  StatusCode execute(void);
  StatusCode finalize(void);

 private:
  MsgStream m_log;
  StoreGateSvc* p_detstore;
  RpcICoolStrSvc* p_coolsvc;

  bool m_readDone;
  bool m_writeDone;
  bool m_read;
  bool m_write;
  bool m_onlineWrite;
  bool m_onlineRead;
  std::string m_fileName;

};
}
#endif // MUONCONDCOOL_RPCREADWRITECOOLSTR_H
