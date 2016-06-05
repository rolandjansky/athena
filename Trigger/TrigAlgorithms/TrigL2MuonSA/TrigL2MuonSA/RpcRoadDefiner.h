/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPCROADDEFINER_H
#define  TRIGL2MUONSA_RPCROADDEFINER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigMuonBackExtrapolator/ITrigMuonBackExtrapolator.h"

#include "MuonIdHelpers/MdtIdHelper.h"

#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/RpcPatFinder.h"
#include "TrigL2MuonSA/MuonRoad.h"
#include "TrigL2MuonSA/MdtRegion.h"
#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/BarrelRoadData.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

#include "RegionSelector/IRegSelSvc.h"

class StoreGateSvc;

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class RpcRoadDefiner: public AthAlgTool
{
 public:
  static const InterfaceID& interfaceID();

  RpcRoadDefiner(const std::string& type,
                 const std::string& name,
                 const IInterface*  parent);
  ~RpcRoadDefiner(void);

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  
 public:
  StatusCode defineRoad(const LVL1::RecMuonRoI*      p_roi,
			TrigL2MuonSA::MuonRoad&      muonRoad,
			TrigL2MuonSA::RpcHits&       rpcHits,
			ToolHandle<RpcPatFinder>*    rpcPatFinder,
			TrigL2MuonSA::RpcFitResult&  rpcFitResult,
			double                       roiEtaMinLow,
			double                       roiEtaMaxLow,
			double                       roiEtaMinHigh,
			double                       roiEtaMaxHigh);

  void setMdtGeometry(IRegSelSvc* regionSelector, const MdtIdHelper* mdtIdHelper);
  void setRoadWidthForFailure(double rWidth_RPC_Failed);
  void setRpcGeometry(bool use_rpc);

 protected:
  float f(float x, float c0, float c1, float c2, float c3) const;
  float fp(float x, float c33, float c22, float c1) const;

 private:
  const BarrelRoadData*  m_roadData;

  double m_rWidth_RPC_Failed;
  bool m_use_rpc;

  IRegSelSvc* m_regionSelector;
  const MdtIdHelper* m_mdtIdHelper;
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
 
 inline float TrigL2MuonSA::RpcRoadDefiner::f(float x, float c0, float c1, float c2, float c3) const
   {
     return c0 + x * (c1 + x * (c2 + x * c3)); // faster
   }
 
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
 inline float TrigL2MuonSA::RpcRoadDefiner::fp(float x, float c33, float c22, float c1) const
   {
     return c1 + x * (c22 + x * c33); // faster
   }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
}

#endif // TRIGL2MUONSA_RPCROADDEFINER_H
