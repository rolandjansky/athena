/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MDTDATAPREPARATOR_H
#define  TRIGL2MUONSA_MDTDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "IRegionSelector/IRegSelTool.h"
#include "Identifier/IdentifierHash.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "TgcData.h"
#include "MdtData.h"
#include "MdtRegionDefiner.h"

#include "RpcFitResult.h"
#include "TgcFitResult.h"

namespace MuonGM{
     class MdtReadoutElement;
     class MuonStation;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {

  class MdtDataPreparator: public AthAlgTool
  {
  public:

    static const InterfaceID& interfaceID();

  public:

    MdtDataPreparator(const std::string& type,
		      const std::string& name,
		      const IInterface*  parent);

    virtual StatusCode initialize() override;

  public:

    StatusCode prepareData(const TrigRoiDescriptor*          p_roids,
			   const TrigL2MuonSA::RpcFitResult& rpcFitResult,
			   TrigL2MuonSA::MuonRoad&           muonRoad,
			   TrigL2MuonSA::MdtRegion&          mdtRegion,
			   TrigL2MuonSA::MdtHits&            mdtHits_normal) const;

    StatusCode prepareData(const TrigRoiDescriptor*          p_roids,
			   const TrigL2MuonSA::TgcFitResult& tgcFitResult,
			   TrigL2MuonSA::MuonRoad&           muonRoad,
			   TrigL2MuonSA::MdtRegion&          mdtRegion,
			   TrigL2MuonSA::MdtHits&            mdtHits_normal) const;

    void setRpcGeometry(bool use_rpc) {m_mdtRegionDefiner->setRpcGeometry(use_rpc);};
    void setRoIBasedDataAccess(bool use_RoIBasedDataAccess){m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;};

  private:

    StatusCode getMdtHits(const TrigRoiDescriptor* p_roids,
			  TrigL2MuonSA::MuonRoad& muonRoad,
			  TrigL2MuonSA::MdtHits& mdtHits_normal) const;

    StatusCode collectMdtHitsFromPrepData(const std::vector<IdentifierHash>& v_idHash,
					  TrigL2MuonSA::MdtHits& mdtHits,
					  const TrigL2MuonSA::MuonRoad& muonRoad,
					  const MuonGM::MuonDetectorManager* muDetMgr) const;

    void initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl);

  private:

    // Geometry Services
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    IdentifierHash m_hash_id;

    // Region Selector
    ToolHandle<IRegSelTool> m_regionSelector;

    // MdtRegionDefiner
    ToolHandle<MdtRegionDefiner> m_mdtRegionDefiner {
      this, "MdtRegionDefiner", "TrigL2MuonSA::MdtRegionDefiner"};

    // handles to data access
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtPrepContainerKey{
	this, "MDTPrepDataContainer","MDT_DriftCircles", "Name of the MDTContainer to read in"};
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"};

    bool m_use_RoIBasedDataAccess;
    bool m_BMGpresent;
    int  m_BMGid;
    std::map<Identifier, std::vector<Identifier> > m_DeadChannels;

  };

} // namespace TrigL2MuonSA

#endif  //
