/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPCDATAPREPARATOR_H
#define  TRIGL2MUONSA_RPCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "MuonRDO/RpcPadContainer.h"

#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/RpcPatFinder.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"
#include "RegionSelector/IRegSelSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "MuonRPC_Cabling/MuonRPC_CablingSvc.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"

#include "MuonRecToolInterfaces/IMuonCombinePatternTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"


class StoreGateSvc;
class ActiveStoreSvc;
class RpcIdHelper;

namespace HLT {
  class TriggerElement;
  class Algo;
}

namespace MuonGM {
  class MuonDetectorManager;
  class RpcReadoutElement;
}

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

 private:
      // Region Selector
      ServiceHandle<IRegSelSvc> m_regionSelector;

      // RPC cabling service
      const IRPCcablingSvc* m_rpcCabling;
      const CablingRPCBase* m_rpcCablingSvc;
      
      const MuonGM::MuonDetectorManager* m_muonMgr;
      // Muon Id Helpers
      const RpcIdHelper* m_rpcIdHelper;

      // handles to the RoI driven data access
      ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataProviderTool{
       this, "RpcRawDataProvider", "Muon::RPC_RawDataProviderTool/RPC_RawDataProviderTool"};

      ToolHandle<Muon::IMuonRdoToPrepDataTool> m_rpcPrepDataProvider{
       this, "RpcPrepDataProvider", "Muon::RpcRdoToPrepDataTool/RpcPrepDataProviderTool"};
      
      ToolHandle <Muon::MuonIdHelperTool> m_idHelperTool{"Muon::MuonIdHelperTool/MuonIdHelperTool"};

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
};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_RPCDATAPREPARATOR_H
