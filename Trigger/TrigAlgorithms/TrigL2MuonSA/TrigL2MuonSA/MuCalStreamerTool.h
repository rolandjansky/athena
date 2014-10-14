/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUCALSTREAMERTOOL_H
#define  TRIGL2MUONSA_MUCALSTREAMERTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TrigL2MuonSA/MdtData.h"
#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/TrackData.h"

////////////// tmp cabling - to be removed //////////////////
#include "MuonRPC_Cabling/MuonRPC_CablingSvc.h"
#include "RPCgeometry/IRPCgeometrySvc.h"
#include "RPCcablingInterface/RpcPadIdHash.h" 
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
/////////////////////////////////////////////////////////////


#include <fstream>
#include <string>

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace LVL1 {
  class RecMuonRoI;
}

namespace LVL2_MUON_CALIBRATION {
  class MdtCalibFragment;
  class RpcCalibFragment;
  class TgcCalibFragment;
}

class RpcPad;

class StoreGateSvc;

class ROBDataProviderSvc;

namespace TrigL2MuonSA {
  
  class MuCalStreamerTool: public AlgTool
  {
  public:
    
    static const InterfaceID& interfaceID();
    
  public:
    
    MuCalStreamerTool(const std::string& type, 
		      const std::string& name,
		      const IInterface*  parent);
    
    ~MuCalStreamerTool();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

    MsgStream* m_msg;
    inline MsgStream& msg() const { return *m_msg; }    
    inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }


    std::string instanceName() const {return m_algInstanceName;}
    void setInstanceName(std::string name) { m_algInstanceName = name; }

    // set the properties
    void setBufferName(std::string buffName) {m_calBufferName=buffName;}
    void setBufferSize(int buffSize) {m_calBufferSize=buffSize;}

    //
    // initialize the stream
    StatusCode openStream();

    //
    // finalize and close the stream
    StatusCode closeStream();
    
    //
    // create the fragment corresponding to an roi
    StatusCode createRoiFragment(const LVL1::RecMuonRoI* roi,
				 TrigL2MuonSA::TrackPattern& trackPattern,
				 TrigL2MuonSA::MdtHits& mdtHits,
				 TrigL2MuonSA::RpcHits& rpcHits);

  private:

    BooleanProperty m_writeToFile;

    // name of the calibration buffer or of the 
    // output file
    std::string  m_calBufferName;
    std::string m_algInstanceName;
    int m_calBufferSize;

    // output file 
    std::ofstream* m_outputFile;

    // Reference to StoreGateSvc;
    ServiceHandle<StoreGateSvc>    m_storeGate;
      

    // ROB DataProvider
    ROBDataProviderSvc* m_robDataProvider;

    // id of the circular buffer
    int m_cid;
    int m_calibEvent;

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    /// TMP RPC cabling ( to be moved to the new one ) 

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // Tools for the Raw data conversion
    ToolHandle<Muon::IMuonRawDataProviderTool>  m_rpcRawDataProvider;
    
    // L2-specific Geometry Svc
    ServiceHandle<IRPCgeometrySvc>  m_rpcGeometrySvc;
    
    // RPC cablings
    const IRPCcablingSvc*   m_iRpcCablingSvc;
    const CablingRPCBase*   m_rpcCabling;

    // pointer to the muon roi
    const LVL1::RecMuonRoI* m_roi;

    
    //
    // create the MDT fragment
    StatusCode createMdtFragment(TrigL2MuonSA::MdtHits& mdtHits,  
				 LVL2_MUON_CALIBRATION::MdtCalibFragment& mdtFragment, float phi);

    //
    // create the RPC fragment
    StatusCode createRpcFragment(LVL2_MUON_CALIBRATION::RpcCalibFragment& rpcFragment);

    //
    // create the TGC fragment
//    StatusCode createTgcFragment(TrigL2MuonSA::TgcHits& tgcHits,  
//				 LVL2_MUON_CALIBRATION::TgcCalibFragment& tgcFragment);

    StatusCode getRpcPad(unsigned int robId, unsigned short int subsystemID, 
			 unsigned short int sectorID, unsigned short int roiNumber, 
			 const RpcPad* rpcPad );

  };
  
} // namespace TrigL2MuonSA

#endif  // 
