/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPCDATAPREPARATOR_H
#define  TRIGL2MUONSA_RPCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1RPCRecRoiSvc/RPCRecRoiSvc.h"
#include "MuonRDO/RpcPadContainer.h"

#include "../src/RpcData.h"
#include "../src/RpcFitResult.h"
#include "../src/RpcPatFinder.h"
#include "../src/RecMuonRoIUtils.h"
#include "RegionSelector/IRegSelSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"

#include "MuonRecToolInterfaces/IMuonCombinePatternTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "RPC_CondCabling/RpcCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {

class RpcDataPreparator: public AthAlgTool
{
   public:

      RpcDataPreparator(const std::string& type, 
			const std::string& name,
			const IInterface*  parent);
    
      virtual StatusCode initialize() override;

      StatusCode prepareData(const TrigRoiDescriptor*    p_roids,
			     unsigned int roiWord,
			     TrigL2MuonSA::RpcHits&      rpcHits,
			     ToolHandle<RpcPatFinder>*   rpcPatFinder);

      bool isFakeRoi() { return m_isFakeRoi; }

      void setRoIBasedDataAccess(bool use_RoIBasedDataAccess);

      void setMultiMuonTrigger( const bool multiMuonTrigger );

 private:
      ServiceHandle<IRegSelSvc> m_regionSelector;
      SG::ReadCondHandleKey<RpcCablingCondData> m_readKey{this, "ReadKey", "RpcCablingCondData", "Key of RpcCablingCondData"};
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      ServiceHandle<LVL1RPC::RPCRecRoiSvc> m_recRPCRoiSvc{ this, "RPCRecRoiSvc", "LVL1RPC::RPCRecRoiSvc"};

      // handles to the RoI driven data access
      ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataProviderTool{
       this, "RpcRawDataProvider", "Muon::RPC_RawDataProviderTool/RPC_RawDataProviderTool"};

      ToolHandle<Muon::IMuonRdoToPrepDataTool> m_rpcPrepDataProvider{
       this, "RpcPrepDataProvider", "Muon::RpcRdoToPrepDataTool/RpcPrepDataProviderTool"};

      SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_rpcPrepContainerKey{
       this, "RpcPrepDataContainer", "RPC_Measurements", "Name of the RPCContainer to read in"};

      // Declare the keys used to access the data: one for reading and one
      // for writing.
      bool m_use_RoIBasedDataAccess;
      bool m_isFakeRoi;

      // Flag to decide if we need to run the actual decoding (in MT setup, we can use offline code for this)
      Gaudi::Property<bool> m_doDecoding{ this, "DoDecoding", true, "Flag to decide if we need to do decoding of the MDTs" };

      // Flag to decide whether or not to run BS decoding
      Gaudi::Property< bool > m_decodeBS { this, "DecodeBS", true, "Flag to decide whether or not to run BS->RDO decoding" };
      Gaudi::Property< bool > m_emulateNoRpcHit { this, "EmulateNoRpcHit", false, "Flag for emulation of no RPC hit events" };
      bool m_doMultiMuon{false};
};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_RPCDATAPREPARATOR_H
