/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCDIGITTORPCRDO_H
#define RPCDIGITTORPCRDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
//#include "RPCgeometry/IRPCgeometrySvc.h"
#include "TrigT1RPClogic/RPCsimuData.h"
#include "TrigT1RPClogic/CMAdata.h"
#include "TrigT1RPClogic/RPCbytestream.h"

#include "MuonByteStreamCnvTest/RpcByteStreamDecoder.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcPad.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

class RpcIdHelper;

/////////////////////////////////////////////////////////////////////////////

class RpcDigitToRpcRDO : public AthAlgorithm {

public:

  RpcDigitToRpcRDO (const std::string& name, ISvcLocator* pSvcLocator);
  ~RpcDigitToRpcRDO();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  IntegerProperty m_fast_debug;            // bits for debugging "fast" algos
  IntegerProperty m_monitoring;            // bits for monitoring sequence

  IntegerProperty m_cma_debug;             // bits for debugging CMA simulation
  IntegerProperty m_pad_debug;             // bits for debugging PAD simulation
  IntegerProperty m_sl_debug;              // bits for debugging SL simulation

  IntegerProperty m_cma_ro_debug;          // bits for debugging CMA readout
  IntegerProperty m_pad_ro_debug;          // bits for debugging PAD readout
  IntegerProperty m_rx_ro_debug;           // bits for debugging Rx readout
  IntegerProperty m_sl_ro_debug;           // bits for debugging SL readout

  IntegerProperty m_cma_rostruct_debug;    // bits for debugging CMA ro struct
  IntegerProperty m_pad_rostruct_debug;    // bits for debugging PAD ro struct
  IntegerProperty m_rx_rostruct_debug;     // bits for debugging Rx ro struct
  IntegerProperty m_sl_rostruct_debug;     // bits for debugging SL ro struct 

  BooleanProperty m_geometric_algo;        // flag to activate fast simu
  BooleanProperty m_geometric_algo_param;  // flag to compute fast simu par
  BooleanProperty m_detailed_algo;         // flag to activate detailed simu
  BooleanProperty m_detail_algo_param;     // flag to compute detailed simu par
  StringProperty  m_bytestream_file;       // name for the bytestream file

  BooleanProperty m_patch_for_rpc_time;    // flag for patching the RPC time

  BooleanProperty m_data_detail;           // flag to printout detailed INFO on
                                           // processed data

  //unsigned int m_simu_code;  

  StatusCode fill_RPCdata(RPCsimuData&);

protected:
  ServiceHandle<ActiveStoreSvc>  m_activeStore;
  const MuonGM::MuonDetectorManager* m_MuonMgr;
  StatusCode fillTagInfo() const;

  const IRPCcablingSvc* m_cabling;
  //  IRPCgeometrySvc*      m_geometry;

  RpcPadContainer*      m_padContainer;
  const RpcIdHelper*    m_rpcHelper;
  std::string  m_cablingType;
};

#endif
