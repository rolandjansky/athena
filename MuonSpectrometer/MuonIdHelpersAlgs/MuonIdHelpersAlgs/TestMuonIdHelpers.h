/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestMuonIdHelpers_H
#define TestMuonIdHelpers_H

#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"

class StoreGateSvc;
class ActiveStoreSvc;
class MsgStream;
class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;

namespace MuonGM {
class MuonDetectorManager;
}

/////////////////////////////////////////////////////////////////////////////

class TestMuonIdHelpers : public AthAlgorithm {

public:

  TestMuonIdHelpers (const std::string& name, ISvcLocator* pSvcLocator);
  ~TestMuonIdHelpers();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  StatusCode testMdtIdHelper();
  StatusCode testCscIdHelper();
  StatusCode testRpcIdHelper();
  StatusCode testTgcIdHelper();

  int testMDTIds(const Identifier& id);
  int testCSCIds(const Identifier& id);
  int testRPCIds(const Identifier& id);
  int testTGCIds(const Identifier& id);

  BooleanProperty m_testMDT;
  BooleanProperty m_testCSC;
  BooleanProperty m_testRPC;
  BooleanProperty m_testTGC;

  ServiceHandle<ActiveStoreSvc> m_activeStore;
  const MdtIdHelper          * m_mdtId;
  const CscIdHelper          * m_cscId;
  const RpcIdHelper          * m_rpcId;
  const TgcIdHelper          * m_tgcId;
  const MuonGM::MuonDetectorManager  * m_muon_mgr;

  longlong m_deltaUser;
  longlong m_deltaKernel;
  longlong m_deltaElapsed;
  int m_nTries;

};

#endif



