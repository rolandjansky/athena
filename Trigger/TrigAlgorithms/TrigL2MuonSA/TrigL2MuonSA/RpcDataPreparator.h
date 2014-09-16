/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPCDATAPREPARATOR_H
#define  TRIGL2MUONSA_RPCDATAPREPARATOR_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRPC_Cabling/MuonRPC_CablingSvc.h"
#include "RPCgeometry/IRPCgeometrySvc.h"
#include "MDTcabling/IMDTcablingSvc.h"

#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"

#include "TrigT1RPClogic/RPCrawData.h"
#include "TrigT1RPClogic/CMAdata.h"

class StoreGateSvc;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {

class RpcDataPreparator: public AlgTool
{
   public:
      
      static const InterfaceID& interfaceID();

   public:

      RpcDataPreparator(const std::string& type, 
			const std::string& name,
			const IInterface*  parent);
    
      ~RpcDataPreparator();
    
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();
    
      StatusCode prepareData(const LVL1::RecMuonRoI*   p_roi,
			     TrigL2MuonSA::RpcHits&      rpcHits,
			     TrigL2MuonSA::RpcFitResult& rpcFitResult);

      inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }
      inline void setMsgLvl(const MSG::Level& level) { if(m_msg != 0) m_msg->setLevel(level); }
      // void setOptions(const TrigL2MuonSA::RpcDataPreparatorOptions& options) { m_options = options; };
      
 private:
      
      MsgStream* m_msg;
      inline MsgStream& msg() const { return *m_msg; }
      
      // internal methods
      StatusCode getRpcPad(unsigned int robId, unsigned short int subsystemID,
			   unsigned short int sectorID, unsigned short int roiNumber,
			   const RpcPad*& p_pad);

      StatusCode decodeRpcPad(const RpcPad*             p_rdo,
			      TrigL2MuonSA::RpcHits&      rpcHits,
			      TrigL2MuonSA::RpcFitResult& rpcFitResult);
 
      float stationBeta(RPCdigit& phi,
			RPCdigit& eta,
			unsigned short int SubsystemId,
			unsigned short int SectorId) const;
 private:
		       
      // Reference to StoreGateSvc;
      ServiceHandle<StoreGateSvc>    m_storeGateSvc;
      
      // Tools for the Raw data conversion
      ToolHandle<Muon::IMuonRawDataProviderTool>  m_rpcRawDataProvider;

      // L2-specific Geometry Svc
      ServiceHandle<IRPCgeometrySvc>  m_rpcGeometrySvc;

      // RPC cablings
      const IRPCcablingSvc*   m_iRpcCablingSvc;
      const CablingRPCBase*   m_rpcCabling;

      // MDT cablings
      ServiceHandle<IMDTcablingSvc>  m_iMdtCablingSvc;

      // ROB DataProvider
      ROBDataProviderSvc*  m_robDataProvider;

      // option
      // TrigL2MuonSA::RpcDataPreparatorOptions m_options;

      // utils
      TrigL2MuonSA::RecMuonRoIUtils m_recMuonRoIUtils;

      // Masking of the RPC uncabled channels
      bool m_maskUncCMAch;
};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_RPCDATAPREPARATOR_H
