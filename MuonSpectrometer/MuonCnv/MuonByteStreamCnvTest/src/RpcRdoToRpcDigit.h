/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_RPCRDOTORPCDIGIT_H
#define MUONBYTESTREAMCNVTEST_RPCRDOTORPCDIGIT_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class RpcRdoToRpcDigit : public AthReentrantAlgorithm {

 public:

  RpcRdoToRpcDigit(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RpcRdoToRpcDigit() = default;

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

 private:
  StatusCode decodeRpc( const RpcPad *, RpcDigitContainer *, RpcDigitCollection*& ) const;
  ToolHandle<Muon::IRPC_RDO_Decoder>  m_rpcRdoDecoderTool{this,"rpcRdoDecoderTool","Muon::RpcRDO_Decoder",""};
  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
  ServiceHandle<IRPCcablingServerSvc> m_rpcCablingServerSvc{this, "RPCcablingServerSvc", "RPCcablingServerSvc", ""};
  const IRPCcablingSvc * m_rpcCabling{};
  SG::ReadHandleKey<RpcPadContainer> m_rpcRdoKey{this, "RpcRdoContainer", "RPCPAD", "Rpc RDO Input"};
  SG::WriteHandleKey<RpcDigitContainer> m_rpcDigitKey{this, "RpcDigitContainer", "RPC_DIGITS", "Rpc Digit Output"};

};

#endif

