/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestMuonIdHelpers_H
#define TestMuonIdHelpers_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

#include <string>
#include <vector>

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

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  SG::ReadHandleKey<MdtDigitContainer> m_mdtDigitContKey{this,"MdtDigits","MDT_DIGITS","ReadHandleKey for Input MdtDigitContainer"};
  SG::ReadHandleKey<RpcDigitContainer> m_rpcDigitContKey{this,"RpcDigits","RPC_DIGITS","ReadHandleKey for Input RpcDigitContainer"};
  SG::ReadHandleKey<CscDigitContainer> m_cscDigitContKey{this,"CscDigits","CSC_DIGITS","ReadHandleKey for Input CscDigitContainer"};
  SG::ReadHandleKey<TgcDigitContainer> m_tgcDigitContKey{this,"TgcDigits","TGC_DIGITS","ReadHandleKey for Input TgcDigitContainer"};

  mutable std::atomic<long long> m_deltaUser{0};
  mutable std::atomic<long long> m_deltaKernel{0};
  mutable std::atomic<long long> m_deltaElapsed{0};
  mutable std::atomic<int> m_nTries{0};

};

#endif



