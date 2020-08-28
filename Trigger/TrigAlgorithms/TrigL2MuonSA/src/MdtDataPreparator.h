/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MDTDATAPREPARATOR_H
#define  TRIGL2MUONSA_MDTDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "IRegionSelector/IRegSelTool.h"
#include "Identifier/IdentifierHash.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "TgcData.h"
#include "MdtData.h"
#include "RecMuonRoIUtils.h"
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

    StatusCode prepareData(const LVL1::RecMuonRoI*           p_roi,
			   const TrigRoiDescriptor*          p_roids,
			   const TrigL2MuonSA::RpcFitResult& rpcFitResult,
			   TrigL2MuonSA::MuonRoad&           muonRoad,
			   TrigL2MuonSA::MdtRegion&          mdtRegion,
			   TrigL2MuonSA::MdtHits&            mdtHits_normal,
			   TrigL2MuonSA::MdtHits&            mdtHits_overlap);

    StatusCode prepareData(const LVL1::RecMuonRoI*           p_roi,
			   const TrigRoiDescriptor*          p_roids,
			   const TrigL2MuonSA::TgcFitResult& tgcFitResult,
			   TrigL2MuonSA::MuonRoad&           muonRoad,
			   TrigL2MuonSA::MdtRegion&          mdtRegion,
			   TrigL2MuonSA::MdtHits&            mdtHits_normal,
			   TrigL2MuonSA::MdtHits&            mdtHits_overlap);

    void setRpcGeometry(bool use_rpc) {m_mdtRegionDefiner->setRpcGeometry(use_rpc);};
    void setMdtDataCollection(bool use_mdtcsm){m_use_mdtcsm = use_mdtcsm;};
    void setRoIBasedDataAccess(bool use_RoIBasedDataAccess){m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;};

  public:
    float etaMinChamber[11],etaMaxChamber[11],phiMinChamber[11],phiMaxChamber[11];

  private:

    StatusCode getMdtHits(const LVL1::RecMuonRoI* p_roi,
			  const TrigRoiDescriptor* p_roids, 
			  const TrigL2MuonSA::MdtRegion& mdtRegion,
			  TrigL2MuonSA::MuonRoad& muonRoad,
			  TrigL2MuonSA::MdtHits& mdtHits_normal,
			  TrigL2MuonSA::MdtHits& mdtHits_overlap);

    void getMdtIdHashesBarrel(const TrigL2MuonSA::MdtRegion& mdtRegion,
			std::vector<IdentifierHash>& mdtIdHashes_normal,
			std::vector<IdentifierHash>& mdtIdHashes_overlap);

    void getMdtIdHashesEndcap(const TrigL2MuonSA::MdtRegion& mdtRegion,
			std::vector<IdentifierHash>& mdtIdHashes_normal,
			std::vector<IdentifierHash>& mdtIdHashes_overlap);

    StatusCode getMdtCsm(const MdtCsmContainer* pMdtCsmContainer,
			 const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& v_robFragments,
			 const std::vector<IdentifierHash>& v_idHash,
			 std::vector<const MdtCsm*>& v_mdtCsms);

    bool decodeMdtCsm(const MdtCsm* csm, TrigL2MuonSA::MdtHits& mdtHits, const TrigL2MuonSA::MuonRoad& muonRoad, const MuonGM::MuonDetectorManager* muDetMgr);
    uint32_t get_system_id (unsigned short int SubsystemId) const;

    StatusCode collectMdtHitsFromPrepData(const std::vector<IdentifierHash>& v_idHash,
					  std::vector<uint32_t>& v_robIds,
					  TrigL2MuonSA::MdtHits& mdtHits,
					  const TrigL2MuonSA::MuonRoad& muonRoad,
            const MuonGM::MuonDetectorManager* muDetMgr);

    void initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl);

  private:

    // Tools for the Raw data conversion
    ToolHandle<Muon::IMuonRawDataProviderTool>  m_mdtRawDataProvider {
      this, "MDT_RawDataProvider", "Muon::MDT_RawDataProviderTool"};


    // Geometry Services
    const MuonGM::MdtReadoutElement* m_mdtReadout {nullptr};
    const MuonGM::MuonStation* m_muonStation {nullptr};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    IdentifierHash m_hash_id;

    // Region Selector
    ToolHandle<IRegSelTool> m_regionSelector;

    // ROB DataProvider
    ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

    // Utils
    TrigL2MuonSA::RecMuonRoIUtils m_recMuonRoIUtils;

    // MdtRegionDefiner
    ToolHandle<MdtRegionDefiner> m_mdtRegionDefiner {
      this, "MdtRegionDefiner", "TrigL2MuonSA::MdtRegionDefiner"};

    // handles to data access
    ToolHandle<Muon::IMuonRdoToPrepDataTool> m_mdtPrepDataProvider {
      this, "MdtPrepDataProvider", "Muon::MdtRdoToPrepDataTool/MdtPrepDataProviderTool"};

    SG::ReadHandleKey<MdtCsmContainer> m_mdtCsmContainerKey{
	this, "MDTCSMContainer", "MDTCSM", "Name of the MDTRDO to read in"};
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtPrepContainerKey{
	this, "MDTPrepDataContainer","MDT_DriftCircles", "Name of the MDTContainer to read in"};
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 

    // Flag to decide if we need to run the actual decoding (in MT setup, we can use offline code for this)
    Gaudi::Property<bool> m_doDecoding{ this, "DoDecoding", true, "Flag to decide if we need to do decoding of the MDTs" };

    // Flag to decide if we need to decode BS or not (e.g. not needed for MC)
    Gaudi::Property<bool> m_decodeBS { this, "DecodeBS", true, "Flag to decide whether ot not to run BS->RDO decoding" };

    bool m_use_mdtcsm;
    bool m_use_RoIBasedDataAccess;
    bool m_BMGpresent;
    int  m_BMGid;
    std::map<Identifier, std::vector<Identifier> > m_DeadChannels;

    SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};

  };

} // namespace TrigL2MuonSA

#endif  //
