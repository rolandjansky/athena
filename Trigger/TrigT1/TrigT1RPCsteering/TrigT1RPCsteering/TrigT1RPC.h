/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCMuonTrigger_H
#define RPCMuonTrigger_H

#ifndef LVL1_STANDALONE

#include <vector>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/DataHandle.h"
#include "Identifier/Identifier.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "RPCgeometry/IRPCgeometrySvc.h"

#include "TrigT1RPClogic/RPCsimuData.h"
#include "TrigT1RPClogic/CMAdata.h"
#include "TrigT1RPClogic/PADdata.h"
#include "TrigT1RPClogic/SLdata.h"
#include "TrigT1RPClogic/RPCbytestream.h"

#include "TrigT1RPCmonitoring/TrigEfficiency.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#define DEFAULT_L1MuctpiStoreLocationRPC "/Event/L1MuctpiStoreRPC"

class ActiveStoreSvc;
class MsgStream;
class RpcIdHelper;

/////////////////////////////////////////////////////////////////////////////

class TrigT1RPC : public Algorithm {

public:

  TrigT1RPC (const std::string& name, ISvcLocator* pSvcLocator);
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
  BooleanProperty m_logic_emulation;       // flag to activate logic emulation
  BooleanProperty m_hardware_emulation;    // flag to activate hardware emula.
  BooleanProperty m_bytestream_production; // flag to produce RPC byte stream
  StringProperty  m_bytestream_file;       // name for the bytestream file

  BooleanProperty m_data_detail;           // flag to printout detailed INFO on
                                           // processed data

  BooleanProperty m_detailedTW;            // flag for computing the triggwr 
                                           // windows in a detailed mode
  BooleanProperty m_EffMonitor;            // flag to  write the ntuple for
                                           // the trigger efficiency

  BooleanProperty m_time_geo_monitor;      // flag to analyze the time
                                           // correction
  
  BooleanProperty m_patch_for_rpc_time;    // flag for patching the RPC time
  
  IntegerProperty m_max_muon;              // max number of muon track in the monitor ntuple
  IntegerProperty m_max_roi;               // max number of trigger roi in the monitor ntuple

  StringProperty m_key_for_truth;          // key for retrieving the Montecarlo
                                           // truth

  unsigned int m_simu_code;  

  StatusCode fill_RPCdata(RPCsimuData&);

private:

  ActiveStoreSvc*                      m_activeStore;
  ServiceHandle<StoreGateSvc>          m_EvtStore;
  
  const MuonGM::MuonDetectorManager*   m_MuonMgr;
  const RpcIdHelper*                   m_rpcId;

  ServiceHandle <IRPCcablingServerSvc> m_cabling_getter;
  const IRPCcablingSvc*                m_cabling;
  ServiceHandle <IRPCgeometrySvc>      m_geometry;
    
  TrigEfficiency*       m_TrigEfficiency;
  
  NTuple::Tuple* m_ntuple;
  NTuple::Item<int> m_rpc_stationType;
  NTuple::Item<int> m_rpc_stationEta;
  NTuple::Item<int> m_rpc_stationPhi;
  NTuple::Item<int> m_rpc_doubletR;
  NTuple::Item<int> m_rpc_doubletZ;
  NTuple::Item<int> m_rpc_doubletP;
  NTuple::Item<int> m_rpc_gasGap;
  NTuple::Item<int> m_rpc_measurePhi;
  NTuple::Item<int> m_rpc_strip;
  NTuple::Item<long>  m_rpc_code;
  NTuple::Item<float> m_rpc_time;
  NTuple::Item<float> m_rpc_geomodel_x;
  NTuple::Item<float> m_rpc_geomodel_y;
  NTuple::Item<float> m_rpc_geomodel_z; 
  NTuple::Item<float> m_rpc_standalone_x;
  NTuple::Item<float> m_rpc_standalone_y;
  NTuple::Item<float> m_rpc_standalone_z;
};

#endif  // end of LVL1_STANDALONE

#endif
