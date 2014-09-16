/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MDTREGIONDEFINER_H
#define  TRIGL2MUONSA_MDTREGIONDEFINER_H

#include <string>

#include "GaudiKernel/MsgStream.h"

#include "MdtCalibSvc/MdtCalibrationSvc.h"

#include "MDTcabling/IMDTcablingSvc.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigL2MuonSA/TgcFit.h"
#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/TgcFitResult.h"
#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/MuonRoad.h"
#include "TrigL2MuonSA/MdtRegion.h"

namespace TrigL2MuonSA {


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

  class MdtRegionDefiner
  {
  public:
    MdtRegionDefiner(MsgStream* msg);
    MdtRegionDefiner(MsgStream* msg, const MDTGeometry* mdtGeometry);
    ~MdtRegionDefiner(void);
    
    void setMdtGeometry(const MDTGeometry* mdtGeometry);
    
  public:
    inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }
    
    StatusCode getMdtRegions(const LVL1::RecMuonRoI* p_roi,
			     const TrigL2MuonSA::RpcFitResult& rpcFitResult,
			     TrigL2MuonSA::MuonRoad& muonRoad,
			     TrigL2MuonSA::MdtRegion& mdtRegion);
    
    StatusCode getMdtRegions(const LVL1::RecMuonRoI*    p_roi,
			     const TrigL2MuonSA::TgcFitResult& tgcFitResult,
			     TrigL2MuonSA::MuonRoad& muonRoad,
			     TrigL2MuonSA::MdtRegion& mdtRegion);
    
  private:
    inline MsgStream& msg() const { return *m_msg; }
    StatusCode prepareTgcPoints(const TrigL2MuonSA::TgcHits& tgcHits);
    void find_barrel_road_dim(float max_road, float aw, float bw,
			      float rMmin,float rMax,float *zMin,float *zMax);
    void find_endcap_road_dim(float road,float aw, float bw, float zMin,
			      float zMax,float *rMin,float *rMax);
    void find_eta_min_max(float zMin, float rMin,
			  float zMax, float rMax,
			  float& etaMin, float& etaMax);
    void find_phi_min_max(float phiMiddle, float& phiMin, float& phiMax);
        
    StatusCode computePhi(const LVL1::RecMuonRoI*    p_roi,
			  const TrigL2MuonSA::RpcFitResult& rpcFitResult,
			  const TrigL2MuonSA::MdtRegion&    mdtRegion,
			  TrigL2MuonSA::MuonRoad&           muonRoad);
      
    StatusCode computePhi(const LVL1::RecMuonRoI*    p_roi,
			  const TrigL2MuonSA::TgcFitResult& tgcFitResult,
			  const TrigL2MuonSA::MdtRegion&    mdtRegion,
			  TrigL2MuonSA::MuonRoad&           muonRoad);

  private:
    MsgStream* m_msg;
    const MDTGeometry*     m_mdtGeometry;
    const IMDTcablingSvc*  m_mdtCablingSvc;
    
    TrigL2MuonSA::TgcFit::PointArray m_tgcStripMidPoints;  // List of TGC strip middle station points.
    TrigL2MuonSA::TgcFit::PointArray m_tgcWireMidPoints;   // List of TGC wire middle station points.
    TrigL2MuonSA::TgcFit::PointArray m_tgcStripInnPoints;  // List of TGC strip inner station points.
    TrigL2MuonSA::TgcFit::PointArray m_tgcWireInnPoints;   // List of TGC wire inner station points.
  };
  
  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
}

#endif // TRIGL2MUONSA_TESTCLASS_H
