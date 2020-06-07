/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_RPCRDOTORPCDIGIT_H
#define MUONBYTESTREAMCNVTEST_RPCRDOTORPCDIGIT_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "RPC_CondCabling/RpcCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class RpcRdoToRpcDigit : public AthReentrantAlgorithm {

 public:

  RpcRdoToRpcDigit(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RpcRdoToRpcDigit() = default;

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

 private:
  StatusCode decodeRpc(const RpcPad *, RpcDigitContainer *, RpcDigitCollection*&, const RpcCablingCondData* rpcCab) const;
  ToolHandle<Muon::IRPC_RDO_Decoder>  m_rpcRdoDecoderTool{this,"rpcRdoDecoderTool","Muon::RpcRDO_Decoder",""};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ServiceHandle<IRPCcablingServerSvc> m_rpcCablingServerSvc{this, "RPCcablingServerSvc", "RPCcablingServerSvc", ""};
  const IRPCcablingSvc * m_rpcCabling{};
  SG::ReadHandleKey<RpcPadContainer> m_rpcRdoKey{this, "RpcRdoContainer", "RPCPAD", "Rpc RDO Input"};
  SG::WriteHandleKey<RpcDigitContainer> m_rpcDigitKey{this, "RpcDigitContainer", "RPC_DIGITS", "Rpc Digit Output"};
  SG::ReadCondHandleKey<RpcCablingCondData> m_rpcReadKey{this, "RpcCablingKey", "RpcCablingCondData", "Key of RpcCablingCondData"};

};

#endif

