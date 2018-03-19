/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MDTDATAPREPARATOR_H
#define  TRIGL2MUONSA_MDTDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "RegionSelector/IRegSelSvc.h"
#include "Identifier/IdentifierHash.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/MdtData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"
#include "TrigL2MuonSA/MdtRegionDefiner.h"

#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/TgcFitResult.h"

#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"

class StoreGateSvc;
class MdtIdHelper;
namespace MuonGM{
     class MuonDetectorManager;
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
    
    ~MdtDataPreparator();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();
    
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

    void setRpcGeometry(bool use_rpc);
    void setMdtDataCollection(bool use_mdtcsm);
    void setRoIBasedDataAccess(bool use_RoIBasedDataAccess);

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
    
    bool decodeMdtCsm(const MdtCsm* csm, TrigL2MuonSA::MdtHits& mdtHits, const TrigL2MuonSA::MuonRoad& muonRoad);
    uint32_t get_system_id (unsigned short int SubsystemId) const;
    
    StatusCode collectMdtHitsFromPrepData(const std::vector<IdentifierHash>& v_idHash,
					  std::vector<uint32_t>& v_robIds,
					  TrigL2MuonSA::MdtHits& mdtHits,
					  const TrigL2MuonSA::MuonRoad& muonRoad);

    void initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl);

  private:

    // Reference to StoreGateSvc;
    ServiceHandle<StoreGateSvc>    m_storeGateSvc;
    ActiveStoreSvc* m_activeStore;
    
    // Tools for the Raw data conversion
    ToolHandle<Muon::IMuonRawDataProviderTool>  m_mdtRawDataProvider;
    
    // Cabling
    MuonMDT_CablingSvc* m_mdtCabling;
    
    // Geometry Services
    const MuonGM::MuonDetectorManager* m_muonMgr;
    const MuonGM::MdtReadoutElement* m_mdtReadout;
    const MuonGM::MuonStation* m_muonStation;
    const MdtIdHelper* m_mdtIdHelper;
    IdentifierHash hash_id;
    
    // Region Selector
    IRegSelSvc*          m_regionSelector;
    
    // ROB DataProvider
    ROBDataProviderSvc*  m_robDataProvider;
    
    // Utils
    TrigL2MuonSA::RecMuonRoIUtils m_recMuonRoIUtils;
   
    //
    ToolHandle<MdtRegionDefiner>  m_mdtRegionDefiner;

    // handles to data access
    ToolHandle<Muon::IMuonRdoToPrepDataTool> m_mdtPrepDataProvider;

    bool m_use_mdtcsm;
    bool m_use_RoIBasedDataAccess;
    bool m_BMGpresent;
    int  m_BMGid;
    std::map<Identifier, std::vector<Identifier> > m_DeadChannels;

  };

} // namespace TrigL2MuonSA

#endif  // 
