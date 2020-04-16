/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestMuonIdHelpers_H
#define TestMuonIdHelpers_H

#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class StoreGateSvc;
class ActiveStoreSvc;

/////////////////////////////////////////////////////////////////////////////

class TestMuonIdHelpers : public AthAlgorithm {

public:

  TestMuonIdHelpers (const std::string& name, ISvcLocator* pSvcLocator);
  ~TestMuonIdHelpers()=default;

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  StatusCode testMdtIdHelper() const;
  StatusCode testCscIdHelper() const;
  StatusCode testRpcIdHelper() const;
  StatusCode testTgcIdHelper() const;

  int testMDTIds(const Identifier& id) const;
  int testCSCIds(const Identifier& id) const;
  int testRPCIds(const Identifier& id) const;
  int testTGCIds(const Identifier& id) const;

  BooleanProperty m_testMDT;
  BooleanProperty m_testCSC;
  BooleanProperty m_testRPC;
  BooleanProperty m_testTGC;

  ServiceHandle<ActiveStoreSvc> m_activeStore;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  mutable std::atomic<long long> m_deltaUser{0};
  mutable std::atomic<long long> m_deltaKernel{0};
  mutable std::atomic<long long> m_deltaElapsed{0};
  mutable std::atomic<int> m_nTries{0};

};

#endif



