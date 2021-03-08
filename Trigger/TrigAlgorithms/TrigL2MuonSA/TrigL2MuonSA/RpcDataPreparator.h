/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPCDATAPREPARATOR_H
#define  TRIGL2MUONSA_RPCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "MuonRDO/RpcPadContainer.h"

#include "../src/RpcData.h"
#include "../src/RpcFitResult.h"
#include "../src/RpcPatFinder.h"
#include "../src/RecMuonRoIUtils.h"
#include "../src/RpcClusterPreparator.h"
#include "IRegionSelector/IRegSelTool.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"

#include "MuonRecToolInterfaces/IMuonCombinePatternTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

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
			     TrigL2MuonSA::RpcHits&      rpcHits,
                             TrigL2MuonSA::RpcLayerHits& rpcLayerHits,
			     ToolHandle<RpcPatFinder>*   rpcPatFinder);

      //for multi-track SA mode
      StatusCode prepareData(const TrigRoiDescriptor*         p_roids,
                             TrigL2MuonSA::RpcLayerClusters&  rpcLayerClusters,
                             const ToolHandle<ClusterPatFinder>*    clusterPatFinder) const;

      void setRoIBasedDataAccess(bool use_RoIBasedDataAccess);

      void setMultiMuonTrigger( const bool multiMuonTrigger );

 private:
      ToolHandle<IRegSelTool> m_regionSelector{this,"RegSel_RPC","RegSelTool/RegSelTool_RPC"};
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_recRPCRoiTool{ this, "TrigT1RPCRecRoiTool", "LVL1::TrigT1RPCRecRoiTool/TrigT1RPCRecRoiTool"};

      // handles to the RoI driven data access
      ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataProviderTool{
       this, "RpcRawDataProvider", "Muon::RPC_RawDataProviderTool/RPC_RawDataProviderTool"};

      ToolHandle<Muon::IMuonRdoToPrepDataTool> m_rpcPrepDataProvider{
       this, "RpcPrepDataProvider", "Muon::RpcRdoToPrepDataTool/RpcPrepDataProviderTool"};

      SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_rpcPrepContainerKey{
       this, "RpcPrepDataContainer", "RPC_Measurements", "Name of the RPCContainer to read in"};

      //for multi-track SA mode
      ToolHandle<RpcClusterPreparator>  m_clusterPreparator{this, "RpcClusterPreparator", "TrigL2MuonSA::RpcClusterPreparator"};

      // Declare the keys used to access the data: one for reading and one
      // for writing.
      bool m_use_RoIBasedDataAccess;

      // Flag to decide if we need to run the actual decoding (in MT setup, we can use offline code for this)
      Gaudi::Property<bool> m_doDecoding{ this, "DoDecoding", true, "Flag to decide if we need to do decoding of the MDTs" };

      // Flag to decide whether or not to run BS decoding
      Gaudi::Property< bool > m_decodeBS { this, "DecodeBS", true, "Flag to decide whether or not to run BS->RDO decoding" };
      Gaudi::Property< bool > m_emulateNoRpcHit { this, "EmulateNoRpcHit", false, "Flag for emulation of no RPC hit events" };
      bool m_doMultiMuon{false};
};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_RPCDATAPREPARATOR_H
