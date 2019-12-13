/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCDIGITTORPCRDO_H
#define RPCDIGITTORPCRDO_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "TrigT1RPClogic/RPCsimuData.h"
#include "TrigT1RPClogic/CMAdata.h"
#include "TrigT1RPClogic/RPCbytestream.h"

#include "MuonByteStreamCnvTest/RpcByteStreamDecoder.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcPad.h"
#include "MuonDigitContainer/RpcDigitContainer.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"

/////////////////////////////////////////////////////////////////////////////

class RpcDigitToRpcRDO : public AthReentrantAlgorithm {

public:

  RpcDigitToRpcRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RpcDigitToRpcRDO() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  StatusCode fill_RPCdata(RPCsimuData& data, const EventContext& ctx) const;

  StatusCode fillTagInfo() const;

  IntegerProperty m_fast_debug{this, "FastDebug", 0, "bits for debugging 'fast' algos"};
  IntegerProperty m_monitoring{this, "Monitoring", 0, "bits for monitoring sequence"};

  IntegerProperty m_cma_debug{this, "CMAdebug", 0, "bits for debugging CMA simulation"};
  IntegerProperty m_pad_debug{this, "PADdebug", 0, "bits for debugging PAD simulation"};
  IntegerProperty m_sl_debug{this, "SLdebug", 0, "bits for debugging SL simulation"};

  IntegerProperty m_cma_ro_debug{this, "CMArodebug", 0, "bits for debugging CMA readout"};
  IntegerProperty m_pad_ro_debug{this, "PADrodebug", 0, "bits for debugging PAD readout"};
  IntegerProperty m_rx_ro_debug{this, "RXrodebug", 0, "bits for debugging Rx readout"};
  IntegerProperty m_sl_ro_debug{this, "SLrodebug", 0, "bits for debugging SL readout"};

  IntegerProperty m_cma_rostruct_debug{this, "CMArostructdebug", 0, "bits for debugging CMA ro struct"};
  IntegerProperty m_pad_rostruct_debug{this, "PADrostructdebug", 0, "bits for debugging PAD ro struct"};
  IntegerProperty m_rx_rostruct_debug{this, "RXrostructdebug", 0, "bits for debugging Rx ro struct"};
  IntegerProperty m_sl_rostruct_debug{this, "SLrostructdebug", 0, "bits for debugging SL ro struct"};

  BooleanProperty m_geometric_algo{this, "Geometric", false, "flag to activate fast simu"};
  BooleanProperty m_geometric_algo_param{this, "GeometricParameters", false, "flag to compute fast simu par"};
  BooleanProperty m_detailed_algo{this, "Detailed", false, "flag to activate detailed simu"};
  BooleanProperty m_detail_algo_param{this, "DetailedParameters", false, "flag to compute detailed simu par"};
  StringProperty  m_bytestream_file{this, "RPCbytestreamFile", "", "name for the bytestream file"};

  BooleanProperty m_patch_for_rpc_time{this, "PatchForRpcTime", false, "flag for patching the RPC time"};

  BooleanProperty m_data_detail{this, "DataDetail", false, "flag to printout detailed INFO on processed data"};

  ServiceHandle<IRPCcablingSvc> m_cabling{this, "MuonRPC_CablingSvc", "MuonRPC_CablingSvc", ""};

  SG::WriteHandleKey<RpcPadContainer> m_padContainerKey{this,"OutputObjectName","RPCPAD","WriteHandleKey for Output RpcPadContainer"};
  SG::ReadHandleKey<RpcDigitContainer> m_digitContainerKey{this,"InputObjectName","RPC_DIGITS","ReadHandleKey for Input RpcDigitContainer"};
  const MuonGM::MuonDetectorManager* m_MuonMgr{};
  std::string  m_cablingType{"UNKNOWN"};
  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
};

#endif
