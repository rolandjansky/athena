/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPCDATAPREPARATOR_H
#define  TRIGL2MUONSA_RPCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"

#include "../src/RpcData.h"
#include "../src/RpcFitResult.h"
#include "../src/RpcPatFinder.h"
#include "../src/RecMuonRoIUtils.h"
#include "../src/RpcClusterPreparator.h"
#include "IRegionSelector/IRegSelTool.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"

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
			     const ToolHandle<RpcPatFinder>*   rpcPatFinder) const;

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
      SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_rpcPrepContainerKey{
       this, "RpcPrepDataContainer", "RPC_Measurements", "Name of the RPCContainer to read in"};

      //for multi-track SA mode
      ToolHandle<RpcClusterPreparator>  m_clusterPreparator{this, "RpcClusterPreparator", "TrigL2MuonSA::RpcClusterPreparator"};

      // Declare the keys used to access the data: one for reading and one
      // for writing.
      bool m_use_RoIBasedDataAccess;

      Gaudi::Property< bool > m_emulateNoRpcHit { this, "EmulateNoRpcHit", false, "Flag for emulation of no RPC hit events" };
      bool m_doMultiMuon{false};
};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_RPCDATAPREPARATOR_H
