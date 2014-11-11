/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTDATAPREPARATOR_H
#define  TRIGL2MUONSA_MUFASTDATAPREPARATOR_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include "MDTcabling/IMDTcablingSvc.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "RegionSelector/IRegSelSvc.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigL2MuonSA/MuFastDataPreparatorOptions.h"
#include "TrigL2MuonSA/RpcDataPreparator.h"
#include "TrigL2MuonSA/RpcRoadDefiner.h"
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

class MuFastDataPreparator: public AlgTool
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
			 TrigL2MuonSA::RpcHits&      rpcHits,
			 TrigL2MuonSA::MuonRoad&     muonRoad,
			 TrigL2MuonSA::MdtRegion&    mdtRegion,
			 TrigL2MuonSA::RpcFitResult& rpcFitResult,
			 TrigL2MuonSA::MdtHits&      mdtHits_normal,
			 TrigL2MuonSA::MdtHits&      mdtHits_overlap);
  
  StatusCode prepareData(const LVL1::RecMuonRoI*     p_roi,
			 TrigL2MuonSA::TgcHits&      tgcHits,
			 TrigL2MuonSA::MuonRoad&     muonRoad,
			 TrigL2MuonSA::MdtRegion&    mdtRegion,
			 TrigL2MuonSA::TgcFitResult& tgcFitResult,
			 TrigL2MuonSA::MdtHits&      mdtHits_normal,
			 TrigL2MuonSA::MdtHits&      mdtHits_overlap,
			 TrigL2MuonSA::CscHits&      cscHits);
  
  /** @brief Pointer to MsgStream.*/
  MsgStream* m_msg;
  
  /**
   * @brief Accessor method for the MsgStream.
   * @return handle to the MsgStream.
   */
  inline MsgStream& msg() const { return *m_msg; }
  
  /**
   * @brief Accessor method for the message level variable.
   * @return value of the message level for this algorithm.
   */
  inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }
  
  BooleanProperty  m_use_mcLUT;

  void setOptions(const TrigL2MuonSA::MuFastDataPreparatorOptions& options); 

  void setRoadWidthForFailure(double rWidth_RPC_Failed, double rWidth_TGC_Failed);

  StatusCode setGeometry(bool use_new_geometry);
  void setRpcGeometry(bool use_rpc);

  StatusCode setMCFlag(BooleanProperty  use_mcLUT);

  void setExtrapolatorTool(ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator);

 private:
  
  TrigL2MuonSA::MuFastDataPreparatorOptions m_options;

  IRegSelSvc*          m_regionSelector;
  const MdtIdHelper* m_mdtIdHelper;
  
 private:
  
  const TrigL2MuonSA::PtEndcapLUTSvc*    m_ptEndcapLUTSvc;

  // Tools
  ToolHandle<RpcDataPreparator>  m_rpcDataPreparator;
  ToolHandle<TgcDataPreparator>  m_tgcDataPreparator;
  ToolHandle<MdtDataPreparator>  m_mdtDataPreparator;
  ToolHandle<CscDataPreparator>  m_cscDataPreparator;
  
  TrigL2MuonSA::RpcRoadDefiner*  m_rpcRoadDefiner;
  TrigL2MuonSA::TgcRoadDefiner*  m_tgcRoadDefiner;
  
  ToolHandle<ITrigMuonBackExtrapolator>* m_backExtrapolatorTool;

  // Cabling
  ServiceHandle<IMDTcablingSvc>  m_mdtCablingSvc;
  BooleanProperty m_use_new_geometry;
  BooleanProperty m_use_rpc;

};
  
} // namespace TrigL2MuonSA

#endif  // MUFASTDATAPREPARATOR_H
