/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCMuonTrigger_H
#define RPCMuonTrigger_H

#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/DataHandle.h"
#include "Identifier/Identifier.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "TrigT1RPClogic/RPCsimuData.h"
#include "TrigT1RPClogic/CMAdata.h"
#include "TrigT1RPClogic/PADdata.h"
#include "TrigT1RPClogic/SLdata.h"
#include "TrigT1RPClogic/RPCbytestream.h"

//#include "TrigT1RPCmonitoring/TrigEfficiency.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#define DEFAULT_L1MuctpiStoreLocationRPC "/Event/L1MuctpiStoreRPC"

class RpcIdHelper;

/////////////////////////////////////////////////////////////////////////////

class TrigT1RPC : public AthAlgorithm {

public:

  TrigT1RPC (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  IntegerProperty m_fast_debug;            // bits for debugging "fast" algos

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

    IntegerProperty m_firstBC_to_MUCTPI;   // First BC to be sent to muctpi, wrt to BC0, e.g. -1 meanst the BC before the trigger Bc
    IntegerProperty m_lastBC_to_MUCTPI;   // First BC to be sent to muctpi, wrt to BC0, e.g. -1 meanst the BC before the trigger Bc
    
  StatusCode fill_RPCdata(RPCsimuData&);

private:

    //  ActiveStoreSvc*                      m_activeStore;
    // ServiceHandle<StoreGateSvc>          m_EvtStore;
  
  const MuonGM::MuonDetectorManager*   m_MuonMgr;
  const RpcIdHelper*                   m_rpcId;

  ServiceHandle <IRPCcablingServerSvc> m_cabling_getter;
  const IRPCcablingSvc*                m_cabling;

};


#endif
