/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MDTREGIONDEFINER_H
#define  TRIGL2MUONSA_MDTREGIONDEFINER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"

#include "MdtCalibSvc/MdtCalibrationSvc.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigL2MuonSA/TgcFit.h"
#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/TgcFitResult.h"
#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/MuonRoad.h"
#include "TrigL2MuonSA/MdtRegion.h"

class MdtIdHelper;

namespace MuonGM {
     class MuonDetectorManager;
     class MdtReadoutElement;
     class MuonStation;
}

namespace TrigL2MuonSA {


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

  class MdtRegionDefiner: public AthAlgTool
  {
  public:
    static const InterfaceID& interfaceID();

    MdtRegionDefiner(const std::string& type, 
		     const std::string& name,
		     const IInterface*  parent);

    ~MdtRegionDefiner(void);
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

    // function using the new cabling/geometry
    void setMdtGeometry(const MdtIdHelper* mdtIdHelper, const MuonGM::MuonDetectorManager* muonMgr);
    void setRpcGeometry(bool use_rpc);
    
  public:
    StatusCode getMdtRegions(const LVL1::RecMuonRoI* p_roi,
			     const TrigL2MuonSA::RpcFitResult& rpcFitResult,
			     TrigL2MuonSA::MuonRoad& muonRoad,
			     TrigL2MuonSA::MdtRegion& mdtRegion);
    
    StatusCode getMdtRegions(const LVL1::RecMuonRoI*    p_roi,
			     const TrigL2MuonSA::TgcFitResult& tgcFitResult,
			     TrigL2MuonSA::MuonRoad& muonRoad,
			     TrigL2MuonSA::MdtRegion& mdtRegion);
    
  private:
    StatusCode prepareTgcPoints(const TrigL2MuonSA::TgcHits& tgcHits);
    void find_barrel_road_dim(float max_road, float aw, float bw,
			      float rMmin,float rMax,float *zMin,float *zMax);
    void find_endcap_road_dim(float road,float aw, float bw, float zMin,
			      float zMax,float *rMin,float *rMax);
    void find_eta_min_max(float zMin, float rMin,
			  float zMax, float rMax,
			  float& etaMin, float& etaMax);
    void find_phi_min_max(float phiMiddle, float& phiMin, float& phiMax);
        
    void find_station_sector(std::string name, int phi, bool& endcap, int& chamber, int& sector);   

    StatusCode computePhi(const LVL1::RecMuonRoI*    p_roi,
			  const TrigL2MuonSA::RpcFitResult& rpcFitResult,
			  const TrigL2MuonSA::MdtRegion&    mdtRegion,
			  TrigL2MuonSA::MuonRoad&           muonRoad);
      
    StatusCode computePhi(const LVL1::RecMuonRoI*    p_roi,
			  const TrigL2MuonSA::TgcFitResult& tgcFitResult,
			  const TrigL2MuonSA::MdtRegion&    mdtRegion,
			  TrigL2MuonSA::MuonRoad&           muonRoad);

  private:
    const MdtIdHelper* m_mdtIdHelper;
    const MuonGM::MuonDetectorManager* m_muonMgr;
    const MuonGM::MdtReadoutElement* m_mdtReadout;
    const MuonGM::MuonStation* m_muonStation;
    
    bool m_use_rpc;

    TrigL2MuonSA::TgcFit::PointArray m_tgcStripMidPoints;  // List of TGC strip middle station points.
    TrigL2MuonSA::TgcFit::PointArray m_tgcWireMidPoints;   // List of TGC wire middle station points.
    TrigL2MuonSA::TgcFit::PointArray m_tgcStripInnPoints;  // List of TGC strip inner station points.
    TrigL2MuonSA::TgcFit::PointArray m_tgcWireInnPoints;   // List of TGC wire inner station points.

  };
  
  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
}

#endif // TRIGL2MUONSA_TESTCLASS_H
