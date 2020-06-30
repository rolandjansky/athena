/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCDIGITTORPCRDO_H
#define RPCDIGITTORPCRDO_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"

#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "RPC_CondCabling/RpcCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonByteStreamCnvTest/RpcByteStreamDecoder.h"

#include "TrigT1RPClogic/RPCsimuData.h"
#include "TrigT1RPClogic/CMAdata.h"
#include "TrigT1RPClogic/RPCbytestream.h"
#include "TrigT1RPChardware/MatrixReadOutStructure.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcPad.h"
#include "MuonDigitContainer/RpcDigitContainer.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"

/////////////////////////////////////////////////////////////////////////////

class RpcDigitToRpcRDO : public AthReentrantAlgorithm {

public:

  RpcDigitToRpcRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RpcDigitToRpcRDO() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:

  //--- Migrated from RpcByteStreamDecoder.
  typedef std::map < int, PADreadout, std::less <int> > PAD_Readout;
  //Pad and Matrix decoding functions
  RpcPad* decodePad(PADreadout& pad, const RpcCablingCondData* readCdo) const;
  RpcCoinMatrix * decodeMatrix(MatrixReadOut* matrix, Identifier& id) const ;

  StatusCode fill_RPCdata(RPCsimuData& data, const EventContext& ctx, const RpcCablingCondData*) const;

  // NOTE: although this function has no clients in release 22, currently the Run2 trigger simulation is still run in
  //       release 21 on RDOs produced in release 22. Since release 21 accesses the TagInfo, it needs to be written to the
  //       RDOs produced in release 22. The fillTagInfo() function thus needs to stay in release 22 until the workflow changes
  StatusCode fillTagInfo() const;

  const MuonGM::MuonDetectorManager* m_MuonMgr;

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
  SG::ReadCondHandleKey<RpcCablingCondData> m_readKey{this, "ReadKey", "RpcCablingCondData", "Key of RpcCablingCondData"};

  SG::WriteHandleKey<RpcPadContainer> m_padContainerKey{this,"OutputObjectName","RPCPAD","WriteHandleKey for Output RpcPadContainer"};
  SG::ReadHandleKey<RpcDigitContainer> m_digitContainerKey{this,"InputObjectName","RPC_DIGITS","ReadHandleKey for Input RpcDigitContainer"};
  
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
};

#endif
