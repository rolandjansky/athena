/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTDATAPREPARATOR_H
#define  TRIGL2MUONSA_MUFASTDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "RegionSelector/IRegSelSvc.h"

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigT1RPCRecRoiSvc/RPCRecRoiSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "MuFastDataPreparatorOptions.h"
#include "TrigL2MuonSA/RpcDataPreparator.h"
#include "RpcRoadDefiner.h"
#include "RpcPatFinder.h"
#include "TgcDataPreparator.h"
#include "TgcRoadDefiner.h"
#include "MdtDataPreparator.h"
#include "MdtRegion.h"
#include "CscDataPreparator.h"
#include "CscData.h"

#include "TrigMuonToolInterfaces/ITrigMuonBackExtrapolator.h"
#include "PtEndcapLUTSvc.h"

#include "RPC_CondCabling/RpcCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

namespace TrigL2MuonSA {

class MuFastDataPreparator: public AthAlgTool {
 public:
  
  MuFastDataPreparator(const std::string& type,
		       const std::string& name,
		       const IInterface*  parent);

  virtual StatusCode initialize() override;

 public:
  
  StatusCode prepareData(const LVL1::RecMuonRoI*     p_roi,
			 const TrigRoiDescriptor*    p_roids,
			 TrigL2MuonSA::RpcHits&      rpcHits,
			 TrigL2MuonSA::MuonRoad&     muonRoad,
			 TrigL2MuonSA::MdtRegion&    mdtRegion,
			 TrigL2MuonSA::RpcFitResult& rpcFitResult,
			 TrigL2MuonSA::MdtHits&      mdtHits_normal,
			 TrigL2MuonSA::MdtHits&      mdtHits_overlap);
  
  StatusCode prepareData(const LVL1::RecMuonRoI*     p_roi,
			 const TrigRoiDescriptor*    p_roids,
			 TrigL2MuonSA::TgcHits&      tgcHits,
			 TrigL2MuonSA::MuonRoad&     muonRoad,
			 TrigL2MuonSA::MdtRegion&    mdtRegion,
			 TrigL2MuonSA::TgcFitResult& tgcFitResult,
			 TrigL2MuonSA::MdtHits&      mdtHits_normal,
			 TrigL2MuonSA::MdtHits&      mdtHits_overlap,
			 TrigL2MuonSA::CscHits&      cscHits);
  
  void setOptions(const TrigL2MuonSA::MuFastDataPreparatorOptions& options);

  void setRoadWidthForFailure(double rWidth_RPC_Failed, double rWidth_TGC_Failed);

  StatusCode setGeometry(bool use_new_geometry);
  void setRpcGeometry(bool use_rpc);
  void setMdtDataCollection(bool use_mdtcsm);

  StatusCode setMCFlag(BooleanProperty  use_mcLUT);
  void setRoIBasedDataAccess(bool use_RoIBasedDataAccess_MDT,
			     bool use_RoIBasedDataAccess_RPC,
			     bool use_RoIBasedDataAccess_TGC,
			     bool use_RoIBasedDataAccess_CSC);

  void setExtrapolatorTool(ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator);

  bool isRpcFakeRoi() {return m_isRpcFakeRoi;}

  void setMultiMuonTrigger( const bool multiMuonTrigger ) {m_doMultiMuon = multiMuonTrigger;};

 private:
  TrigL2MuonSA::MuFastDataPreparatorOptions m_options;
  SG::ReadCondHandleKey<RpcCablingCondData> m_readKey{this, "ReadKey", "RpcCablingCondData", "Key of RpcCablingCondData"};

  ServiceHandle<IRegSelSvc> m_regionSelector;

  ServiceHandle<LVL1RPC::RPCRecRoiSvc> m_recRPCRoiSvc{this, "RPCRecRoiSvc", "LVL1RPC::RPCRecRoiSvc"};
  ToolHandle<RpcDataPreparator>   m_rpcDataPreparator{this, "RPCDataPreparator", "TrigL2MuonSA::RpcDataPreparator"};
  ToolHandle<TgcDataPreparator>   m_tgcDataPreparator{this, "TGCDataPreparator", "TrigL2MuonSA::TgcDataPreparator"};
  ToolHandle<MdtDataPreparator>   m_mdtDataPreparator{this, "MDTDataPreparator", "TrigL2MuonSA::MdtDataPreparator"};
  ToolHandle<CscDataPreparator>   m_cscDataPreparator{this, "CSCDataPreparator", "TrigL2MuonSA::CscDataPreparator"};

  ToolHandle<RpcRoadDefiner>      m_rpcRoadDefiner{"TrigL2MuonSA::RpcRoadDefiner"};
  ToolHandle<TgcRoadDefiner>      m_tgcRoadDefiner{"TrigL2MuonSA::TgcRoadDefiner"};
  ToolHandle<RpcPatFinder>        m_rpcPatFinder{"TrigL2MuonSA::RpcPatFinder"};
  
  ToolHandle<ITrigMuonBackExtrapolator>* m_backExtrapolatorTool{nullptr};

  bool m_use_rpc{false};
  bool m_isRpcFakeRoi{false};
  bool m_use_mcLUT{false};
  bool m_doMultiMuon{false};

};
  
} // namespace TrigL2MuonSA

#endif  // MUFASTDATAPREPARATOR_H
