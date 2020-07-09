/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCMuonTrigger_H
#define RPCMuonTrigger_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "RPC_CondCabling/RpcCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "TrigT1RPClogic/RPCsimuData.h"
#include "TrigT1RPClogic/CMAdata.h"
#include "TrigT1RPClogic/PADdata.h"
#include "TrigT1RPClogic/SLdata.h"
#include "TrigT1RPClogic/RPCbytestream.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"

/////////////////////////////////////////////////////////////////////////////

class TrigT1RPC : public AthAlgorithm {

public:

  TrigT1RPC (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:
  IntegerProperty m_fast_debug{this,"FastDebug",0};            // bits for debugging "fast" algos
  
  IntegerProperty m_cma_debug{this,"CMAdebug",0};             // bits for debugging CMA simulation
  IntegerProperty m_pad_debug{this,"PADdebug",0};             // bits for debugging PAD simulation
  IntegerProperty m_sl_debug{this,"SLdebug",0};              // bits for debugging SL simulation
  
  IntegerProperty m_cma_ro_debug{this,"CMArodebug",0};          // bits for debugging CMA readout
  IntegerProperty m_pad_ro_debug{this,"PADrodebug",0};          // bits for debugging PAD readout
  IntegerProperty m_rx_ro_debug{this,"RXrodebug",0};           // bits for debugging Rx readout
  IntegerProperty m_sl_ro_debug{this,"SLrodebug",0};           // bits for debugging SL readout
  
  IntegerProperty m_cma_rostruct_debug{this,"CMArostructdebug",0};    // bits for debugging CMA ro struct
  IntegerProperty m_pad_rostruct_debug{this,"PADrostructdebug",0};    // bits for debugging PAD ro struct
  IntegerProperty m_rx_rostruct_debug{this,"RXrostructdebug",0};     // bits for debugging Rx ro struct
  IntegerProperty m_sl_rostruct_debug{this,"SLrostructdebug",0};     // bits for debugging SL ro struct 
  
  BooleanProperty m_geometric_algo{this,"Geometric",false};        // flag to activate fast simu
  BooleanProperty m_logic_emulation{this,"Logic",false};       // flag to activate logic emulation
  BooleanProperty m_hardware_emulation{this,"Hardware",true};    // flag to activate hardware emula.
  BooleanProperty m_bytestream_production{this,"RPCbytestream",false}; // flag to produce RPC byte stream
  StringProperty  m_bytestream_file{this,"RPCbytestreamFile",""};       // name for the bytestream file
  
  BooleanProperty m_data_detail{this,"DataDetail",false};           // flag to printout detailed INFO on
  // processed data
  
  IntegerProperty m_firstBC_to_MUCTPI{this,"firstBC_to_MUCTPI",-1};   // First BC to be sent to muctpi, wrt to BC0, e.g. -1 meanst the BC before the trigger Bc
  IntegerProperty m_lastBC_to_MUCTPI{this,"lastBC_to_MUCTPI",1};   // First BC to be sent to muctpi, wrt to BC0, e.g. -1 meanst the BC before the trigger Bc
  
  BooleanProperty m_useRun3Config{this,"useRun3Config",false};         // flag for using switch between Run3 and Run2 configurations
  
  StatusCode fill_RPCdata(RPCsimuData&, const RpcCablingCondData* readCdo, const MuonGM::MuonDetectorManager* muDetMgr);

 private:
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  SG::ReadCondHandleKey<RpcCablingCondData> m_readKey{this, "ReadKey", "RpcCablingCondData", "Key of RpcCablingCondData"};

  SG::ReadHandleKey<RpcDigitContainer> m_rpcDigitKey{this, "RPCDigitContainer", "RPC_DIGITS", "RPC Digit Input Container"};
  SG::WriteHandleKey<LVL1MUONIF::Lvl1MuCTPIInput> m_muctpiKey{this, "MuctpiLocationRPC", "L1MuctpiStoreRPC", "Location of muctpi for Rpc"};
  SG::WriteHandleKey<LVL1MUONIF::Lvl1MuCTPIInputPhase1> m_muctpiPhase1Key{this, "MuctpiPhase1LocationRPC", "L1MuctpiStoreRPC", "Location of muctpiPhase1 for Rpc"};
  
  ServiceHandle <IRPCcablingServerSvc> m_cabling_getter;
  const IRPCcablingSvc*                m_cabling;
};

#endif
