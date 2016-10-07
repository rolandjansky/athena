/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "TrigL2MuonSA/MuFastDataPreparatorOptions.h"
#include "TrigL2MuonSA/RpcDataPreparator.h"
#include "TrigL2MuonSA/RpcRoadDefiner.h"
#include "TrigL2MuonSA/RpcPatFinder.h"
#include "TrigL2MuonSA/TgcDataPreparator.h"
#include "TrigL2MuonSA/TgcRoadDefiner.h"
#include "TrigL2MuonSA/MdtDataPreparator.h"
#include "TrigL2MuonSA/MdtRegion.h"
#include "TrigL2MuonSA/CscDataPreparator.h"
#include "TrigL2MuonSA/CscData.h"

#include "TrigMuonBackExtrapolator/ITrigMuonBackExtrapolator.h"
#include "TrigL2MuonSA/PtEndcapLUTSvc.h"
#include "RegionSelector/IRegSelSvc.h"

class StoreGateSvc;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {

class MuFastDataPreparator: public AthAlgTool
{
 public:
  
  static const InterfaceID& interfaceID();
  
  MuFastDataPreparator(const std::string& type, 
		       const std::string& name,
		       const IInterface*  parent);
  
  ~MuFastDataPreparator();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  
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
  
  BooleanProperty  m_use_mcLUT;

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

 protected:
  
  // Services
  ServiceHandle<LVL1RPC::RPCRecRoiSvc> m_recRPCRoiSvc;

 private:
  
  TrigL2MuonSA::MuFastDataPreparatorOptions m_options;

  IRegSelSvc*        m_regionSelector;
  const MdtIdHelper* m_mdtIdHelper;
  
 private:
  
  const TrigL2MuonSA::PtEndcapLUTSvc*    m_ptEndcapLUTSvc;

  // Tools
  ToolHandle<RpcDataPreparator>  m_rpcDataPreparator;
  ToolHandle<TgcDataPreparator>  m_tgcDataPreparator;
  ToolHandle<MdtDataPreparator>  m_mdtDataPreparator;
  ToolHandle<CscDataPreparator>  m_cscDataPreparator;
  
  ToolHandle<RpcRoadDefiner>     m_rpcRoadDefiner;
  ToolHandle<TgcRoadDefiner>     m_tgcRoadDefiner;
  ToolHandle<RpcPatFinder>       m_rpcPatFinder;
  
  ToolHandle<ITrigMuonBackExtrapolator>* m_backExtrapolatorTool;

  BooleanProperty m_use_rpc;

  bool m_isRpcFakeRoi;
};
  
} // namespace TrigL2MuonSA

#endif  // MUFASTDATAPREPARATOR_H
